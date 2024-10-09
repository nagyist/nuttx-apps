/****************************************************************************
 * apps/builtin/exec_builtin.c
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

#include <fcntl.h>
#include <errno.h>
#include <debug.h>

#include "nshlib/nshlib.h"
#include "builtin/builtin.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: exec_builtin
 *
 * Description:
 *   Executes builtin applications registered during 'make context' time.
 *   New application is run in a separate task context (and thread).
 *
 * Input Parameter:
 *   filename  - Name of the linked-in binary to be started.
 *   argv      - Argument list
 *   redirfile - If output is redirected, this parameter will be non-NULL
 *               and will provide the full path to the file.
 *   oflags    - If output is redirected, this parameter will provide the
 *               open flags to use.  This will support file replacement
 *               of appending to an existing file.
 *
 * Returned Value:
 *   This is an end-user function, so it follows the normal convention:
 *   Returns the PID of the exec'ed module.  On failure, it returns
 *   -1 (ERROR) and sets errno appropriately.
 *
 ****************************************************************************/

int exec_builtin(FAR const char *appname, FAR char * const *argv,
                 FAR const char *redirfile, int oflags)
{
  FAR const struct builtin_s *builtin;
  int index;

  /* Verify that an application with this name exists */

  index = builtin_isavail(appname);
  if (index < 0)
    {
      errno = ENOENT;
      return ERROR;
    }

  /* Get information about the builtin */

  builtin = builtin_for_index(index);
  if (builtin == NULL)
    {
      errno = ENOENT;
      return ERROR;
    }

  return nsh_spawn(builtin->name, builtin->main, argv, builtin->priority,
                   builtin->stacksize, redirfile, oflags, false);
}
