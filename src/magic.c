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



/*
 *	How a Magic Packet Frame looks like:
 *
 *	 _________________________________________
 *	| 0xff | 0xff | 0xff | 0xff | 0xff | 0xff |
 *	|MAC[0]|MAC[1]|MAC[2]|MAC[3]|MAC[4]|MAC[5]|
 *	|MAC[0]|MAC[1]|MAC[2]|MAC[3]|MAC[4]|MAC[5]|
 *	|MAC[0]|MAC[1]|MAC[2]|MAC[3]|MAC[4]|MAC[5]|
 *	|MAC[0]|MAC[1]|MAC[2]|MAC[3]|MAC[4]|MAC[5]|
 *	|MAC[0]|MAC[1]|MAC[2]|MAC[3]|MAC[4]|MAC[5]|
 *	|MAC[0]|MAC[1]|MAC[2]|MAC[3]|MAC[4]|MAC[5]|
 *	|MAC[0]|MAC[1]|MAC[2]|MAC[3]|MAC[4]|MAC[5]|
 *	|MAC[0]|MAC[1]|MAC[2]|MAC[3]|MAC[4]|MAC[5]|
 *	|MAC[0]|MAC[1]|MAC[2]|MAC[3]|MAC[4]|MAC[5]|
 *	|MAC[0]|MAC[1]|MAC[2]|MAC[3]|MAC[4]|MAC[5]|
 *	|MAC[0]|MAC[1]|MAC[2]|MAC[3]|MAC[4]|MAC[5]|
 *	|MAC[0]|MAC[1]|MAC[2]|MAC[3]|MAC[4]|MAC[5]|
 *	|MAC[0]|MAC[1]|MAC[2]|MAC[3]|MAC[4]|MAC[5]|
 *	|MAC[0]|MAC[1]|MAC[2]|MAC[3]|MAC[4]|MAC[5]|
 *	|MAC[0]|MAC[1]|MAC[2]|MAC[3]|MAC[4]|MAC[5]|
 *	|MAC[0]|MAC[1]|MAC[2]|MAC[3]|MAC[4]|MAC[5]|
 *	 -----------------------------------------
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h> /* memset() in HP-UX */

#include "xmalloc.h"
#include "wol.h"
#include "magic.h"



magic_packet *
magic_create (void)
{
	return (magic_packet *) xmalloc (sizeof (magic_packet));
}



void
magic_destroy (magic_packet *m)
{
  xfree ((void *) m);
}



int
magic_assemble (magic_packet *magic_buf, const char *mac_str)
{
	int m[MAC_LEN];
	int j, k;

  if (mac_str == NULL || magic_buf == NULL) return -1;

	/* split the MAC address string into it's hex components */
	if (sscanf (mac_str, "%2x:%2x:%2x:%2x:%2x:%2x",
							&m[0], &m[1], &m[2], &m[3], &m[4], &m[5]) != MAC_LEN)
		{
			errno = EINVAL;
			return -1;
		}

	/* assemble magic packet header */
	memset ((void *) &magic_buf->header, 0xff, sizeof (struct magic_header));

  /* and now the data */
	for (j = 0; j < MAGIC_TIMES; j++)
		{
			for (k = 0; k < MAC_LEN; k++)
				{
					magic_buf->data.mac_address[j][k] = (unsigned char) m[k];
				}
		}

	return 0;
}
