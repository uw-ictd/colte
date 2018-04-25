/*
 * Copyright (c) 2015, EURECOM (www.eurecom.fr)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libgen.h>

#include "pid_file.h"
#include "log.h"

int    g_fd_pid_file = -1;

//------------------------------------------------------------------------------
char* get_exe_basename(void)
{

  char   pid_file_name[256] = {0};
  char   *exe_basename      = NULL;
  int    rv                 = 0;

  // get executable name
  rv = readlink("/proc/self/exe",pid_file_name, 256);
  if ( -1 == rv) {
    return NULL;
  }
  pid_file_name[rv] = 0;
  exe_basename = basename(pid_file_name);
  snprintf(pid_file_name, 128, "/var/run/%s.pid", exe_basename);
  return strdup(pid_file_name);
}

//------------------------------------------------------------------------------
int lockfile(int fd, int lock_type)
{
  // lock on fd only, not on file on disk (do not prevent another process from modifying the file)
  return lockf(fd, F_TLOCK, 0);
}

//------------------------------------------------------------------------------
bool is_pid_file_lock_success(char const *pid_file_name)
{
  char       pid_dec[32] = {0};

  g_fd_pid_file = open(pid_file_name,
                       O_RDWR | O_CREAT,
                       S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); /* Read/write by owner, read by grp, others */
  if ( 0 > g_fd_pid_file) {
    FPRINTF_DEBUG("filename %s failed %d:%s\n", pid_file_name, errno, strerror(errno));
    return false;
  }

  if ( 0 > lockfile(g_fd_pid_file, F_TLOCK)) {
    if ( EACCES == errno || EAGAIN == errno ) {
      close(g_fd_pid_file);
    }
    FPRINTF_DEBUG("filename %s failed %d:%s\n", pid_file_name, errno, strerror(errno));
    return false;
  }
  // fruncate file content
  if (! ftruncate(g_fd_pid_file, 0)) {
    // write PID in file
    sprintf(pid_dec, "%ld", (long)getpid());
    if (0 < write(g_fd_pid_file, pid_dec, strlen(pid_dec))) {
      return true;
    }
  }
  pid_file_unlock();
  close(g_fd_pid_file);
  return false;
}

//------------------------------------------------------------------------------
void pid_file_unlock(void)
{
  lockfile(g_fd_pid_file, F_ULOCK);
  close(g_fd_pid_file);
  g_fd_pid_file = -1;
}
