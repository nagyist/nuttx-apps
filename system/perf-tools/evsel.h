/****************************************************************************
 * apps/system/perf-tools/evsel.h
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

#ifndef __APPS_SYSTEM_PERF_TOOLS_EVSEL_H
#define __APPS_SYSTEM_PERF_TOOLS_EVSEL_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/list.h>
#include <nuttx/perf.h>

#include "evlist.h"

/****************************************************************************
 * Public Definitions
 ****************************************************************************/

struct perf_evsel_s
{
  struct list_node node;
  struct perf_event_attr_s attr;
  int evfd;
  int cpu;
  int pid;
  uint64_t count;
};

struct evsel_s
{
  struct perf_evsel_s core;
  FAR struct evlist_s *evlist;
};

#define DEFAULT_HW_CONFIG_NUM 4

/****************************************************************************
 * Public Data
 ****************************************************************************/

FAR extern const char *const evsel_hw_names[PERF_COUNT_HW_MAX];
extern int default_hw_config[DEFAULT_HW_CONFIG_NUM];

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

FAR const char *evsel_get_hw_name(uint64_t config);
FAR struct evsel_s *evsel_new(FAR struct perf_event_attr_s *attr);
void evsel_delete(FAR struct evsel_s *evsel);
int evsel_read_counter(FAR struct evsel_s *evsel,
                       FAR struct evlist_s *evlist);
int evsel_open(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist);
int evsel_close(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist);
int evsel_reset(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist);
int evsel_enable(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist);
int evsel_disable(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist);
int evsel_count_start(FAR struct evsel_s *evsel,
                      FAR struct evlist_s *evlist);
int evsel_count_end(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist);

#endif /* __APPS_SYSTEM_PERF_TOOLS_EVSEL_H */
