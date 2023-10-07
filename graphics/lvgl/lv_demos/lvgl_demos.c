/****************************************************************************
 * apps/graphics/lvgl/lv_demos/lvgl_demos.c
 *
 * Copyright (C) 2023 Xiaomi Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <unistd.h>

#include <lvgl/lvgl.h>
#include <lvgl/demos/lv_demos.h>

/****************************************************************************
 * Private Type Declarations
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: main or lv_demos_main
 *
 * Description:
 *
 * Input Parameters:
 *   Standard argc and argv
 *
 * Returned Value:
 *   Zero on success; a positive, non-zero value on failure.
 *
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  lv_nuttx_t info;
  info.fb_path = CONFIG_LV_FBDEV_INTERFACE_DEFAULT_DEVICEPATH;
  info.input_path = CONFIG_LV_TOUCHPAD_INTERFACE_DEFAULT_DEVICEPATH;
  info.need_wait_vsync = true;

  lv_init();

  lv_disp_t *disp = lv_nuttx_init(&info);

  if (disp == NULL)
    {
      LV_LOG_ERROR("lv_demos initialization failure!");
      return 1;
    }

  if (!lv_demos_create(&argv[1], argc - 1))
    {
      lv_demos_show_help();

      /* we can add custom demos here */

      goto demo_end;
    }

  while (1)
    {
      lv_timer_handler();
      usleep(10 * 1000);
    }

demo_end:
  lv_disp_remove(disp);
  lv_deinit();

  return 0;
}
