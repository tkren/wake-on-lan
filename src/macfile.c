/*
 *	wol - wake on lan client
 *
 *	parses a macfile and return its tokens
 * 
 *	$Id: macfile.c,v 1.1.1.1 2001/11/06 19:31:37 wol Exp $
 *
 *	Copyright (C) 2000-2002 Thomas Krennwallner <krennwallner@aon.at>
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



#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>
#include <errno.h>

#include "wol.h"
#include "macfile.h"
#include "xalloc.h"
#include "getline.h"



#define MAC_FMT  "%17[0-9a-fA-F:]"
#define IP_FMT   "%15[0-9.]"
#define PORT_FMT ":%5u"
#define PASS_FMT "%17[0-9a-fA-F-]"



static int
get_tokens (char *str, char *mac, char *ip, unsigned int *port, char *passwd)
{
	if (sscanf (str, " " MAC_FMT " " IP_FMT PORT_FMT " " PASS_FMT,
											mac, ip, port, passwd) != 4)
		{
			if (sscanf (str, " " MAC_FMT " " IP_FMT " " PASS_FMT,
												mac, ip, passwd) != 3)
				{
					if (sscanf (str, " " MAC_FMT " " IP_FMT PORT_FMT,
														mac, ip, port) != 3)
						{
							if (sscanf (str, " " MAC_FMT " " IP_FMT,
																mac, ip) != 2)
								{
									return -1;
								}
						}
				}
		}

	return 0;
}



int
macfile_parse (FILE *fp, char **mac_str, char **ip_str, unsigned short *port,
								char **passwd_str)
{
	char *willy = NULL;
	ssize_t whale = 0, ret;
	char mac[18], ip[16], passwd[18];

	*port = 0;

	while (!feof (fp) && !ferror (fp))
		{
			if ((ret = getline (&willy, &whale, fp)) == -1)
				return -1;

			if (get_tokens (willy, mac, ip, (unsigned int *) port, passwd))
				{
					XFREE (willy);
					continue;	
				}
	
			XFREE (willy);
			break;
		}

	if (ferror (fp))
		return -1;

	*mac_str = strdup (mac);
	*ip_str = strdup (ip);
	*passwd_str = strdup (passwd);

	return 0;
}
