/*
 * wol - wake on lan client
 *
 * $Id$
 *
 * Copyright (C) 2004 Thomas Krennwallner <krennwallner@aon.at>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */



#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <string.h>
#include <stdlib.h>

#include "wrappers.h"
#include "xalloc.h"
#include "magic.h"
#include "proxy.h"
#include "md5.h"
#include "net.h"

#define MD5_LENGTH 16
#define CHALLENGE_LENGTH 16
#define HEX_CHALLENGE CHALLENGE_LENGTH * 2 + 1


int
proxy_send (int sock,
	    const char *macbuf,
	    const char *passwd)
{
  size_t i, j, len;
  unsigned char rsbuf[HEX_CHALLENGE];
  unsigned char challenge[CHALLENGE_LENGTH];
  unsigned char md5sum[MD5_LENGTH];
  unsigned char *buf, *tmp;

  /* receive challenge in hex format */

  if (tcp_recv (sock, &rsbuf, HEX_CHALLENGE))
    {
      return -1;
    }

  for (i = j = 0; i < HEX_CHALLENGE - 1; i += 2, j++)
    {
      sscanf (&rsbuf[i], "%2x", (unsigned int *) &challenge[j]);
    }

  /* compute reply */

  len = sizeof (challenge) + MAC_LEN + strlen (passwd);

  buf = tmp = (unsigned char *) xmalloc (len);
  
  memcpy (tmp, challenge, sizeof (challenge));

  tmp += sizeof (challenge);
  memcpy (tmp, macbuf, MAC_LEN);

  tmp += MAC_LEN;
  memcpy (tmp, passwd, strlen (passwd));

  md5_buffer (buf, len, md5sum);

  XFREE (buf);

  for (i = j = 0; i < HEX_CHALLENGE - 1; i += 2, j++)
    {
      snprintf (&rsbuf[i], 2, "%2x", md5sum[j]);
    }

  rsbuf[HEX_CHALLENGE] = '\n';

  /* send to proxy */

  return tcp_send (sock, rsbuf, HEX_CHALLENGE);
}
