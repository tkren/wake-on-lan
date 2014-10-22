/*
 * wol - wake on lan client
 *
 * $Id: net.c,v 1.9 2004/05/09 10:50:52 wol Exp $
 *
 * Copyright (C) 2000,2001,2002,2003,2004 Thomas Krennwallner <krennwallner@aon.at>
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


#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <error.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netpacket/packet.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>

#include "net.h"
#include "wol.h"

#if ! defined (ETH_P_WOL)
    #define ETH_P_WOL 0x0842
#endif


static int
net_resolv (const char *hostname, struct in_addr *sin_addr)
{
  struct hostent *hent;

  hent = gethostbyname (hostname);
  if (hent == NULL)
    {
      errno = EINVAL;
      return -1;
    }

  memcpy ((void *) sin_addr, (const void *) hent->h_addr, (size_t) hent->h_length);

  return 0;
}



int
net_close (int socket)
{
  if (close (socket))
    {
      perror ("Couldn't close socket");
      return -1;
    }

  return 0;
}



int
raw_open (void)
{
  int sockfd;

  sockfd = socket (PF_PACKET, SOCK_DGRAM, htons (ETH_P_WOL));
  if (sockfd < 0)
    {
      perror ("socket() failed");
      return -1;
    }

  return sockfd;
}



int
udp_open (void)
{
  int optval;
  int sockfd;

  sockfd = socket (PF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
    {
      perror ("socket() failed");
      return -1;
    }

  optval = 1;

  if (setsockopt (sockfd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof (optval)))
    {
      perror ("setsockopt() failed");
      close (sockfd);
      return -1;
    }

  return sockfd;
}



int
tcp_open (const char *ip_str,
	  unsigned int port)
{
  int sockfd;
  struct sockaddr_in toaddr;

  if (ip_str == NULL)
    {
      return -1;
    }
	
  memset (&toaddr, 0, sizeof (struct sockaddr_in));

  if (net_resolv (ip_str, &toaddr.sin_addr))
    {
      error (0, 0, _("Invalid IP address given: %s"), strerror (errno));
      return -1;
    }

  toaddr.sin_family = AF_INET;
  toaddr.sin_port = htons (port);

  sockfd = socket (PF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    {
      perror ("socket() failed");
      return -1;
    }

  if (connect (sockfd, (const struct sockaddr *) &toaddr, sizeof (toaddr)))
    {
      error (0, 0, _("Couldn't connect to %s:%hu: %s"), ip_str, port, strerror (errno));
      close (sockfd);
      return -1;
    }

  return sockfd;
}


ssize_t 
raw_send (int sock,
          const char *if_name,
	  const void *buf,
	  size_t len)
{
  struct ifreq ifr;
  int if_idx = -1;
  const unsigned char broadcast[ETHER_ADDR_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  struct sockaddr_ll toaddr;
  ssize_t sendret;

  if (if_name == NULL || buf == NULL)
    {
      return -1;
    }

  /* Find out index of the interface. */
  size_t if_name_len = strlen(if_name);
  memset (&ifr, sizeof (ifr), 0);
  if (if_name_len >= sizeof(ifr.ifr_name))
    {
      error (0, 0, _("Interface name too long"));
      return -1;
    }
  strcpy (ifr.ifr_name, if_name);
  if (ioctl (sock, SIOCGIFINDEX, &ifr))
    {
      error (0, 0, _("Couldn't find interface %s: %s"), if_name, strerror (errno));
      return -1;
    }
  if_idx = ifr.ifr_ifindex;

  memset (&toaddr, 0, sizeof (toaddr));

  toaddr.sll_family = AF_PACKET;

  /* Construct ll address. */
  memset (&toaddr, sizeof (toaddr), 0);
  toaddr.sll_family = AF_PACKET;
  toaddr.sll_protocol = htons (ETH_P_WOL);
  toaddr.sll_ifindex = if_idx;
  toaddr.sll_pkttype = PACKET_BROADCAST;
  assert (ETHER_ADDR_LEN <= sizeof (toaddr.sll_addr));
  toaddr.sll_halen = ETHER_ADDR_LEN;
  memcpy (toaddr.sll_addr, broadcast, ETHER_ADDR_LEN);

  /* keep on sending and check for possible errors */
  sendret = sendto (sock, buf, len, 0, (struct sockaddr *) &toaddr,
		    sizeof (toaddr));

  return sendret == -1 ? sendret : 0;
}



ssize_t 
udp_send (int sock,
	  const char *ip_str,
	  unsigned short int port,
	  const void *buf,
	  size_t len)
{
  struct sockaddr_in toaddr;
  ssize_t sendret;


  if (ip_str == NULL || buf == NULL)
    {
      return -1;
    }
	
  memset (&toaddr, 0, sizeof (toaddr));

  if (net_resolv (ip_str, &toaddr.sin_addr))
    {
      error (0, 0, _("Invalid IP address given: %s"), strerror (errno));
      return -1;
    }

  toaddr.sin_family = AF_INET;
  toaddr.sin_port = htons (port);

  /* keep on sending and check for possible errors */
  sendret = sendto (sock, buf, len, 0, (struct sockaddr *) &toaddr,
		    sizeof (toaddr));

  return sendret == -1 ? sendret : 0;
}



ssize_t
tcp_send (int sock,
	  const void *buf,
	  size_t len)
{
  return send (sock, buf, len, 0);
}



ssize_t
tcp_recv (int sock,
	  void *buf,
	  size_t len)
{
  return recv (sock, buf, len, 0);
}
