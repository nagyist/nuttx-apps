/****************************************************************************
 * apps/netutils/rpmsg_tun/rpmsg_tun_server.c
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
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "rpmsg_tun_util.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: rpmsg_tun_netlink_loop
 *
 * Description:
 *   Poll tunfd and rpmsgfd and nlfd when fds return POLLIN
 *   then read buffer from tunfd and send to rpmsgfd
 *   or read buffer from rpmsgfd and send to tunfd
 *
 * Parameters:
 *   tunfd   - tun device fd
 *   rpmsgfd - rpmsg socket fd
 *   nlfd    - netlink socket fd
 *   name    - network device name
 *
 * Returned Value:
 *   None
 ****************************************************************************/

static void
rpmsg_tun_netlink_loop(int tunfd, int rpmsgfd, int nlfd, const char *name)
{
  struct rpmsg_tun_buf_s buf[2];
  struct pollfd fds[3];

  memset(buf, 0, sizeof(buf));
  memset(fds, 0, sizeof(fds));
  fds[0].fd = tunfd;
  fds[1].fd = rpmsgfd;
  fds[2].fd = nlfd;
  fds[2].events = POLLIN;

  for (; ; )
    {
      if (buf[0].off)
        {
          fds[0].events = 0;
          fds[1].events = POLLIN | POLLOUT;
        }
      else
        {
          fds[0].events = POLLIN;
          fds[1].events = POLLIN;
        }

      if (poll(fds, 3, -1) < 0)
        {
          break;
        }

      if ((fds[0].revents | fds[1].revents | fds[2].revents) &
          (POLLIN | POLLOUT))
        {
          if ((fds[0].revents & POLLIN) || (fds[1].revents & POLLOUT))
            {
              if (rpmsg_tun_to_socket(tunfd, rpmsgfd, &buf[0]) < 0)
                {
                  break;
                }
            }

          if (fds[1].revents & POLLIN)
            {
              if (rpmsg_tun_from_socket(tunfd, rpmsgfd, &buf[1]) < 0)
                {
                  break;
                }
            }

          if (fds[2].revents & POLLIN)
            {
              if (rpmsg_tun_process_netlink(nlfd, name) != 1)
                {
                  break;
                }
            }
        }
      else if ((fds[0].revents | fds[1].revents |
                fds[2].revents) & (POLLHUP | POLLERR))
        {
          break;
        }
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: main
 ****************************************************************************/

int main(int argc, char *argv[])
{
  int nlfd = -1;
  int rpmsgfd;
  int tunfd;
  int ret;

  if (argc != 4 && argc != 5)
    {
      fprintf(stderr, "Usage: %s <tunname> <ip> <mask> <ifname> or\n"
                      "Usage: %s <tunname> <ip> <mask>\n", argv[0], argv[0]);
      return -EINVAL;
    }

  tunfd = rpmsg_tun_setup(argv[1], argv[2], argv[3]);
  if (tunfd < 0)
    {
      return tunfd;
    }

  if (argc == 5)
    {
      nlfd = rpmsg_tun_connect_netlink();
      if (nlfd < 0)
        {
          close(tunfd);
          return nlfd;
        }
    }

  for (; ; )
    {
      if (nlfd >= 0)
        {
          ret = rpmsg_tun_wait_running(nlfd, argv[4]);
          if (ret < 0)
            {
              goto out;
            }
        }

      rpmsgfd = rpmsg_tun_accept(argv[1]);
      if (rpmsgfd < 0)
        {
          continue;
        }

      ret = rpmsg_tun_set_running(tunfd, true);
      if (ret < 0)
        {
          break;
        }

      if (nlfd >= 0)
        {
          rpmsg_tun_netlink_loop(tunfd, rpmsgfd, nlfd, argv[4]);
        }
      else
        {
          rpmsg_tun_loop(tunfd, rpmsgfd);
        }

      ret = rpmsg_tun_set_running(tunfd, false);
      if (ret < 0)
        {
          break;
        }

      close(rpmsgfd);
    }

  close(rpmsgfd);

out:
  if (nlfd >= 0)
    {
      close(nlfd);
    }

  close(tunfd);
  return ret;
}
