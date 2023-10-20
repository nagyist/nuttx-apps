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
#ifdef CONFIG_LV_USE_NUTTX_LIBUV
#include <uv.h>
#endif

/****************************************************************************
 * Private Type Declarations
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

#ifdef CONFIG_LV_USE_NUTTX_LIBUV
static void lv_nuttx_uv_loop(uv_loop_t *loop, lv_disp_t *disp,
                             lv_indev_t *indev)
{
  lv_nuttx_uv_t uv_info;
  uv_loop_init(loop);

  lv_memset(&uv_info, 0, sizeof(uv_info));
  uv_info.loop = loop;
  uv_info.disp = disp;
  uv_info.indev = indev;

  lv_nuttx_uv_init(&uv_info);
  uv_run(loop, UV_RUN_DEFAULT);
}
#endif

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
  lv_nuttx_dsc_t info;
  lv_nuttx_result_t result;

#ifdef CONFIG_LV_USE_NUTTX_LIBUV
  uv_loop_t ui_loop;
#endif

  lv_init();

  lv_nuttx_dsc_init(&info);
  lv_nuttx_init(&info, &result);

  if (result.disp == NULL)
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

#ifdef CONFIG_LV_USE_NUTTX_LIBUV
  lv_nuttx_uv_loop(&ui_loop, result.disp, result.indev);
#else
  while (1)
    {
      lv_timer_handler();
      usleep(10 * 1000);
    }
#endif

demo_end:
  lv_disp_remove(result.disp);
  lv_deinit();

  return 0;
}
