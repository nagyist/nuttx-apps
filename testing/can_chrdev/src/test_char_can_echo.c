/****************************************************************************
 * apps/testing/can_chrdev/src/test_char_can_echo.c
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

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define CM_CLASS_CAN_DLC 8

/****************************************************************************
 * Public Data
 ****************************************************************************/

extern uint8_t g_can_chrdev0_txconfirm_cnt;
extern uint8_t g_can_chrdev1_txconfirm_cnt;

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void can_echo(int fd0, int fd1, struct can_msg_s *txmsg,
                     struct can_msg_s *rxmsg, int msgsize)
{
  ssize_t nbytes = write(fd0, txmsg, msgsize);

  assert_int_equal(msgsize, nbytes);
  sleep(1);

  nbytes = read(fd1, rxmsg, sizeof(struct can_msg_s));

  /* juage whether the msg that is belonged to fd1 is txconfirm msg */

  if (rxmsg->cm_hdr.ch_tcf)
    {
      g_can_chrdev1_txconfirm_cnt++;
      nbytes = read(fd1, rxmsg, sizeof(struct can_msg_s));
      assert_int_equal(memcmp(txmsg, rxmsg, msgsize), 0);
    }
  else
    {
      assert_int_equal(memcmp(txmsg, rxmsg, msgsize), 0);
    }

  nbytes = write(fd1, rxmsg, msgsize);
  assert_int_equal(msgsize, nbytes);
  sleep(1);
  nbytes = read(fd0, txmsg, sizeof(struct can_msg_s));

  /* juage whether the msg that is belonged to fd0 is txconfirm msg */

  if (txmsg->cm_hdr.ch_tcf)
    {
      g_can_chrdev0_txconfirm_cnt++;
      nbytes = read(fd0, txmsg, sizeof(struct can_msg_s));
      assert_int_equal(memcmp(rxmsg, txmsg, msgsize), 0);
    }
  else
    {
      assert_int_equal(memcmp(rxmsg, txmsg, msgsize), 0);
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: test_charcan_milti_echo
 * Description:
 *   This function send can message and check whether the sending result is
 *   right or not.
 ****************************************************************************/

void test_charcan_multi_echo(FAR void **state)
{
  FAR struct test_charcan_s *confs = (FAR struct test_charcan_s *)*state;
  uint8_t canmsg_databyte;
  size_t msgsize;
  int i;

  struct can_msg_s rxmsg =
    {
      0
    };

  assert_true(confs && confs->txmsg != NULL);

  for (i = 0; i < CAN_MSG_NUMBER; i++)
    {
      /* initilize confs->txmsg */

      memset(&(confs->txmsg[i].cm_hdr), 0, sizeof(struct can_hdr_s));
      confs->txmsg[i].cm_hdr.ch_id  = confs->can_id;
      confs->txmsg[i].cm_hdr.ch_dlc = CAN_MSG_NUMBER + i;
      confs->txmsg[i].cm_hdr.ch_edl = confs->txmsg[i].cm_hdr.ch_dlc >
                                      CM_CLASS_CAN_DLC ? 1 : 0;

      canmsg_databyte = cm_charcan_dlc2bytes(confs->txmsg[i].cm_hdr.ch_dlc);
      msgsize = CAN_MSGLEN(canmsg_databyte);

      /* check whether the sending result is right or not */

      can_echo(confs->fd[0], confs->fd[1], &confs->txmsg[i],
               &rxmsg, msgsize);
    }
}
