/****************************************************************************
 * apps/testing/drivertest/drivertest_regulator.c
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

#include <nuttx/nuttx.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <cmocka.h>
#include <nuttx/power/consumer.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define REGULATOR_ID        "fake_regulator"
#define REGULATOR_SUPPLY_ID "fake_regulator_supply"

/****************************************************************************
 * Private Functions Prototypes
 ****************************************************************************/

int test_regulator_enable(FAR struct regulator_dev_s *rdev);
int test_regulator_is_enabled(FAR struct regulator_dev_s *rdev);
int test_regulator_disable(FAR struct regulator_dev_s *rdev);

struct test_regulator_s
{
  FAR struct regulator_dev_s *rdev;
  int state;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct regulator_ops_s g_fake_regulator_ops =
{
  NULL,                         /* list_voltage */
  NULL,                         /* set_voltage */
  NULL,                         /* set_voltage_sel */
  NULL,                         /* get_voltage */
  NULL,                         /* get_voltage_sel */
  test_regulator_enable,        /* enable */
  test_regulator_is_enabled,    /* is_enabled */
  test_regulator_disable        /* disable */
};

static struct regulator_desc_s g_fake_regulator_desc =
{
  .name = REGULATOR_ID,
  .boot_on = 0,
  .always_on = 1,
};

static struct test_regulator_s g_fake_regulator =
{
  .rdev = NULL,
  .state = 0,
};

static struct regulator_desc_s g_fake_regulator_supply_desc =
{
  .name = REGULATOR_SUPPLY_ID,
  .boot_on = 0,
  .always_on = 0,
};

static struct test_regulator_s g_fake_regulator_supply =
{
  .rdev = NULL,
  .state = 0,
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

int test_regulator_enable(FAR struct regulator_dev_s *rdev)
{
  FAR struct test_regulator_s *tr = rdev->priv;
  tr->state = 1;
  return OK;
}

int test_regulator_is_enabled(FAR struct regulator_dev_s *rdev)
{
  FAR struct test_regulator_s *tr = rdev->priv;
  return tr->state;
}

int test_regulator_disable(FAR struct regulator_dev_s *rdev)
{
  FAR struct test_regulator_s *tr = rdev->priv;
  tr->state = 0;
  return OK;
}

static void test_regulator_always_on(FAR void **state)
{
  FAR struct regulator_s *test = NULL;
  int ret = 0;
  int cnt = 10;

  g_fake_regulator_desc.boot_on = 0;
  g_fake_regulator_desc.always_on = 1;
  g_fake_regulator_desc.supply_name = NULL;
  g_fake_regulator.rdev = regulator_register(&g_fake_regulator_desc,
                                             &g_fake_regulator_ops,
                                             &g_fake_regulator);
  assert_false(NULL == g_fake_regulator.rdev);
  test = regulator_get(REGULATOR_ID);
  assert_false(NULL == test);

  assert_int_equal(g_fake_regulator.state, 1);
  while (cnt--)
    {
      ret = regulator_enable(test);
      assert_false(ret < 0);
      assert_int_equal(g_fake_regulator.state, 1);
      ret = regulator_enable(test);
      assert_false(ret < 0);
      assert_int_equal(g_fake_regulator.state, 1);
      ret = regulator_disable(test);
      assert_false(ret < 0);
      assert_int_equal(g_fake_regulator.state, 1);
      ret = regulator_disable(test);
      assert_false(ret < 0);
      assert_int_equal(g_fake_regulator.state, 1);
      ret = regulator_disable(test);
      assert_int_equal(ret, -EIO);
    }

  regulator_put(test);
  regulator_unregister(g_fake_regulator.rdev);
  return;
}

static void test_regulator_supply_1(FAR void **state)
{
  FAR struct regulator_s *test = NULL;
  int cnt = 10;
  int ret = 0;

  g_fake_regulator_desc.supply_name = REGULATOR_SUPPLY_ID;
  g_fake_regulator_desc.boot_on = 0;
  g_fake_regulator_desc.always_on = 1;
  g_fake_regulator.rdev = regulator_register(&g_fake_regulator_desc,
                                             &g_fake_regulator_ops,
                                             &g_fake_regulator);
  assert_true(NULL == g_fake_regulator.rdev);
  g_fake_regulator_supply.rdev = regulator_register(
                                             &g_fake_regulator_supply_desc,
                                             &g_fake_regulator_ops,
                                             &g_fake_regulator_supply);
  assert_false(NULL == g_fake_regulator_supply.rdev);
  assert_int_equal(g_fake_regulator_supply.state, 0);
  g_fake_regulator.rdev = regulator_register(&g_fake_regulator_desc,
                                             &g_fake_regulator_ops,
                                             &g_fake_regulator);
  assert_false(NULL == g_fake_regulator.rdev);
  assert_int_equal(g_fake_regulator_supply.state, 1);
  assert_int_equal(g_fake_regulator.state, 1);
  test = regulator_get(REGULATOR_ID);
  assert_false(NULL == test);
  while (cnt--)
    {
      ret = regulator_enable(test);
      assert_false(ret < 0);
      assert_int_equal(g_fake_regulator.state, 1);
      assert_int_equal(g_fake_regulator_supply.state, 1);
      ret = regulator_enable(test);
      assert_false(ret < 0);
      assert_int_equal(g_fake_regulator.state, 1);
      assert_int_equal(g_fake_regulator_supply.state, 1);
      ret = regulator_disable(test);
      assert_false(ret < 0);
      assert_int_equal(g_fake_regulator.state, 1);
      assert_int_equal(g_fake_regulator_supply.state, 1);
      ret = regulator_disable(test);
      assert_false(ret < 0);
      assert_int_equal(g_fake_regulator.state, 1);
      assert_int_equal(g_fake_regulator_supply.state, 1);
      ret = regulator_disable(test);
      assert_int_equal(ret, -EIO);
    }

  regulator_put(test);
  regulator_unregister(g_fake_regulator.rdev);
  g_fake_regulator.rdev = NULL;
  assert_int_equal(g_fake_regulator_supply.state, 0);
  regulator_unregister(g_fake_regulator_supply.rdev);
  g_fake_regulator_supply.rdev = NULL;
  return;
}

static void test_regulator_supply_2(FAR void **state)
{
  FAR struct regulator_s *test = NULL;
  int cnt = 10;
  int ret = 0;

  g_fake_regulator_desc.supply_name = REGULATOR_SUPPLY_ID;
  g_fake_regulator_desc.boot_on = 1;
  g_fake_regulator_desc.always_on = 1;
  g_fake_regulator.rdev = regulator_register(&g_fake_regulator_desc,
                                             &g_fake_regulator_ops,
                                             &g_fake_regulator);
  assert_true(NULL == g_fake_regulator.rdev);
  g_fake_regulator_supply.rdev = regulator_register(
                                             &g_fake_regulator_supply_desc,
                                             &g_fake_regulator_ops,
                                             &g_fake_regulator_supply);
  assert_false(NULL == g_fake_regulator_supply.rdev);
  assert_int_equal(g_fake_regulator_supply.state, 0);
  g_fake_regulator.rdev = regulator_register(&g_fake_regulator_desc,
                                             &g_fake_regulator_ops,
                                             &g_fake_regulator);
  assert_false(NULL == g_fake_regulator.rdev);
  assert_int_equal(g_fake_regulator_supply.state, 1);
  assert_int_equal(g_fake_regulator.state, 1);
  test = regulator_get(REGULATOR_ID);
  assert_false(NULL == test);
  while (cnt--)
    {
      ret = regulator_enable(test);
      assert_false(ret < 0);
      assert_int_equal(g_fake_regulator.state, 1);
      assert_int_equal(g_fake_regulator_supply.state, 1);
      ret = regulator_enable(test);
      assert_false(ret < 0);
      assert_int_equal(g_fake_regulator.state, 1);
      assert_int_equal(g_fake_regulator_supply.state, 1);
      ret = regulator_disable(test);
      assert_false(ret < 0);
      assert_int_equal(g_fake_regulator.state, 1);
      assert_int_equal(g_fake_regulator_supply.state, 1);
      ret = regulator_disable(test);
      assert_false(ret < 0);
      assert_int_equal(g_fake_regulator.state, 1);
      assert_int_equal(g_fake_regulator_supply.state, 1);
      ret = regulator_disable(test);
      assert_int_equal(ret, -EIO);
    }

  regulator_put(test);
  regulator_unregister(g_fake_regulator.rdev);
  g_fake_regulator.rdev = NULL;
  assert_int_equal(g_fake_regulator_supply.state, 0);
  regulator_unregister(g_fake_regulator_supply.rdev);
  g_fake_regulator_supply.rdev = NULL;
  return;
}

static void test_regulator_supply_3(FAR void **state)
{
  FAR struct regulator_s *test = NULL;
  int cnt = 10;
  int ret = 0;

  g_fake_regulator_desc.supply_name = REGULATOR_SUPPLY_ID;
  g_fake_regulator_desc.boot_on = 1;
  g_fake_regulator_desc.always_on = 0;
  g_fake_regulator.rdev = regulator_register(&g_fake_regulator_desc,
                                             &g_fake_regulator_ops,
                                             &g_fake_regulator);
  assert_true(NULL == g_fake_regulator.rdev);
  g_fake_regulator_supply.rdev = regulator_register(
                                             &g_fake_regulator_supply_desc,
                                             &g_fake_regulator_ops,
                                             &g_fake_regulator_supply);
  assert_false(NULL == g_fake_regulator_supply.rdev);
  assert_int_equal(g_fake_regulator_supply.state, 0);
  g_fake_regulator.rdev = regulator_register(&g_fake_regulator_desc,
                                             &g_fake_regulator_ops,
                                             &g_fake_regulator);
  assert_false(NULL == g_fake_regulator.rdev);
  assert_int_equal(g_fake_regulator_supply.state, 1);
  assert_int_equal(g_fake_regulator.state, 1);
  test = regulator_get(REGULATOR_ID);
  assert_false(NULL == test);
  while (cnt--)
    {
      ret = regulator_enable(test);
      assert_false(ret < 0);
      assert_int_equal(g_fake_regulator.state, 1);
      assert_int_equal(g_fake_regulator_supply.state, 1);
      ret = regulator_enable(test);
      assert_false(ret < 0);
      assert_int_equal(g_fake_regulator.state, 1);
      assert_int_equal(g_fake_regulator_supply.state, 1);
      ret = regulator_disable(test);
      assert_false(ret < 0);
      assert_int_equal(g_fake_regulator.state, 1);
      assert_int_equal(g_fake_regulator_supply.state, 1);
      ret = regulator_disable(test);
      assert_false(ret < 0);
      assert_int_equal(g_fake_regulator.state, 0);
      assert_int_equal(g_fake_regulator_supply.state, 0);
      ret = regulator_disable(test);
      assert_int_equal(ret, -EIO);
    }

  ret = regulator_enable(test);
  assert_false(ret < 0);
  assert_int_equal(g_fake_regulator.state, 1);
  assert_int_equal(g_fake_regulator_supply.state, 1);

  regulator_put(test);
  regulator_unregister(g_fake_regulator.rdev);
  g_fake_regulator.rdev = NULL;
  assert_int_equal(g_fake_regulator_supply.state, 0);
  regulator_unregister(g_fake_regulator_supply.rdev);
  g_fake_regulator_supply.rdev = NULL;
  return;
}

static void test_regulator_supply_4(FAR void **state)
{
  g_fake_regulator_desc.supply_name = REGULATOR_SUPPLY_ID;
  g_fake_regulator_desc.boot_on = 1;
  g_fake_regulator_desc.always_on = 0;
  g_fake_regulator.rdev = regulator_register(&g_fake_regulator_desc,
                                             &g_fake_regulator_ops,
                                             &g_fake_regulator);
  assert_true(NULL == g_fake_regulator.rdev);
  g_fake_regulator_supply.rdev = regulator_register(
                                             &g_fake_regulator_supply_desc,
                                             &g_fake_regulator_ops,
                                             &g_fake_regulator_supply);
  assert_false(NULL == g_fake_regulator_supply.rdev);
  assert_int_equal(g_fake_regulator_supply.state, 0);
  g_fake_regulator.rdev = regulator_register(&g_fake_regulator_desc,
                                             &g_fake_regulator_ops,
                                             &g_fake_regulator);
  assert_false(NULL == g_fake_regulator.rdev);
  assert_int_equal(g_fake_regulator_supply.state, 1);
  assert_int_equal(g_fake_regulator.state, 1);
  regulator_unregister(g_fake_regulator.rdev);
  g_fake_regulator.rdev = NULL;
  assert_int_equal(g_fake_regulator_supply.state, 0);
  regulator_unregister(g_fake_regulator_supply.rdev);
  g_fake_regulator_supply.rdev = NULL;
  return;
}

static void test_regulator_supply_5(FAR void **state)
{
  FAR struct regulator_s *test = NULL;
  int cnt = 10;
  int ret = 0;

  g_fake_regulator_desc.supply_name = REGULATOR_SUPPLY_ID;
  g_fake_regulator_desc.boot_on = 0;
  g_fake_regulator_desc.always_on = 0;
  g_fake_regulator.rdev = regulator_register(&g_fake_regulator_desc,
                                             &g_fake_regulator_ops,
                                             &g_fake_regulator);
  assert_false(NULL == g_fake_regulator.rdev);
  test = regulator_get(REGULATOR_ID);
  assert_true(NULL == test);
  g_fake_regulator_supply.rdev = regulator_register(
                                             &g_fake_regulator_supply_desc,
                                             &g_fake_regulator_ops,
                                             &g_fake_regulator_supply);
  assert_false(NULL == g_fake_regulator_supply.rdev);
  assert_int_equal(g_fake_regulator_supply.state, 0);
  assert_int_equal(g_fake_regulator.state, 0);
  test = regulator_get(REGULATOR_ID);
  assert_false(NULL == test);
  while (cnt--)
    {
      ret = regulator_enable(test);
      assert_false(ret < 0);
      assert_int_equal(g_fake_regulator.state, 1);
      assert_int_equal(g_fake_regulator_supply.state, 1);
      ret = regulator_enable(test);
      assert_false(ret < 0);
      assert_int_equal(g_fake_regulator.state, 1);
      assert_int_equal(g_fake_regulator_supply.state, 1);
      ret = regulator_disable(test);
      assert_false(ret < 0);
      assert_int_equal(g_fake_regulator.state, 1);
      assert_int_equal(g_fake_regulator_supply.state, 1);
      ret = regulator_disable(test);
      assert_false(ret < 0);
      assert_int_equal(g_fake_regulator.state, 0);
      assert_int_equal(g_fake_regulator_supply.state, 0);
      ret = regulator_disable(test);
      assert_int_equal(ret, -EIO);
    }

  ret = regulator_enable(test);
  assert_false(ret < 0);
  assert_int_equal(g_fake_regulator.state, 1);
  assert_int_equal(g_fake_regulator_supply.state, 1);
  regulator_put(test);
  regulator_unregister(g_fake_regulator.rdev);
  g_fake_regulator.rdev = NULL;
  assert_int_equal(g_fake_regulator_supply.state, 0);
  regulator_unregister(g_fake_regulator_supply.rdev);
  g_fake_regulator_supply.rdev = NULL;
  return;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  const struct CMUnitTest tests[] =
    {
      cmocka_unit_test_prestate(test_regulator_always_on, NULL),
      cmocka_unit_test_prestate(test_regulator_supply_1, NULL),
      cmocka_unit_test_prestate(test_regulator_supply_2, NULL),
      cmocka_unit_test_prestate(test_regulator_supply_3, NULL),
      cmocka_unit_test_prestate(test_regulator_supply_4, NULL),
      cmocka_unit_test_prestate(test_regulator_supply_5, NULL),
    };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
