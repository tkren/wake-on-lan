/*
 * wol - wake on lan client
 *
 * $Id: proxy.c,v 1.1 2004/02/05 18:14:48 wol Exp $
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
#include <errno.h>

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

  len = tcp_recv (sock, &rsbuf, HEX_CHALLENGE);

  if (len < HEX_CHALLENGE)
    {
      errno = EINVAL;
      return -1;
    }

  printf ("recvd: %s", rsbuf);

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

  memset (md5sum, 0, sizeof (md5sum));
  md5_buffer (buf, len, md5sum);

  XFREE (buf);

  printf ("sum %c%c%c %d\n", md5sum[0],md5sum[1],md5sum[2],len);

  for (i = j = 0; i < MD5_LENGTH; i++, j += 2)
    {
      snprintf (&rsbuf[j], 2, "%2x", md5sum[i]);
    }

  rsbuf[HEX_CHALLENGE] = '\n';

  printf ("reply: %s", rsbuf);

  /* send to proxy */

  return tcp_send (sock, rsbuf, HEX_CHALLENGE);
}
