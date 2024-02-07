/****************************************************************************
 * apps/graphics/lvgl/lv_porting/lv_mouse_interface.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <debug.h>
#include <nuttx/input/mouse.h>
#include "lv_mouse_interface.h"

/****************************************************************************
 * Private Type Declarations
 ****************************************************************************/

struct mouse_obj_s
{
  int              fd;
  lv_indev_state_t last_state;
  lv_indev_drv_t   indev_drv;
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: mouse_read
 ****************************************************************************/

static void mouse_read(FAR lv_indev_drv_t *drv, FAR lv_indev_data_t *data)
{
  FAR struct mouse_obj_s *mouse_obj = drv->user_data;
  struct mouse_report_s sample;

  /* Read one sample */

  int nbytes = read(mouse_obj->fd, &sample,
                    sizeof(struct mouse_report_s));

  /* Handle unexpected return values */

  if (nbytes == sizeof(struct mouse_report_s))
    {
      const FAR lv_disp_drv_t *disp_drv = drv->disp->driver;
      lv_coord_t ver_max = disp_drv->ver_res - 1;
      lv_coord_t hor_max = disp_drv->hor_res - 1;

      data->point.x =
        LV_CLAMP(0,
                 data->point.x + (sample.x * CONFIG_LV_MOUSE_RATE),
                 hor_max);
      data->point.y =
        LV_CLAMP(0,
                 data->point.y + (sample.y * CONFIG_LV_MOUSE_RATE),
                 ver_max);

      uint8_t mouse_buttons = sample.buttons;

      if (mouse_buttons & MOUSE_BUTTON_1 || mouse_buttons & MOUSE_BUTTON_2 ||
          mouse_buttons & MOUSE_BUTTON_3)
        {
          mouse_obj->last_state = LV_INDEV_STATE_PR;
        }
      else
        {
          mouse_obj->last_state = LV_INDEV_STATE_REL;
        }
    }

  data->state = mouse_obj->last_state;
}

/****************************************************************************
 * Name: mouse_set_cursor
 ****************************************************************************/

static void mouse_set_cursor(FAR lv_indev_t *indev)
{
  FAR lv_obj_t *cursor_obj = lv_obj_create(lv_layer_sys());
  lv_obj_remove_style_all(cursor_obj);

  lv_coord_t size = 20;
  lv_obj_set_size(cursor_obj, size, size);
  lv_obj_set_style_translate_x(cursor_obj, -size / 2, 0);
  lv_obj_set_style_translate_y(cursor_obj, -size / 2, 0);
  lv_obj_set_style_radius(cursor_obj, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_opa(cursor_obj, LV_OPA_50, 0);
  lv_obj_set_style_bg_color(cursor_obj, lv_color_black(), 0);
  lv_obj_set_style_border_width(cursor_obj, 2, 0);
  lv_obj_set_style_border_color(cursor_obj,
                                lv_palette_main(LV_PALETTE_GREY), 0);
  lv_indev_set_cursor(indev, cursor_obj);
}

/****************************************************************************
 * Name: mouse_init
 ****************************************************************************/

static FAR lv_indev_t *mouse_init(int fd)
{
  FAR lv_indev_t *indev;
  FAR struct mouse_obj_s *mouse_obj;
  mouse_obj = calloc(1, sizeof(struct mouse_obj_s));
  LV_ASSERT_MALLOC(mouse_obj);

  if (mouse_obj == NULL)
    {
      LV_LOG_ERROR("mouse_obj_s malloc failed");
      return NULL;
    }

  mouse_obj->fd = fd;
  mouse_obj->last_state = LV_INDEV_STATE_REL;

  lv_indev_drv_init(&(mouse_obj->indev_drv));
  mouse_obj->indev_drv.type = LV_INDEV_TYPE_POINTER;
  mouse_obj->indev_drv.read_cb = mouse_read;
#if ( LV_USE_USER_DATA != 0 )
  mouse_obj->indev_drv.user_data = mouse_obj;
#else
#error LV_USE_USER_DATA must be enabled
#endif
  indev = lv_indev_drv_register(&(mouse_obj->indev_drv));

  /* Set cursor icon */

  mouse_set_cursor(indev);

  return indev;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: lv_mouse_interface_init
 *
 * Description:
 *   mouse interface initialization.
 *
 * Input Parameters:
 *   dev_path - input device path, set NULL to use the default path
 *
 * Returned Value:
 *   pointer to lv_indev_t on success; NULL on failure.
 *
 ****************************************************************************/

FAR lv_indev_t *lv_mouse_interface_init(FAR const char *dev_path)
{
  FAR const char *device_path = dev_path;
  FAR lv_indev_t *indev;
  int fd;

  if (device_path == NULL)
    {
      device_path = CONFIG_LV_MOUSE_INTERFACE_DEFAULT_DEVICEPATH;
    }

  LV_LOG_INFO("mouse %s opening", device_path);
  fd = open(device_path, O_RDONLY | O_NONBLOCK);
  if (fd < 0)
    {
      LV_LOG_ERROR("mouse %s open failed: %d", device_path, errno);
      return NULL;
    }

  LV_LOG_INFO("mouse %s open success", device_path);

  indev = mouse_init(fd);

  if (indev == NULL)
    {
      close(fd);
    }

  return indev;
}
