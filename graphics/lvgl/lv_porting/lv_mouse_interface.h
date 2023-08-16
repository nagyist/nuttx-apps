/****************************************************************************
 * apps/graphics/lvgl/lv_porting/lv_mouse_interface.h
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

#ifndef __LV_MOUSE_INTERFACE_H__
#define __LV_MOUSE_INTERFACE_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <lvgl/lvgl.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#if defined(CONFIG_LV_USE_MOUSE_INTERFACE)

/****************************************************************************
 * Type Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: lv_mouse_interface_init
 *
 * Description:
 *   Mouse interface initialization.
 *
 * Input Parameters:
 *   dev_path - input device path, set NULL to use the default path
 *
 * Returned Value:
 *   pointer to lv_indev_t on success; NULL on failure.
 *
 ****************************************************************************/

FAR lv_indev_t *lv_mouse_interface_init(FAR const char *dev_path);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* CONFIG_LV_USE_MOUSE_INTERFACE */

#endif /* __LV_MOUSE_INTERFACE_H__ */
