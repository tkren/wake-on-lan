/*
 *	wol - wake on lan client
 *
 *	$Id$
 *
 *	Copyright (C) 2000-2001 Thomas Krennwallner <krennwallner@aon.at>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *	USA.
 */


#ifndef _MACFILE_H
#define _MACFILE_H

#include <stdio.h>

FILE *macfile_open (const char *file);

int macfile_close (FILE *fp);

int macfile_line (FILE *fp, char **mac_str, char **ip_str,
										unsigned short *port);

#endif /* _MACFILE_H */
