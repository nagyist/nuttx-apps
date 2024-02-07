/****************************************************************************
 * apps/system/perf-tools/parse-options.c
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

#include <getopt.h>
#include <stdio.h>

#include "parse-options.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int parse_stat_options(int argc, FAR char **argv,
                       FAR struct stat_args_s *stat_args)
{
  int opt;

  stat_args->cmd_nr = argc;

  while ((opt = getopt(argc, argv, ":aC:p:e:")) != -1)
    {
      switch (opt)
        {
          case 'a':
            if (stat_args->type == STAT_ARGS_NONE ||
                stat_args->type == STAT_ARGS_CPU)
              {
                stat_args->type = STAT_ARGS_ALL;
                stat_args->cpu = -1;
                stat_args->pid = -1;
                stat_args->events = NULL;
              }

            stat_args->cmd_nr -= 2;
            break;
          case 'C':
            if (stat_args->type == STAT_ARGS_NONE)
              {
                stat_args->type = STAT_ARGS_CPU;
                stat_args->cpu = atoi(optarg);
                stat_args->pid = -1;
                stat_args->events = NULL;
              }
            else if (stat_args->type == STAT_ARGS_EVENT)
              {
                stat_args->cpu = atoi(optarg);
                stat_args->pid = -1;
                stat_args->cmd_nr += 1;
              }
            else if (stat_args->type == STAT_ARGS_TASK)
              {
                printf("PID/TID switch overriding CPU\n");
                stat_args->cpu = atoi(optarg);
                stat_args->cmd_nr += 1;
              }

            stat_args->cmd_nr -= 3;
            break;
          case 'p':
            if (stat_args->type == STAT_ARGS_NONE)
              {
                stat_args->type = STAT_ARGS_TASK;
                stat_args->pid = atoi(optarg);
                stat_args->cpu = -1;
                stat_args->events = NULL;
              }
            else if (stat_args->type == STAT_ARGS_EVENT)
              {
                stat_args->pid = atoi(optarg);
                stat_args->cpu = -1;
                stat_args->cmd_nr += 1;
              }
            else if (stat_args->type == STAT_ARGS_CPU)
              {
                printf("PID/TID switch overriding CPU\n");
                stat_args->type = STAT_ARGS_TASK;
                stat_args->pid = atoi(optarg);
                stat_args->cmd_nr += 1;
              }

            stat_args->cmd_nr -= 3;
            break;
          case 'e':
            if (stat_args->type == STAT_ARGS_NONE ||
                stat_args->type == STAT_ARGS_ALL)
              {
                stat_args->cpu = -1;
                stat_args->pid = -1;
              }
            else if (stat_args->type == STAT_ARGS_CPU)
              {
                stat_args->pid = -1;
                stat_args->cmd_nr += 1;
              }

            stat_args->type = STAT_ARGS_EVENT;
            stat_args->events = optarg;
            stat_args->cmd_nr -= 3;
            break;
          case 'h':
            stat_args->type = STAT_ARGS_HELP;
            break;
          default:
            return -EINVAL;
        }
    }

  stat_args->cmd = argv[optind];
  stat_args->cmd_args = &argv[optind];

  if (argc == 1)
    {
      stat_args->type = STAT_ARGS_ALL;
      stat_args->cpu = -1;
      stat_args->pid = -1;
      stat_args->events = NULL;
      stat_args->cmd_nr -= 1;
    }
  else if (stat_args->cmd_nr > 0)
    {
      if (stat_args->type == STAT_ARGS_NONE)
        {
          stat_args->type = STAT_ARGS_FORK;
          stat_args->cpu = -1;
          stat_args->pid = _SCHED_GETPID();
          stat_args->events = NULL;
          stat_args->cmd_nr -= 1;
        }
      else if (stat_args->type == STAT_ARGS_CPU ||
               stat_args->type == STAT_ARGS_EVENT)
        {
          stat_args->type = STAT_ARGS_FORK;
          stat_args->pid = _SCHED_GETPID();
        }
      else if (stat_args->type == STAT_ARGS_TASK)
        {
          stat_args->cmd = NULL;
          stat_args->cmd_nr = 0;
        }
    }

  if (stat_args->cpu >= CONFIG_SMP_NCPUS)
    {
      printf(" Perf can support %d CPUs.\n", CONFIG_SMP_NCPUS);
      return -EINVAL;
    }

  return 0;
}
