/****************************************************************************
 * apps/testing/can_chrdev/src/test_char_can_txconfirm.c
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

#include <assert.h>
#include <poll.h>
#include <unistd.h>
#include <string.h>

#include "charcantest.h"

uint8_t g_can_chrdev0_txconfirm_cnt = 0;
uint8_t g_can_chrdev1_txconfirm_cnt = 0;

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: test_char_can_txconfirm
 * Description:
 *   This function check whether the sent msg callback the txconfirm msg.
 ****************************************************************************/

void test_char_can_txconfirm(FAR void **state)
{
  assert_int_equal(g_can_chrdev0_txconfirm_cnt, CAN_MSG_NUMBER);
  assert_int_equal(g_can_chrdev1_txconfirm_cnt, CAN_MSG_NUMBER - 1);
  g_can_chrdev0_txconfirm_cnt = 0;
  g_can_chrdev1_txconfirm_cnt = 0;
}
