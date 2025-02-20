/****************************************************************************
 * apps/netutils/rpmsg_tun/rpmsg_tun_util.c
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

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netpacket/rpmsg.h>
#ifdef __NuttX__
#  include <netpacket/netlink.h>
#  include <nuttx/net/tun.h>
#else
#  include <linux/if_tun.h>
#  include <linux/rtnetlink.h>
#endif
#include <poll.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "rpmsg_tun_util.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: rpmsg_tun_set_addr
 *
 * Description:
 *   Set ip/mask address to tun device
 *
 * Parameters:
 *   name - tun device name
 *   cmd  - ioctl cmd
 *   addr - ip/mask address
 *
 * Returned Value:
 *   0 on success, -errno on failure.
 ****************************************************************************/

static int rpmsg_tun_set_addr(const char *name, int cmd, const char *addr)
{
  struct sockaddr_in *sin;
  struct ifreq ifr;
  int fd;

  fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
  if (fd < 0)
    {
      return -errno;
    }

  memset(&ifr, 0, sizeof(ifr));
  strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name) - 1);

  sin = (struct sockaddr_in *)&ifr.ifr_addr;
  sin->sin_family = AF_INET;
  inet_aton(addr, &sin->sin_addr);

  if (ioctl(fd, cmd, &ifr) < 0)
    {
      close(fd);
      return -errno;
    }

  close(fd);
  return 0;
}

/****************************************************************************
 * Name: rpmsg_tun_send_all
 *
 * Description:
 *   Send all data buffer to file handle
 *
 * Parameters:
 *   fd  - file handle
 *   buf - data buffer
 *   len - data length
 *
 * Returned Value:
 *   int - 0 on success, -errno on failure.
 ****************************************************************************/

static int rpmsg_tun_send_all(int fd, const void *buf_, size_t len)
{
  const char *buf = buf_;

  while (len > 0)
    {
      ssize_t ret = send(fd, buf, len, 0);
      if (ret < 0)
        {
          return -errno;
        }

      len -= ret;
      buf += ret;
    }

  return 0;
}

/****************************************************************************
 * Name: rpmsg_tun_recv_all
 *
 * Description:
 *   Receive all data buffer from file handle
 *
 * Parameters:
 *   fd  - file handle
 *   buf - data buffer
 *   len - data length
 *
 * Returned Value:
 *   int - 0 on success, -errno on failure.
 ****************************************************************************/

static int rpmsg_tun_recv_all(int fd, void *buf_, size_t len)
{
  char *buf = buf_;

  while (len > 0)
    {
      ssize_t ret = recv(fd, buf, len, 0);
      if (ret <= 0)
        {
          return ret ? -errno : -EPIPE;
        }

      len -= ret;
      buf += ret;
    }

  return 0;
}

/****************************************************************************
 * Name: rpmsg_tun_is_running
 *
 * Description:
 *   Check network device running status
 *
 * Parameters:
 *   name - network device name
 *
 * Returned Value:
 *   1 is running and get ip address, 0 is down or not set ip addr,
 *   -errno on failure.
 ****************************************************************************/

static int rpmsg_tun_is_running(const char *name)
{
  struct ifreq ifr;
  int fd;

  fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
  if (fd < 0)
    {
      return -errno;
    }

  memset(&ifr, 0, sizeof(ifr));
  strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name) - 1);

  if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0)
    {
      close(fd);
      return -errno;
    }

  if ((ifr.ifr_flags & IFF_RUNNING) != 0)
    {
      if (ioctl(fd, SIOCGIFADDR, &ifr) < 0)
        {
          close(fd);
          return -errno;
        }

      struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_addr;
      if (addr->sin_addr.s_addr != INADDR_ANY)
        {
          close(fd);
          return 1;
        }
    }

  close(fd);
  return 0;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: rpmsg_tun_setup
 *
 * Description:
 *   Setup tun device
 *
 * Parameters:
 *   name - tun device name
 *   ip   - tun device ip
 *   mask - tun device mask
 *
 * Returned Value:
 *   int - tun fd on success, -errno on failure.
 ****************************************************************************/

int rpmsg_tun_setup(const char *name, const char *ip, const char *mask)
{
  struct ifreq ifr;
  int fd;

  /* Open the tun device */

  fd = open("/dev/net/tun", O_RDWR | O_CLOEXEC);
  if (fd < 0)
    {
      return -errno;
    }

  /* Configure the tun device */

  memset(&ifr, 0, sizeof(ifr));
  ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
  strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name) - 1);

  if (ioctl(fd, TUNSETIFF, &ifr) < 0)
    {
      goto out;
    }

  if (rpmsg_tun_set_addr(name, SIOCSIFADDR, ip) < 0)
    {
      goto out;
    }

  if (rpmsg_tun_set_addr(name, SIOCSIFNETMASK, mask) < 0)
    {
      goto out;
    }

  return fd;

out:
  close(fd);
  return -errno;
}

/****************************************************************************
 * Name: rpmsg_tun_set_running
 *
 * Description:
 *   Set tun device running status
 *
 * Parameters:
 *   fd      - tun device fd
 *   running - true is running and false is down
 *
 * Returned Value:
 *   0 on success, -errno on failure.
 ****************************************************************************/

int rpmsg_tun_set_running(int fd, bool running)
{
  struct ifreq ifr;
  int carrier = running;
  int sockfd;

  memset(&ifr, 0, sizeof(ifr));

  /* Get tun device name */

  if (ioctl(fd, TUNGETIFF, &ifr) < 0)
    {
      return -errno;
    }

  sockfd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
  if (sockfd < 0)
    {
      return -errno;
    }

  if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0)
    {
      goto out;
    }

  if (running)
    {
      ifr.ifr_flags |= IFF_UP;

      if (ioctl(sockfd, SIOCSIFFLAGS, &ifr) < 0)
        {
          goto out;
        }

      if (ioctl(fd, TUNSETCARRIER, &carrier) < 0)
        {
          goto out;
        }
    }
  else
    {
      ifr.ifr_flags &= ~IFF_UP;

      if (ioctl(fd, TUNSETCARRIER, &carrier) < 0)
        {
          goto out;
        }

      if (ioctl(sockfd, SIOCSIFFLAGS, &ifr) < 0)
        {
          goto out;
        }
    }

  close(sockfd);
  return 0;

out:
  close(sockfd);
  return -errno;
}

/****************************************************************************
 * Name: rpmsg_tun_connect
 *
 * Description:
 *   Create rpmsg socket and connect to server
 *
 * Parameters:
 *   cpu  - remote cpu name
 *   name - tun device name
 *
 * Returned Value:
 *   int - rpmsg socket fd on success, -errno on failure.
 ****************************************************************************/

int rpmsg_tun_connect(const char *cpu, const char *name)
{
  struct sockaddr_rpmsg addr;
  int fd;

  /* Create rpmsg socket */

  fd = socket(AF_RPMSG, SOCK_STREAM | SOCK_CLOEXEC, 0);
  if (fd < 0)
    {
      return -errno;
    }

  /* Initialize address */

  memset(&addr, 0, sizeof(addr));
  addr.rp_family = AF_RPMSG;
  strncpy(addr.rp_cpu, cpu, sizeof(addr.rp_cpu) - 1);
  strncpy(addr.rp_name, name, sizeof(addr.rp_name) - 1);

  /* Connect to remote server */

  if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
      close(fd);
      return -errno;
    }

  return fd;
}

/****************************************************************************
 * Name: rpmsg_tun_accept
 *
 * Description:
 *   Create rpmsg socket and accept the connection from client
 *
 * Parameters:
 *   name - tun device name
 *
 * Returned Value:
 *   int - rpmsg socket fd on success, -errno on failure.
 ****************************************************************************/

int rpmsg_tun_accept(const char *name)
{
  struct sockaddr_rpmsg addr;
  int ret;
  int fd;

  /* Create rpmsg socket */

  fd = socket(AF_RPMSG, SOCK_STREAM | SOCK_CLOEXEC, 0);
  if (fd < 0)
    {
      return -errno;
    }

  /* Initialize address */

  memset(&addr, 0, sizeof(addr));
  addr.rp_family = AF_RPMSG;
  strncpy(addr.rp_name, name, sizeof(addr.rp_name) - 1);

  /* Bind to local address */

  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
      goto out;
    }

  /* Listen for incoming connections */

  if (listen(fd, 1) < 0)
    {
      goto out;
    }

  /* Accept the connection */

  ret = accept4(fd, NULL, NULL, SOCK_CLOEXEC);
  if (ret < 0)
    {
      goto out;
    }

  close(fd);
  return ret;

out:
  close(fd);
  return -errno;
}

/****************************************************************************
 * Name: rpmsg_tun_to_socket
 *
 * Description:
 *   Read buffer from tun device and send to rpmsg socket
 *
 * Parameters:
 *   tunfd   - tun device fd
 *   rpmsgfd - rpmsg socket fd
 *
 * Returned Value:
 *   int - 0 on success, -errno on failure.
 ****************************************************************************/

int rpmsg_tun_to_socket(int tunfd, int rpmsgfd)
{
  char buf[RPMSG_TUN_BUFFER_SIZE];
  int32_t ret;

  /* Read from tun device */

  ret = read(tunfd, buf + sizeof(ret), sizeof(buf) - sizeof(ret));
  if (ret <= 0)
    {
      return ret < 0 ? -errno : -EPIPE;
    }

  /* Pretend the buffer length */

  memcpy(buf, &ret, sizeof(ret));

  /* Send to the remote side */

  return rpmsg_tun_send_all(rpmsgfd, buf, ret + sizeof(ret));
}

/****************************************************************************
 * Name: rpmsg_tun_from_socket
 *
 * Description:
 *   Read buffer from rpmsg socket and send to tun device
 *
 * Parameters:
 *   tunfd   - tun device fd
 *   rpmsgfd - rpmsg socket fd
 *   buf     - buffer to store the data
 *
 * Returned Value:
 *   int - 0 on success, -errno on failure.
 ****************************************************************************/

int rpmsg_tun_from_socket(int tunfd, int rpmsgfd,
                          struct rpmsg_tun_buf_s *buf)
{
  int ret;

  if (buf->len == 0)
    {
      ret = rpmsg_tun_recv_all(rpmsgfd, &buf->len, sizeof(buf->len));
      if (ret < 0)
        {
          return ret;
        }

      if (buf->len > sizeof(buf->data))
        {
          return -EINVAL;
        }
    }

  ret = recv(rpmsgfd, buf->data + buf->off, buf->len - buf->off, 0);
  if (ret < 0)
    {
      return -errno;
    }

  buf->off += ret;
  if (buf->off >= buf->len)
    {
      ret = write(tunfd, buf->data, buf->len);
      if (ret != buf->len)
        {
          return ret < 0 ? -errno : -EPIPE;
        }

      buf->off = 0;
      buf->len = 0;
    }

  return 0;
}

/****************************************************************************
 * Name: rpmsg_tun_connect_netlink
 *
 * Description:
 *   Create netlink socket and monitor the route mesage
 *
 * Parameters:
 *   None
 *
 * Returned Value:
 *   int - netlink fd on success, -errno on failure.
 ****************************************************************************/

int rpmsg_tun_connect_netlink(void)
{
  struct sockaddr_nl addr;
  int fd;

  /* Create netlink socket */

  fd = socket(AF_NETLINK, SOCK_DGRAM | SOCK_CLOEXEC, NETLINK_ROUTE);
  if (fd < 0)
    {
      return -errno;
    }

  /* Initialize address */

  memset(&addr, 0, sizeof(addr));
  addr.nl_family = AF_NETLINK;
  addr.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV6_IFADDR;

  /* Bind to local address */

  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
      close(fd);
      return -errno;
    }

  return fd;
}

/****************************************************************************
 * Name: rpmsg_tun_process_netlink
 *
 * Description:
 *   Process netlink message
 *
 * Parameters:
 *   fd   - netlink socket fd
 *   name - netdev to monitor
 *
 * Returned Value:
 *   1 is running/unknown or have ip addr, 0 is down and -errno on failure.
 ****************************************************************************/

int rpmsg_tun_process_netlink(int fd, const char *name)
{
  char buf[RPMSG_TUN_BUFFER_SIZE];
  struct nlmsghdr *hdr = (struct nlmsghdr *)buf;
  ssize_t len;

  len = read(fd, buf, sizeof(buf));
  if (len <= 0)
    {
      return len < 0 ? -errno : -EPIPE;
    }

  for (; NLMSG_OK(hdr, len); hdr = NLMSG_NEXT(hdr, len))
    {
      struct ifinfomsg *info;
      struct ifaddrmsg *ifaddr;
      struct rtattr *attr;
      size_t attrlen;
      char devname[IFNAMSIZ];

      if (hdr->nlmsg_type != RTM_NEWLINK &&
          hdr->nlmsg_type != RTM_DELLINK &&
          hdr->nlmsg_type != RTM_SETLINK &&
          hdr->nlmsg_type != RTM_NEWADDR &&
          hdr->nlmsg_type != RTM_DELADDR)
        {
          continue;
        }

      switch (hdr->nlmsg_type)
        {
          case RTM_NEWLINK:
          case RTM_DELLINK:
          case RTM_SETLINK:
            info = (struct ifinfomsg *)NLMSG_DATA(hdr);
            attr = IFLA_RTA(info);
            attrlen = IFLA_PAYLOAD(hdr);

            for (; RTA_OK(attr, attrlen); attr = RTA_NEXT(attr, attrlen))
              {
                if (attr->rta_type != IFLA_IFNAME)
                  {
                    continue;
                  }

                if (strcmp(name, RTA_DATA(attr)))
                  {
                    break;
                  }

                return (info->ifi_flags & IFF_RUNNING) != 0;
              }

            break;

          case RTM_NEWADDR:
          case RTM_DELADDR:
            ifaddr = (struct ifaddrmsg *)NLMSG_DATA(hdr);
            attr = IFA_RTA(ifaddr);
            attrlen = IFA_PAYLOAD(hdr);

            for (; RTA_OK(attr, attrlen); attr = RTA_NEXT(attr, attrlen))
              {
                if (attr->rta_type != IFA_LOCAL)
                  {
                    continue;
                  }

                if (if_indextoname(ifaddr->ifa_index, devname) == NULL)
                  {
                    continue;
                  }

                if (strcmp(name, devname))
                  {
                    break;
                  }

                return hdr->nlmsg_type == RTM_NEWADDR;
              }

            break;

          default:
            break;
        }
    }

  return 1;
}

/****************************************************************************
 * Name: rpmsg_tun_wait_running
 *
 * Description:
 *   Wait network device running
 *
 * Parameters:
 *   fd   - netlink socket fd
 *   name - network device name
 *
 * Returned Value:
 *   1 is running, -errno on failure.
 ****************************************************************************/

int rpmsg_tun_wait_running(int fd, const char *name)
{
  int ret;

  for (; ; )
    {
      ret = rpmsg_tun_is_running(name);
      if (ret == 1)
        {
          break;
        }

      ret = rpmsg_tun_process_netlink(fd, name);
      if (ret < 0)
        {
          break;
        }
    }

  return ret;
}

/****************************************************************************
 * Name: rpmsg_tun_loop
 *
 * Description:
 *   Poll tunfd and rpmsgfd when fds return POLLIN
 *   then read buffer from tunfd and send to rpmsgfd
 *   or read buffer from rpmsgfd and send to tunfd
 *
 * Parameters:
 *   tunfd   - tun device fd
 *   rpmsgfd - rpmsg socket fd
 *
 * Returned Value:
 *   None
 ****************************************************************************/

void rpmsg_tun_loop(int tunfd, int rpmsgfd)
{
  struct rpmsg_tun_buf_s buf;
  struct pollfd fds[2];

  memset(&buf, 0, sizeof(buf));
  memset(fds, 0, sizeof(fds));
  fds[0].fd = tunfd;
  fds[0].events = POLLIN;
  fds[1].fd = rpmsgfd;
  fds[1].events = POLLIN;

  for (; ; )
    {
      if (poll(fds, 2, -1) < 0)
        {
          break;
        }

      if ((fds[0].revents | fds[1].revents) & POLLIN)
        {
          if (fds[0].revents & POLLIN)
            {
              if (rpmsg_tun_to_socket(tunfd, rpmsgfd) < 0)
                {
                  break;
                }
            }

          if (fds[1].revents & POLLIN)
            {
              if (rpmsg_tun_from_socket(tunfd, rpmsgfd, &buf) < 0)
                {
                  break;
                }
            }
        }
      else if ((fds[0].revents | fds[1].revents) & (POLLHUP | POLLERR))
        {
          break;
        }
    }
}
