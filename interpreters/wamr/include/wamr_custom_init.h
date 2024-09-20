/****************************************************************************
 * apps/interpreters/wamr/include/wamr_custom_init.h
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

#ifndef __WAMR_CUSTOM_INIT_H
#define __WAMR_CUSTOM_INIT_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdbool.h>

#include "wasm_export.h"

/****************************************************************************
 * Public Functions Prototypes
 ****************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif
bool wamr_custom_init(RuntimeInitArgs *init_args);
#ifdef __cplusplus
}
#endif
bool wamr_libc_nuttx_register(void);

#endif /* __WAMR_CUSTOM_INIT_H */
