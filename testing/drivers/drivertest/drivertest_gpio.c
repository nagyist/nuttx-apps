/****************************************************************************
 * apps/testing/drivers/drivertest/drivertest_gpio.c
 *
 * SPDX-License-Identifier: Apache-2.0
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

#include <stdbool.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <nuttx/ioexpander/gpio.h>

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

/****************************************************************************
 * Private Types
 ****************************************************************************/

#define GPIOTEST_MAXVALUE 5

struct pre_build_s
{
  FAR char *gpio_a;
  FAR char *gpio_b;
  FAR char *gpio_c;
  FAR char *gpio_d;
  bool loop;
};

/****************************************************************************
 * Private Function
 ****************************************************************************/

/****************************************************************************
 * Name: show_usage
 ****************************************************************************/

static void show_usage(FAR const char *progname, int exitcode)
{
  printf("Usage: %s -a <gpio_a>[dev/gpio0] -b"
         " <gpio_b>[dev/gpio1] -l [loop test]\n", progname);
  printf("Where:\n");
  printf("  -a <gpio_a> gpio_a location [default location: dev/gpio0].\n");
  printf("  -b <gpio_b> gpio_b location [default location: dev/gpio1].\n");
  printf("  -l <loop test> [default: Test the input and output of GPIO ]\n");
  exit(exitcode);
}

/****************************************************************************
 * Name: parse_commandline
 ****************************************************************************/

static void parse_commandline(int argc, FAR char **argv,
                              FAR struct pre_build_s *pre_build)
{
  int option;

  while ((option = getopt(argc, argv, "a:b:c:d:l")) != ERROR)
    {
      switch (option)
        {
          case 'a':
            pre_build->gpio_a = optarg;
            break;
          case 'b':
            pre_build->gpio_b = optarg;
            break;
          case 'c':
            pre_build->gpio_c = optarg;
            break;
          case 'd':
            pre_build->gpio_d = optarg;
            break;
          case 'l':
            pre_build->loop = true;
            break;
          case '?':
            printf("Unknown option: %c\n", optopt);
            show_usage(argv[0], EXIT_FAILURE);
            break;
        }
    }
}

/****************************************************************************
 * Name: gpiotest_randbin
 ****************************************************************************/

static inline bool gpiotest_randbin(void)
{
  int value = rand() % 18;
  if (value > 9)
    {
      return true;
    }
  else
    {
      return false;
    }

  return true;
}

/****************************************************************************
 * Name: setup
 ****************************************************************************/

static int setup(FAR void **state)
{
  FAR struct pre_build_s *pre_build;
  time_t t;
  pre_build = (FAR struct pre_build_s *)*state;

  /* Seed the random number generated */

  srand((unsigned)time(&t));
  *state = pre_build;
  return 0;
}

/****************************************************************************
 * Name: drivertest_gpio_one
 ****************************************************************************/

static void drivertest_gpio_one(FAR void **state)
{
  FAR struct pre_build_s *pre_build;
  bool outvalue;
  bool invalue;
  int fd[2];
  int ret;
  int i;
  int j;

  pre_build = *state;
  fd[0] = open(pre_build->gpio_a, O_RDWR);
  assert_false(fd[0] < 0);

  fd[1] = open(pre_build->gpio_b, O_RDWR);
  assert_false(fd[1] < 0);

  /* Test Single GPIO I/O functionality */

  for (i = 0; i < 2; i++)
    {
      ret = ioctl(fd[i], GPIOC_SETPINTYPE, GPIO_INPUT_PIN | GPIO_OUTPUT_PIN);
      assert_false(ret < 0);
      for (j = 0; j < GPIOTEST_MAXVALUE; j++)
        {
          outvalue = gpiotest_randbin();
          ret = ioctl(fd[i], GPIOC_WRITE, outvalue);
          assert_false(ret < 0);

          ret = ioctl(fd[i], GPIOC_READ, &invalue);
          assert_false(ret < 0);

          printf("[input and output test]  outvalue is %d, invalue is %d\n",
                  outvalue, invalue);
          assert_int_equal(invalue, outvalue);
        }

        close(fd[i]);
    }
}

/****************************************************************************
 * Name: drivertest_gpio_loop
 ****************************************************************************/

static void drivertest_gpio_loop(FAR void **state)
{
  FAR struct pre_build_s *pre_build;
  int fd_a;
  int fd_b;
  bool outvalue;
  bool invalue;
  int ret;
  int i;

  pre_build = *state;

  fd_a = open(pre_build->gpio_a, O_RDWR);
  assert_false(fd_a < 0);
  fd_b = open(pre_build->gpio_b, O_RDWR);
  assert_false(fd_b < 0);

  /* Test the input function
   * Method: Output from gpio_a to gpio_b and
   * compare the read values.
   */

  /* gpio_a to gpio_b */

  ret = ioctl(fd_a, GPIOC_SETPINTYPE, (unsigned long) GPIO_OUTPUT_PIN);
  assert_false(ret < 0);
  ret = ioctl(fd_b, GPIOC_SETPINTYPE, (unsigned long) GPIO_INPUT_PIN);
  assert_false(ret < 0);

  for (i = 0; i < GPIOTEST_MAXVALUE; i++)
    {
      /* Set GPIO level */

      outvalue = gpiotest_randbin();
      assert_false(ret < 0);
      ret = ioctl(fd_a, GPIOC_WRITE, (unsigned long) outvalue);
      assert_false(ret < 0);

      /* Read Pin Test */

      ret = ioctl(fd_b, GPIOC_READ, (unsigned long)((uintptr_t)&invalue));
      assert_false(ret < 0);

      printf("[Loop test]  outvalue is %d, invalue is %d\n",
            outvalue, invalue);
      assert_int_equal(invalue, outvalue);
    }

  ret = ioctl(fd_b, GPIOC_SETPINTYPE, (unsigned long) GPIO_OUTPUT_PIN);
  assert_false(ret < 0);
  ret = ioctl(fd_a, GPIOC_SETPINTYPE, (unsigned long) GPIO_INPUT_PIN);
  assert_false(ret < 0);

  /* gpio_b to gpio_a */

  for (i = 0; i < GPIOTEST_MAXVALUE; i++)
    {
      /* Set GPIO level */

      outvalue = gpiotest_randbin();
      assert_false(ret < 0);
      ret = ioctl(fd_b, GPIOC_WRITE, (unsigned long) outvalue);
      assert_false(ret < 0);

      /* Read Pin Test */

      ret = ioctl(fd_a, GPIOC_READ, (unsigned long)((uintptr_t)&invalue));
      assert_false(ret < 0);

      printf("[Loop test]  outvalue is %d, invalue is %d\n",
             outvalue, invalue);
      assert_int_equal(invalue, outvalue);
    }

  close(fd_a);
  close(fd_b);
}

/****************************************************************************
 * Name: teardown
 ****************************************************************************/

static int teardown(FAR void **state)
{
  return 0;
}

/****************************************************************************
 * Name: drivertest_rw_gpio
 ****************************************************************************/

static void drivertest_rw_gpio(FAR void **state)
{
  FAR struct pre_build_s *pre_build;
  char outvalue;
  char invalue;
  int fd;
  int ret;
  int j;

  pre_build = *state;

  fd = open(pre_build->gpio_b, O_RDWR);
  assert_false(fd < 0);

  /* Test Single GPIO I/O functionality */

  ret = ioctl(fd, GPIOC_SETPINTYPE, GPIO_OUTPUT_PIN);
  assert_false(ret < 0);
  for (j = 0; j < GPIOTEST_MAXVALUE; j++)
    {
      outvalue = gpiotest_randbin() + '0';
      ret = write(fd, &outvalue, sizeof(bool));
      assert_false(ret < 0);
      ret = lseek(fd, 0, SEEK_SET);
      assert_false(ret < 0);
      ret = read(fd, &invalue, sizeof(bool));
      assert_false(ret < 0);
      printf("[input and output test]  outvalue is %d, invalue is %d\n",
              outvalue, invalue);
      assert_int_equal(invalue, outvalue);
    }
  close(fd);
}

/****************************************************************************
 * Name: drivertest_int_edge
 ****************************************************************************/

static void drivertest_int_edge(FAR void **state)
{
  FAR struct pre_build_s *pre_build;
  enum gpio_pintype_e pintype;
  struct pollfd fds;
  int fd_c;
  int ret;

  pre_build = *state;
  fd_c = open(pre_build->gpio_c, O_RDWR);
  assert_false(fd_c < 0);

  fds.events = POLLIN;
  fds.fd = fd_c;
  ret = ioctl(fd_c, GPIOC_SETPINTYPE, GPIO_INTERRUPT_FALLING_PIN);
  assert_false(ret < 0);
  ret = ioctl(fd_c, GPIOC_PINTYPE, &pintype);
  assert_false(ret < 0);
  assert_int_equal(pintype, GPIO_INTERRUPT_FALLING_PIN);

  ret = ioctl(fd_c, GPIOC_REGISTER, NULL);
  assert_false(ret < 0);
  ret = poll(&fds, 1, -1);
  assert_false(ret < 0);
  ret = ioctl(fd_c, GPIOC_UNREGISTER, NULL);
  assert_false(ret < 0);

  close(fd_c);
}

/****************************************************************************
 * Name: drivertest_int_edge
 ****************************************************************************/

static void drivertest_int_level(FAR void **state)
{
  FAR struct pre_build_s *pre_build;
  struct pollfd fds;
  int fd_d;
  int ret;

  pre_build = *state;
  fd_d = open(pre_build->gpio_d, O_RDWR);
  assert_false(fd_d < 0);

  fds.events = POLLIN;
  fds.fd = fd_d;
  ret = ioctl(fd_d, GPIOC_REGISTER, NULL);
  assert_false(ret < 0);
  ret = poll(&fds, 1, -1);
  assert_false(ret < 0);
  ret = ioctl(fd_d, GPIOC_UNREGISTER, NULL);
  assert_false(ret < 0);

  close(fd_d);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: drivertest_gpio_main
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  void (*drivertest_gpio)(FAR void **state);
  FAR struct pre_build_s pre_build =
    {
      .gpio_a = "dev/gpio0",
      .gpio_b = "dev/gpio1",
      .gpio_c = "dev/gpio2",
      .gpio_d = "dev/gpio3",
      .loop   = false
    };

  parse_commandline(argc, argv, &pre_build);

  if (pre_build.loop)
    {
      drivertest_gpio = drivertest_gpio_loop;
    }
  else
    {
      drivertest_gpio = drivertest_gpio_one;
    }

  const struct CMUnitTest tests[] =
    {
      cmocka_unit_test_prestate_setup_teardown(drivertest_gpio, setup,
                                               teardown, &pre_build),
      cmocka_unit_test_prestate_setup_teardown(drivertest_rw_gpio, setup,
                                               teardown, &pre_build),
      cmocka_unit_test_prestate_setup_teardown(drivertest_int_edge, setup,
                                               teardown, &pre_build),
      cmocka_unit_test_prestate_setup_teardown(drivertest_int_level, setup,
                                               teardown, &pre_build),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
