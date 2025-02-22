/****************************************************************************
 * apps/netutils/rpmsg_tun/rpmsg_tun_client.c
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

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "rpmsg_tun_util.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: main
 ****************************************************************************/

int main(int argc, char *argv[])
{
  int rpmsgfd;
  int tunfd;
  int ret;

  if (argc != 5)
    {
      fprintf(stderr, "Usage: %s <tunname> <ip> <mask> <cpu>\n", argv[0]);
      return -EINVAL;
    }

  tunfd = rpmsg_tun_setup(argv[1], argv[2], argv[3]);
  if (tunfd < 0)
    {
      return tunfd;
    }

  for (; ; )
    {
      rpmsgfd = rpmsg_tun_connect(argv[4], argv[1]);
      if (rpmsgfd < 0)
        {
          sleep(3);
          continue;
        }

      ret = rpmsg_tun_loop(tunfd, rpmsgfd, -1, NULL);
      close(rpmsgfd);
      if (ret < 0)
        {
          break;
        }
    }

  close(tunfd);
  return ret;
}
