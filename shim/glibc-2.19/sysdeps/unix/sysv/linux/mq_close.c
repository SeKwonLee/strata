/* Copyright (C) 2004-2014 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <errno.h>
#include <mqueue.h>
#include <sysdep.h>

#ifdef __NR_mq_open

/* Removes the association between message queue descriptor MQDES and its
   message queue.  */
int
mq_close (mqd_t mqdes)
{
  return INLINE_SYSCALL (close, 1, mqdes);
}

#else
# include <rt/mq_close.c>
#endif
