/*
 *	$Id: xmalloc.c,v 1.1.1.1 2001/11/06 19:31:32 wol Exp $
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


void *
xmalloc (size_t n)
{
	void *buf;
	
	if (n == 0)
		{
			fprintf (stderr, "%s: zero size\n", __FUNCTION__);
			exit (1);
		}

	buf = malloc (n);

	if (buf == NULL)
		{
			fprintf (stderr, "%s: Memory exhaust: %s\n",
								__FUNCTION__, strerror (errno));
			exit (1);
		}

	return buf;
}


void *
xrealloc (void *buf, size_t n)
{
  buf = realloc (buf, n);

	if (buf == NULL)
		{
			fprintf (stderr, "%s: Memory exhaust: %s\n",
								__FUNCTION__, strerror (errno));
			exit (1);
		}

	return buf;
}


void
xfree (void *buf)
{
	if (buf == NULL)
		{
			fprintf (stderr, "%s: NULL pointer given\n", __FUNCTION__);
			exit (1);
		}
	free (buf);
	buf = NULL;
}


char *
xstrdup (const char *str)
{
	size_t len;
	char *copy;

	len = strlen (str) + 1;
	copy = (char *) xmalloc (len);
	if ((copy = strncpy (copy, str, len)) == NULL)
		{
			fprintf (stderr, "%s: failed to copy string\n", __FUNCTION__);
			exit (1);
		}

	return copy;
}
