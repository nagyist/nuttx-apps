/****************************************************************************
 * apps/system/perf-tools/builtin-list.c
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

#include <stdlib.h>
#include <stdio.h>

#include "evsel.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int cmd_list(int argc, FAR const char **argv)
{
  int i;

  printf("List of pre-defined events (to be used in -e):\n\n");

  for (i = 0; i < PERF_COUNT_HW_MAX; i++)
    {
      printf("  %-32s\t [PMU event]\n", evsel_hw_names[i]);
    }

  return 0;
}
