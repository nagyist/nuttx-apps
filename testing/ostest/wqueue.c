/****************************************************************************
 * apps/testing/ostest/wqueue.c
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

#include <assert.h>
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#include <nuttx/wqueue.h>

#ifdef CONFIG_SCHED_WORKQUEUE

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define wqtest_assert(f, msg) __ASSERT__(f, __FILE__, __LINE__, msg)

#define SLEEP_TIME   (100 * 1000)
#define TEST_COUNT   (100)
#define VERIFY_COUNT (100)

#ifdef CONFIG_SCHED_LPWORK
#  define TEST_QUEUE           LPWORK
#  define TEST_QUEUE_PRIORITY  CONFIG_SCHED_LPWORKPRIORITY
#else
#  define TEST_QUEUE           HPWORK
#  define TEST_QUEUE_PRIORITY  CONFIG_SCHED_HPWORKPRIORITY
#endif

#define WORK_NOTIFIER_TEST_TIMEOUT_US 100000
#define FD1                           10
#define FD2                           11

/****************************************************************************
 * Private Types
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

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void empty_worker(FAR void *arg)
{
}

static void sleep_worker(FAR void *arg)
{
  FAR sem_t *sem = arg;
  usleep(SLEEP_TIME);
  sem_post(sem);
}

static void count_worker(FAR void *arg)
{
  FAR sem_t *sem = arg;
  sem_post(sem);
}

static FAR void *tester(FAR void *arg)
{
  FAR void **val = arg;
  struct work_s work;
  int i;

  memset(&work, 0, sizeof(work));
  for (i = 0; i < TEST_COUNT; i++)
    {
      if (val[1] != NULL)
        {
          work_queue_wq(val[1], &work, empty_worker, NULL, 0);
          work_cancel_wq(val[1], &work);
        }
      else
        {
          work_queue((int)(uintptr_t)val[0], &work, empty_worker, NULL, 0);
          work_cancel((int)(uintptr_t)val[0], &work);
        }

      usleep((int)(uintptr_t)val[2]);
    }

  usleep(SLEEP_TIME); /* Wait for workers to run. */
  return NULL;
}

static FAR void *verifier(FAR void *arg)
{
  FAR void **val = arg;
  sem_t sem;
  sem_t call_sem;
  int call_count;
  int i;
  struct work_s work[VERIFY_COUNT + 1];

  sem_init(&sem, 0, 0);
  sem_init(&call_sem, 0, 0);
  memset(&work, 0, sizeof(work));

  /* Queue sleep worker. */

  if (val[1] != NULL)
    {
      work_queue_wq(val[1], &work[0], sleep_worker, &sem, 0);
    }
  else
    {
      work_queue((int)(uintptr_t)val[0], &work[0], sleep_worker, &sem, 0);
    }

  /* Queue count workers when qid is busy. */

  for (i = 1; i <= VERIFY_COUNT; i++)
    {
      if (val[1] != NULL)
        {
          work_queue_wq(val[1], &work[i], count_worker, &call_sem, 0);
        }
      else
        {
          work_queue((int)(uintptr_t)val[0], &work[i],
                     count_worker, &call_sem, 0);
        }
    }

  /* Wait for sleep worker to run. */

  sem_wait(&sem);

  /* Wait for count workers to run. */

  do
    {
      usleep(SLEEP_TIME);
      sem_getvalue(&call_sem, &call_count);
    }
  while (call_count != VERIFY_COUNT);

  sem_getvalue(&call_sem, &call_count);
  printf("wqueue_test: call = %d, expect = %d\n", call_count, VERIFY_COUNT);

  for (i = 0; i < VERIFY_COUNT; i++)
    {
      ASSERT(work[i].worker == NULL);
    }

  ASSERT(call_count == VERIFY_COUNT);
  return NULL;
}

static void run_once(int qid, FAR void *wq, int interval,
                     int priority_test, int priority_verify)
{
  pthread_t thread;
  pthread_attr_t attr;
  struct sched_param sparam;
  int status;
  FAR void *val[3];

  status = pthread_attr_init(&attr);
  if (status != 0)
    {
      printf("wqueue_test: pthread_attr_init failed, status=%d\n", status);
    }

  memset(&sparam, 0, sizeof(sparam));

  /* Tester: try race conditions. */

  sparam.sched_priority = priority_test;
  status = pthread_attr_setschedparam(&attr, &sparam);
  if (status != 0)
    {
      printf("wqueue_test: pthread_attr_setschedparam failed for tester, "
             "status=%d\n", status);
    }

  val[0] = (FAR void *)(uintptr_t)qid;
  val[1] = wq;
  val[2] = (FAR void *)(uintptr_t)interval;
  status = pthread_create(&thread, &attr, tester, val);
  if (status != 0)
    {
      printf("wqueue_test: pthread_create failed for tester, "
             "status=%d\n", status);
    }

  status = pthread_join(thread, NULL);
  if (status != 0)
    {
      printf("wqueue_test: pthread_join failed for tester, "
             "status=%d\n", status);
    }

  /* Verifier: make sure queue is still working properly. */

  sparam.sched_priority = priority_verify;
  status = pthread_attr_setschedparam(&attr, &sparam);
  if (status != 0)
    {
      printf("wqueue_test: pthread_attr_setschedparam failed for verifier, "
             "status=%d\n", status);
    }

  status = pthread_attr_setstacksize(&attr,
        VERIFY_COUNT * sizeof(struct work_s) + CONFIG_PTHREAD_STACK_DEFAULT);
  if (status != 0)
    {
      printf("wqueue_test: pthread_attr_setstacksize failed for verifier, "
             "status=%d\n", status);
    }

  val[0] = (FAR void *)(uintptr_t)qid;
  val[1] = wq;
  status = pthread_create(&thread, &attr, verifier, val);
  if (status != 0)
    {
      printf("wqueue_test: pthread_create failed for verifier, "
             "status=%d\n", status);
    }

  status = pthread_join(thread, NULL);
  if (status != 0)
    {
      printf("wqueue_test: pthread_join failed for verifier, "
             "status=%d\n", status);
    }
}

void wqueue_priority_test(int qid, FAR void *wq, int prio)
{
  int interval;
  int priority_test;
  int priority_verify;

  for (interval = 0; interval <= 1; interval++)
    {
      for (priority_test  = prio - 1;
           priority_test <= prio + 1;
           priority_test++)
        {
          for (priority_verify  = prio - 1;
               priority_verify <= prio + 1;
               priority_verify++)
            {
              run_once(qid, wq, interval, priority_test, priority_verify);
            }
        }
    }
}

#ifdef CONFIG_WQUEUE_NOTIFIER
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
  test_work->completed               = true;
  printf("notifier callback2 triggered.\n");
}

static int create_and_add_notifier(FAR void *test_work,
                                   int evtype,
                                   int fd,
                                   worker_t worker)
{
  struct work_notifier_s info;
  memset(&info, 0, sizeof(struct work_notifier_s));
  info.evtype    = evtype;
  info.qid       = HPWORK;
  info.qualifier = (void *)(intptr_t)fd;
  info.arg       = test_work;
  info.worker    = worker;

  int ret = work_notifier_setup(&info);
  wqtest_assert(ret != -ENOMEM, "wqueue_test: work_notifier_setup() failed");

  return ret;
}

static void test_work_notifier_signal(void)
{
  struct test_state_s test_state;
  unsigned int elapsed  = 0;
  unsigned int elapsed2 = 0;
  int notifier_key1;
  int notifier_key2;

  memset(&test_state.test_work1, 0, sizeof(test_state.test_work1));
  memset(&test_state.test_work2, 0, sizeof(test_state.test_work2));

  notifier_key1 = create_and_add_notifier(
      &test_state.test_work1, WORK_NET_DOWN, FD1, test_notifier1_callback);
  wqtest_assert(notifier_key1 != -ENOMEM,
                "wqueue_test: work_notifier_setup() failed");

  notifier_key2 = create_and_add_notifier(
      &test_state.test_work2, WORK_NET_DOWN, FD2, test_notifier2_callback);
  wqtest_assert(notifier_key2 != -ENOMEM,
                "wqueue_test: work_notifier_setup() failed");

  printf("start sending notifier signal 1.\n");
  work_notifier_signal(WORK_NET_DOWN, (void *)(intptr_t)FD1);

  while (!test_state.test_work1.completed
         && elapsed < WORK_NOTIFIER_TEST_TIMEOUT_US)
    {
      usleep(1000);
      elapsed += 1000;
    }

  wqtest_assert(test_state.test_work1.completed, NULL);

  if (test_state.test_work1.completed)
    {
      printf("notifier 1 processed success.\n");
    }
  else
    {
      printf("notifier 1 processing failed.\n");
    }

  while (!test_state.test_work2.completed
         && elapsed2 < WORK_NOTIFIER_TEST_TIMEOUT_US)
    {
      usleep(1000);
      elapsed2 += 1000;
    }

  wqtest_assert(test_state.test_work2.completed, NULL);

  if (test_state.test_work2.completed)
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

static void test_work_notifier_teardown(void)
{
  struct test_work2_s test_work;
  int notifier_key;
  int fd                 = 1;
  unsigned int elapsed   = 0;
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

  while (!test_work.completed && elapsed < WORK_NOTIFIER_TEST_TIMEOUT_US)
    {
      usleep(1000);
      elapsed += 1000;
    }

  printf("test completed,work status:%s\n",
         test_work.completed ? "completed" : "teardown");
}
#endif /* CONFIG_WQUEUE_NOTIFIER */

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void wqueue_test(void)
{
  FAR void *wq;
  int i;

#ifdef CONFIG_SCHED_HPWORK
  printf("wqueue_test: HPWORK\n");
  wqueue_priority_test(HPWORK, NULL, CONFIG_SCHED_HPWORKPRIORITY);
  printf("wqueue_test: HPWORK done\n");
#endif

#ifdef CONFIG_SCHED_LPWORK
  printf("wqueue_test: LPWORK\n");
  wqueue_priority_test(LPWORK, NULL, CONFIG_SCHED_LPWORKPRIORITY);
  printf("wqueue_test: HPWORK done\n");
#endif

  for (i = 1; i < 3; i++)
    {
      printf("wqueue_test: test %d\n", i);
      wq = work_queue_create("test", 100, NULL, 2048, i);
      DEBUGASSERT(wq != NULL);
      wqueue_priority_test(0, wq, 100);
      work_queue_free(wq);
      printf("wqueue_test: test %d done\n", i);
    }

#ifdef CONFIG_WQUEUE_NOTIFIER
  printf("wqueue_test: work notifier test start...\n");
  test_work_notifier_signal();
  test_work_notifier_teardown();
  printf("wqueue_test: work notifier test done.\n");
#endif

  return;
}

#endif /* CONFIG_SCHED_WORKQUEUE */
