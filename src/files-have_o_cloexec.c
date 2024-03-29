/* Copyright (C) 2007-2014 Free Software Foundation, Inc.
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
/*
 * Much of this code has been copied from nss_files and has
 * been adapted to suite our needs. The copied code has the above copyright
 * header
 *
 * Brocade Communications Systems 2016
 */

#include <fcntl.h>

#if defined O_CLOEXEC && !defined __ASSUME_O_CLOEXEC
int __have_o_cloexec;
#endif
