/****************************************************************************
 * apps/nshlib/nsh_wait.c
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

#include <nuttx/sched.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "nsh.h"
#include "nsh_console.h"

#if !defined(CONFIG_NSH_DISABLE_WAIT) && defined(CONFIG_SCHED_WAITPID)

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: cmd_wait
 *
 * Description:
 *   Handle 'cmd_wait' command from terminal.
 *   wait pid1 [pid2 [pid3] ..] - wait for a pid to exit.
 *
 * Input Parameters:
 *   vtbl - The NSH console.
 *   argc - Amount of argument strings in command.
 *   argv - The argument strings.
 *
 * Returned Value:
 *   Zero (OK) on success; a negated errno value on failure.
 *
 ****************************************************************************/

int cmd_wait(FAR struct nsh_vtbl_s *vtbl, int argc, FAR char **argv)
{
  FAR struct tcb_s *ptcb;
  FAR struct tcb_s *ctcb;
  int status = 0;
  int ret = OK;
  pid_t pid;

  if (argc == 1)
    {
      return OK;
    }

  ptcb = nxsched_self();
  for (int i = 1; i < argc; i++)
    {
      pid = atoi(argv[i]);
      if (pid == 0)
        {
          continue;
        }

      ctcb = nxsched_get_tcb(pid);
      if (ctcb == NULL)
        {
          continue;
        }

      if (ctcb->group == ptcb->group)
        {
          ret = pthread_join(pid, (FAR pthread_addr_t *)&status);
        }
      else
        {
          ret = waitpid(pid, &status, 0);
        }

      if (ret < 0)
        {
          nsh_error(vtbl, g_fmtcmdfailed,
                    argv[0], "wait", NSH_ERRNO);
          continue;
        }
    }

  return ret < 0 ? ret : status;
}

#endif /* CONFIG_NSH_DISABLE_WAIT */
