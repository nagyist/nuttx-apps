/****************************************************************************
 * apps/system/perf-tools/evsel.c
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
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <nuttx/nuttx.h>

#include "evsel.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

FAR const char *const evsel_hw_names[PERF_COUNT_HW_MAX] =
{
  "cycles",
  "instructions",
  "cache-references",
  "cache-misses",
  "branches",
  "branch-misses",
  "bus-cycles",
  "stalled-cycles-frontend",
  "stalled-cycles-backend",
  "ref-cycles",
};

int default_hw_config[DEFAULT_HW_CONFIG_NUM] =
{
  PERF_COUNT_HW_CPU_CYCLES,
  PERF_COUNT_HW_INSTRUCTIONS,
  PERF_COUNT_HW_BRANCH_INSTRUCTIONS,
  PERF_COUNT_HW_BRANCH_MISSES,
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void perf_evsel_init(FAR struct perf_evsel_s *evsel,
                            FAR struct perf_event_attr_s *attr)
{
  list_initialize(&evsel->node);
  evsel->attr = *attr;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

FAR const char *evsel_get_hw_name(uint64_t config)
{
  if (config < PERF_COUNT_HW_MAX && evsel_hw_names[config])
    {
      return evsel_hw_names[config];
    }

  return "unknown-hardware";
}

void evsel_init(FAR struct evsel_s *evsel,
                FAR struct perf_event_attr_s *attr)
{
  perf_evsel_init(&evsel->core, attr);
  evsel->evlist = NULL;
}

FAR struct evsel_s *evsel_new(FAR struct perf_event_attr_s *attr)
{
  FAR struct evsel_s *evsel = zalloc(sizeof(struct evsel_s));

  if (!evsel)
    {
      return NULL;
    }

  evsel_init(evsel, attr);

  return evsel;
}

void evsel_delete(FAR struct evsel_s *evsel)
{
  if (!evsel)
    {
      return;
    }

  free(evsel);
}

int evsel_read_counter(FAR struct evsel_s *evsel,
                       FAR struct evlist_s *evlist)
{
  uint64_t count;

  if (read(evsel->core.evfd, &count, sizeof(count)) < 0)
    {
      printf("Read perf event data failed!\n");
      return -EINVAL;
    }

  evsel->core.count = count;

  return 0;
}

int evsel_open(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist)
{
  evsel->core.evfd = perf_event_open(&evsel->core.attr, evsel->core.pid,
                                     evsel->core.cpu, -1, O_CLOEXEC);
  if (evsel->core.evfd < 0)
    {
      printf("Perf event open failed! event_fd = %d\n", evsel->core.evfd);
      return -ENOENT;
    }

  return 0;
}

int evsel_close(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist)
{
  if (evsel->core.evfd < 0)
    {
      return -EINVAL;
    }

  close(evsel->core.evfd);

  return 0;
}

int evsel_reset(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist)
{
  if (evsel->core.evfd < 0)
    {
      return -EINVAL;
    }

  ioctl(evsel->core.evfd, PERF_EVENT_IOC_RESET, 0);

  return 0;
}

int evsel_enable(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist)
{
  if (evsel->core.evfd < 0)
    {
      return -EINVAL;
    }

  ioctl(evsel->core.evfd, PERF_EVENT_IOC_ENABLE, 0);

  return 0;
}

int evsel_disable(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist)
{
  if (evsel->core.evfd < 0)
    {
      return -EINVAL;
    }

  ioctl(evsel->core.evfd, PERF_EVENT_IOC_DISABLE, 0);

  return 0;
}

int evsel_count_start(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist)
{
  int status;

  status = evsel_open(evsel, evlist);
  if (status)
    {
      return status;
    }

  evsel_reset(evsel, evlist);
  evsel_enable(evsel, evlist);

  return status;
}

int evsel_count_end(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist)
{
  evsel_disable(evsel, evlist);
  evsel_close(evsel, evlist);

  return 0;
}
