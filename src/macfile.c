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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>
#include <errno.h>

#include "wol.h"
#include "macfile.h"
#include "xmalloc.h"



static void
get_line (FILE *fp, char **linebuf, size_t n)
{
	int c;
	size_t offset;
	char *tmp;
	

	assert (linebuf != NULL);

	n = n ? n : 10;
	tmp = (char *) xmalloc (n);
	memset ((void *) tmp, '\0', n);

	/* read a line not including '\n' */
	for (offset = 0; (c = getc (fp)) != EOF && c != '\n'; offset++)
		{
			/* if tmp is too small, double his size */
			if (offset > (n - 1))
				{
					n <<= 2;
					tmp = (char *) xrealloc ((void *) tmp, n);
				}

			tmp[offset] = c;
		}

	tmp[offset] = '\0';

	*linebuf = xstrdup (tmp);
	xfree (tmp);
}



FILE *
macfile_open (const char *path)
{
	return fopen (path, "r");
}



int
macfile_close (FILE *fp)
{
	return fclose (fp);
}



int
macfile_line (FILE *fp, char **mac_str, char **ip_str, unsigned short *port)
{
	char *willy = NULL;
	char *tmp;
	size_t whale;
	char mac[18], ip[22];


	if (feof (fp) || ferror (fp)) return -1;

	assert (fp != NULL && mac_str != NULL && ip_str != NULL && port != NULL);

	whale = 40; /* size for eg. "ff:ff:ff:ff:ff:ff 255.255.255.255:65535" */

	while (!feof (fp) && !ferror (fp))
		{
			get_line (fp, &willy, whale);

			if (sscanf (willy, " %17[0-9A-Fa-f:] %21[0-9.:]", mac, ip) != 2)
				{
					xfree (willy);
					continue;	
				}
	
			xfree (willy);
			break;
		}

	if (feof (fp) || ferror (fp)) return -1;

	/* extract optional port */
	tmp = strchr (ip, ':');
	if (tmp)
		{
			unsigned int tmpport;

			tmp++;
			if (sscanf (tmp, "%5u", &tmpport) != 1 || tmpport > 65535)
				*port = DEFAULT_PORT;
			else
				*port = (unsigned short int) tmpport;
			tmp--;
			*tmp = '\0';
		}
	else
		*port = DEFAULT_PORT;
	
	*ip_str = xstrdup (ip);
	*mac_str = xstrdup (mac);

	return 0;
}
