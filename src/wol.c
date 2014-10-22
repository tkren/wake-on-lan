/*
 * wol - wake on lan client
 *
 * main program
 * 
 * $Id: wol.c,v 1.19 2004/05/08 09:25:45 wol Exp $
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 */



#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <error.h>

#include "wrappers.h"
#include "xalloc.h"
#include "wol.h"
#include "magic.h"
#include "net.h"
#include "macfile.h"
#include "getpass4.h"
#include "proxy.h"



/* My name is argv[0], used by error() */
char *program_name;

/* pointer to a MAC address */
static char *mac_str = NULL;

/* network interface name. */
static char *if_name = NULL;

/* IP Address or hostname magic packet is addressed to */
static char *host_str = DEFAULT_IPADDR;

/* filename with mac addresses */
static char *pathname = NULL;

/* udp port */
static unsigned int port = DEFAULT_PORT;

/* SecureON password */
static char *passwd = NULL;

/* default is not to read from stdin */
static int request_stdin = 0;

/* be verbose */
static int verbose = 0;


/* send udp / raw / proxy packet */

#define UDP_MODE 0x1
#define RAW_MODE 0x2
#define PROXY_MODE 0x4

static unsigned int packet_mode = UDP_MODE;


/* how long to wait between packets */
static int msecs = 0;

/* a magic packet */
static struct magic *magic = NULL;

/* socket file descriptor */
static int sockfd = -1;



static void
usage (int status)
{
  if (status)
    {
      fprintf (stderr, _("Try `%s --help' for more information.\n"), program_name);
    }
  else
    {
      fprintf (stdout, _("\
Usage: %s [OPTION] ... MAC-ADDRESS ...\n\
Wake On LAN client - wakes up magic packet compliant machines.\n\n\
    --help          display this help and exit\n\
-V, --version       output version information and exit\n\
-v, --verbose       verbose output\n\
-w, --wait=NUM      wait NUM millisecs after sending\n\
-h, --host=HOST     broadcast to this IP address or hostname\n\
-i, --ipaddr=HOST   same as --host\n\
-p, --port=NUM      broadcast to this UDP port\n\
-f, --file=FILE     read addresses from file FILE (\"-\" reads from stdin)\n\
    --passwd[=PASS] send SecureON password PASS (if no PASS is given, you\n\
                    will be prompted for the password)\n\
-r, --raw=IF        send raw ethernet magic packet through interface IF\n\
-s, --proxy=HOST    send wake up information to wolp proxy HOST\n\
-u, --udp           send udp magic packet\n\
\n\
Each MAC-ADDRESS is written as x:x:x:x:x:x, where x is a hexadecimal number\n\
between 0 and ff which represents one byte of the address, which is in\n\
network byte order (big endian).\n"), program_name);

      fprintf (stdout, _("\n\
PASS is written as x-x-x-x-x-x, where x is a hexadecimal number between 0\n\
and ff which represents one byte of the password.\n"));

      fprintf (stdout, _("\nReport bugs to <krennwallner@aon.at>\n"));
    }

  exit (status);
}



static void
version (void)
{
  fprintf (stdout, PACKAGE " " VERSION "\n\n");
  fprintf (stdout, _("\
Copyright (C) 2000-2004 Thomas Krennwallner <krennwallner@aon.at>\n\
This is free software; see the source for copying conditions. There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\
\n"));
}



/* parse command line and set various globals */
static int
parse_args (int argc, char *argv[])
{
  int c;
  int option_index;
  int password_set = 0;
  char *options = "Vvw:h:i:p:f:s:r:u-";
  static struct option long_options[] = 
    {
      { "help", no_argument, NULL, 'H' },
      { "version", no_argument, NULL, 'V' },
      { "verbose", no_argument, NULL, 'v' },
      { "wait", required_argument, NULL, 'w' },
      { "host", required_argument, NULL, 'h' },
      { "ipaddr", required_argument, NULL, 'i' },
      { "port", required_argument, NULL, 'p' },
      { "file", required_argument, NULL, 'f' },
      { "passwd", optional_argument, NULL, 'P' },
      { "proxy", required_argument, NULL, 's' },
      { "raw", required_argument, NULL, 'r' },
      { "udp", no_argument, NULL, 'u' },
      { NULL, 0, NULL, 0 }
    };


  if (argc == 1)
    {
      error (0, 0, _("Too few arguments."));
      usage (1);
    }



  for (;;)
    {
      c = getopt_long (argc, argv, options, long_options, &option_index);
      if (c == -1) break;

      switch (c)
	{
	case 'H':
	  usage (0);
	  break;

	  
	case 'V':
	  version ();
	  exit (0);
	  break;

	  
	case 'v':
	  verbose = 1;
	  break;


	case 'w':
	  if (sscanf (optarg, "%u", &msecs) != 1)
	    {
	      error (0, 0, _("Invalid time given"));
	      usage (1);
	    }
	  msecs *= 1000;
	  break;


	case 'u':
	  packet_mode |= UDP_MODE;
	  packet_mode &= ~(RAW_MODE | PROXY_MODE);
	  break;


	case 'r':
	  packet_mode |= RAW_MODE;
	  packet_mode &= ~(UDP_MODE | PROXY_MODE);
	  if_name = optarg;
	  break;


	case 's':
	  packet_mode |= PROXY_MODE;
	  packet_mode &= ~(UDP_MODE | RAW_MODE);
	case 'h':
	case 'i':
	  host_str = optarg;
	  break;


	case 'p':
	  if ((sscanf (optarg, "%5u", &port) != 1) ||
	      port > 65535 || port == 0)
	    {
	      error (0, 0, _("Invalid port given"));
	      usage (1);
	    }
	  break; 


	case 'f':
	  pathname = optarg;
	  break;


	case 'P':
	  if (optarg == NULL)
	    {
	      size_t n;
	      
	      if (password_set)
		break;
	      
	      if (getpass4 (_("Password"), &passwd, &n, stdin) == -1)
		{
		  error (1, 0, "getpass4 failed");
		}
	      password_set = 1;
	    }
	  else
	    {
	      passwd = optarg;
	    }
	  break;


	case '?':
	  break;
	}
    }

  if ((optind == argc) && (pathname == NULL))
    {
      error (0, 0, _("You must specify at least one MAC-ADDRESS."));
      usage (1);
    }

  /* check if stdin is requested */
  if (optind < argc)
    {
      int i;

      for (i = optind; i < argc; ++i)
	{
	  if (argv[i][0] == '-' && argv[i][1] == 0)
	    {
	      request_stdin = 1;
	      break;
	    }
	}
    }
  else if (pathname != NULL)
    {
      if (!strncmp (pathname, "-", 1))
	{
	  request_stdin = 1;
	}
    }

  /* return the offset of the GNU getopt sorted parameters */
  return optind;
}



static int
assemble_and_send (struct magic *m,
		   const char *mac_str,
		   const char *host_str,
		   unsigned int portnum,
		   const char *pass_str,
		   int socketfd)
{
  if (packet_mode & UDP_MODE)
    {
      int ret = magic_assemble (m, mac_str, pass_str);
	
      switch (ret)
	{
	case -1:
	  error (0, errno, _("Cannot assemble magic packet for '%s'"), mac_str);
	  errno = 0;
	  return -1;
	  
	case -2:
	  error (0, 0, _("Invalid password given for '%s'"), mac_str);
	  errno = 0;
	  return -1;
	}

      if (udp_send (socketfd, host_str, portnum, m->packet, m->size))
	{
	  error (0, errno, _("Cannot send magic packet for '%s' to %s:%d"),
		 mac_str, host_str, portnum);
	  errno = 0;
	  return -1;
	}
    }
  else if (packet_mode & PROXY_MODE)
    {
      size_t n;
      char *proxy_pass = (char *) pass_str;
      int errval;

      /* request proxy password if it's not given */
      if (proxy_pass == NULL)
	{
	  if (getpass4 (_("Password"), &proxy_pass, &n, stdin) == -1)
	    {
	      error (1, 0, "getpass4 failed");
	    }
	}
      
      errval = proxy_send (socketfd, mac_str, proxy_pass);

      /* free possible new password */
      XFREE (proxy_pass);
      proxy_pass = NULL;

      if (errval)
	{
	  error (0, errno, _("Cannot send magic packet for '%s' to %s:%d"),
		 mac_str, host_str, portnum);
	  errno = 0;
	  return -1;
	}
    }
  else /* RAW_MODE */
    {
      int ret = magic_assemble (m, mac_str, pass_str);
	
      switch (ret)
	{
	case -1:
	  error (0, errno, _("Cannot assemble magic packet for '%s'"), mac_str);
	  errno = 0;
	  return -1;
	  
	case -2:
	  error (0, 0, _("Invalid password given for '%s'"), mac_str);
	  errno = 0;
	  return -1;
	}

      /* FIXME: if_name is a global variable. */
      raw_send (socketfd, if_name, m->packet, m->size);
    }

  fprintf (stdout, _("Waking up %s"), mac_str);
  if (verbose)
    {
      fprintf (stdout, _(" with %s:%d"), host_str, portnum);
    }
  fprintf (stdout, _("...\n"));

  if (msecs)
    {
      usleep (msecs);
    }

  return 0;
}



int 
main (int argc, char *argv[])
{
  int i;
  int ret = 0;

  /* my name is ... */
  program_name = argv[0];

#if ENABLE_NLS
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);
#endif /* ENABLE_NLS */
	
  i = parse_args (argc, argv);

  magic = magic_create (passwd != NULL);
  if (magic == NULL)
    {
      exit (1);
    }

  if (packet_mode & UDP_MODE)
    {
      sockfd = udp_open ();
    }
  else if (packet_mode & PROXY_MODE)
    {
      sockfd = tcp_open (host_str, port);
    }
  else /* RAW_MODE */
    {
      sockfd = raw_open ();
    }

  if (sockfd < 0)
    {
      exit (1);
    }


  /* loop through possible MAC addresses */
  if (!request_stdin)
    {
      for (; i < argc; i++)
	{
	  ret -= assemble_and_send (magic, argv[i], host_str, port, passwd, sockfd);
	}
    }


  /* -f given */
  if (pathname || request_stdin)
    {
      FILE *fp;

      if (request_stdin)
	{
	  fp = stdin;
	}
      else
	{
	  fp = fopen (pathname, "r");
	  if (fp == NULL)
	    {
	      error (1, errno, "%s", pathname);
	    }
	}

      /* loop through fp */
      for (;;)
	{
	  if (macfile_parse (fp, &mac_str, &host_str, &port, &passwd)) break;
	  
	  if (port == 0 || port > 65535)
	    {
	      port = DEFAULT_PORT;
	    }
	  
	  ret -= assemble_and_send (magic, mac_str, host_str, port, passwd, sockfd);
	  
	  XFREE (mac_str);
	  XFREE (host_str);
	  XFREE (passwd);
	}

      fclose (fp);
    }

  net_close (sockfd);

  magic_destroy (magic);
  
  exit (ret != 0);
}
