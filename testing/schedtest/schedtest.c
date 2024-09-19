/****************************************************************************
 * apps/testing/schedtest/schedtest.c
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
#include <nuttx/wqueue.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define TIMEOUT_US 100000
#define FD1 10
#define FD2 11

/****************************************************************************
 * Private Data
 ****************************************************************************/

struct test_work1_s
{
  bool completed;
};

struct test_work2_s
{
  bool completed;
  bool first_notify;
};

struct test_state_s
{
  struct test_work1_s test_work1;
  struct test_work2_s test_work2;
};

/****************************************************************************
 * Private Functions Prototypes
 ****************************************************************************/

static void test_notifier1_callback(FAR void *arg);
static void test_notifier2_callback(FAR void *arg);
static int create_and_add_notifier(FAR void *test_work, int evtype, int fd,
                                   worker_t worker);
static void test_work_notifier_signal(FAR void **state);
static void test_work_notifier_teardown(FAR void **state);
static void test_notifier_callback2(FAR void *arg);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void test_notifier1_callback(FAR void *arg)
{
  FAR struct test_work1_s *test_work = (FAR struct test_work1_s *)arg;
  printf("notifier_callback: triggered.\n");
  fflush(stdout);
  if (test_work)
    {
      test_work->completed = true;
      printf("test_work1 completed.\n");
      printf("start triggering notifier signal 2.\n");
      work_notifier_signal(WORK_NET_DOWN, (FAR void *)(intptr_t)FD2);
    }
}

static void test_notifier2_callback(FAR void *arg)
{
  FAR struct test_work1_s *test_work = (struct test_work1_s *)arg;
  test_work->completed = true;
  printf("notifier callback2 triggered.\n");
}

static int create_and_add_notifier(FAR void *test_work, int evtype, int fd,
                                   worker_t worker)
{
  struct work_notifier_s info;
  memset(&info, 0, sizeof(struct work_notifier_s));
  info.evtype = evtype;
  info.qid = HPWORK;
  info.qualifier = (void *)(intptr_t)fd;
  info.arg = test_work;
  info.worker = worker;

  int ret = work_notifier_setup(&info);
  assert_int_not_equal(ret, -1);

  return ret;
}

static void test_work_notifier_signal(FAR void **state)
{
  struct test_state_s *test_state = (struct test_state_s *)*state;
  unsigned int elapsed = 0;
  unsigned int elapsed2 = 0;
  int notifier_key1;
  int notifier_key2;

  memset(&test_state->test_work1, 0, sizeof(test_state->test_work1));
  memset(&test_state->test_work2, 0, sizeof(test_state->test_work2));

  notifier_key1 = create_and_add_notifier(&test_state->test_work1,
                                          WORK_NET_DOWN,
                                          FD1, test_notifier1_callback);
  assert_int_not_equal(notifier_key1, -1);

  notifier_key2 = create_and_add_notifier(&test_state->test_work2,
                                          WORK_NET_DOWN,
                                          FD2, test_notifier2_callback);
  assert_int_not_equal(notifier_key2, -1);

  printf("start sending notifier signal 1.\n");
  work_notifier_signal(WORK_NET_DOWN, (void *)(intptr_t)FD1);

  while (!test_state->test_work1.completed && elapsed < TIMEOUT_US)
    {
      usleep(1000);
      elapsed += 1000;
    }

  assert_true(test_state->test_work1.completed);

  if (test_state->test_work1.completed)
    {
      printf("notifier 1 processed success.\n");
    }
  else
    {
      printf("notifier 1 processing failed.\n");
    }

  while (!test_state->test_work2.completed && elapsed2 < TIMEOUT_US)
    {
      usleep(1000);
      elapsed2 += 1000;
    }

  assert_true(test_state->test_work2.completed);

  if (test_state->test_work2.completed)
    {
      printf("notifier 2 processed success.\n");
    }
  else
    {
      printf("notifier 2 processing failed.\n");
    }

  work_notifier_teardown(notifier_key1);
  work_notifier_teardown(notifier_key2);
}

static void test_notifier_callback2(FAR void *arg)
{
  FAR struct test_work2_s *test_work = (struct test_work2_s *)arg;
  if (test_work->first_notify)
    {
      printf("notifier callback2 triggered:first notify.\n");
      test_work->first_notify = false;
    }
  else
    {
      printf("notifier callback2 triggered:second notify.\n");
      test_work->completed = true;
    }
}

static void test_work_notifier_teardown(FAR void **state)
{
  struct test_work2_s test_work;
  int notifier_key;
  int fd = 1;
  unsigned int elapsed = 0;
  test_work.first_notify = true;

  memset(&test_work, 0, sizeof(test_work));

  notifier_key = create_and_add_notifier(&test_work, WORK_NET_DOWN, fd,
                                         test_notifier_callback2);

  printf("start sending notifier signal.\n");
  work_notifier_signal(WORK_NET_DOWN, (FAR void *)(intptr_t)(fd));

  printf("start sending teardown notifier.\n");
  work_notifier_teardown(notifier_key);
  printf("notifier successfully torn down on first attempt.\n");

  test_work.completed = false;
  printf("sending second notifier signal.\n");
  work_notifier_signal(WORK_NET_DOWN, (FAR void *)(intptr_t)fd);

  while (!test_work.completed && elapsed < TIMEOUT_US)
    {
      usleep(1000);
      elapsed += 1000;
    }

  printf("test completed,work status:%s\n",
          test_work.completed ? "completed" : "teardown");
  assert_false(test_work.completed);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  cmocka_set_message_output(CM_OUTPUT_STDOUT);

  struct test_state_s test_state;
  memset(&test_state, 0, sizeof(test_state));
  const struct CMUnitTest tests[] = {
    cmocka_unit_test_prestate(test_work_notifier_signal, &test_state),
    cmocka_unit_test_prestate(test_work_notifier_teardown, &test_state)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
