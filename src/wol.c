/*
 *	wol - wake on lan client
 *
 *	main program
 * 
 *	$Id: wol.c,v 1.3 2002/01/10 07:43:59 wol Exp $
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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

#include "wrappers.h"
#include "xmalloc.h"
#include "wol.h"
#include "magic.h"
#include "net.h"
#include "macfile.h"


#define MAC_STRING_LEN 22
#define IP_STRING_LEN 18
#define PASSWD_STRING_LEN 18


/* My name is argv[0] */
static char *name;

/* pointer to a MAC address */
static char *mac_str = NULL;

/* IP Address magic packet is addressed to */
static char *in_addr_str = DEFAULT_IPADDR;

/* filename with mac addresses */
static char *pathname = NULL;

/* udp port */
static unsigned short port = DEFAULT_PORT;

/* SecureON (tm) password */
static char *passwd = NULL;

/* be verbose */
static int verbose = 0;

/* how long to wait between packets */
static int msecs = 0;

/* a magic packet */
static struct magic *magic = NULL;

/* socket file descriptor */
static int sockfd = -1;



static void
usage (void)
{
	fprintf (stdout, _("\
Usage: %s [OPTION] ... MAC-ADDRESS ...\n\
Wake On LAN client - wakes up magic packet compliant machines.\n\n\
-h, --help          display this help and exit\n\
-V, --version       output version information and exit\n\
-v, --verbose       verbose output\n\
-w, --wait=NUM      wait NUM millisecs after sending\n\
-i, --ipaddr=IPADDR broadcast to this IP address\n\
-p, --port=NUM      broadcast to this UDP port\n\
-f, --file=FILE     read addresses from file FILE\n\
-P, --passwd=PASS   send SecureON password PASS\n\
\n\
Each MAC-ADDRESS is written as x:x:x:x:x:x, where x is a hexadecimal number\n\
between 0 and ff which represents one byte of the address, which is in\n\
network byte order (big endian).\n"), name);

	fprintf (stdout, _("\nReport bugs to <krennwallner@aon.at>\n"));
}



static void
version (void)
{
	fprintf (stdout, PACKAGE " " VERSION "\n\n");
	fprintf (stdout, _("\
Copyright (C) 2000-2002 Thomas Krennwallner <krennwallner@aon.at>\n\
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
	char *options = "hVvw:i:p:f:P:";
	static struct option long_options[] = 
		{
			{ "help", no_argument, NULL, 'h' },
			{ "version", no_argument, NULL, 'V' },
			{ "verbose", no_argument, NULL, 'v' },
			{ "wait", required_argument, NULL, 'w' },
			{ "ipaddr", required_argument, NULL, 'i' },
			{ "port", required_argument, NULL, 'p' },
			{ "file", required_argument, NULL, 'f' },
			{ "passwd", required_argument, NULL, 'P' },
			{ NULL, 0, NULL, 0 }
		};


	if (argc == 1)
		{
			fprintf (stderr, _("\
%s: Too few arguments.\n\
Try `%s --help' for more information.\n"), name, name);
			exit (1);
		}


	for (;;)
		{
			c = getopt_long (argc, argv, options, long_options, &option_index);
			if (c == -1) break;

			switch (c)
				{
					case 'h':
						usage ();
						exit (0);
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
								fprintf (stderr, _("%s: Invalid time given\n"), name);
								usage ();
								exit (1);
							}
						msecs *= 1000;
						break;


					case 'i':
						in_addr_str = optarg;
						break;


					case 'p':
						if ((sscanf (optarg, "%5hu", &port) != 1) || port > 65535)
							{
								fprintf (stderr, _("%s: Invalid port given\n"), name);
								usage ();
								exit (1);
							}
						break; 


					case 'f':
						pathname = optarg;
						break;


					case 'P':
						passwd = optarg;
						break;


					case '?':
						break;


					default:
						usage ();
						exit (1);
				}
		}

	/* return the offset of the GNU getopt sorted parameters */
	return optind;
}



static int
assemble_and_send (struct magic *m, const char *mac_str, const char *ip_str,
										unsigned short portnum, const char *pass_str, int socketfd)
{
	if (magic_assemble (m, mac_str, pass_str))
		{
			fprintf (stderr, _("%s: Cannot assemble magic packet for '%s': %s\n"),
								name, mac_str, strerror (errno));
			return -1;
		}

	if (net_send (socketfd, ip_str, portnum, m->packet, m->size))
		{
			fprintf (stderr,
								_("%s: Cannot send magic packet for '%s' to %s:%d: %s\n"),
								name, mac_str, ip_str, portnum, strerror (errno));
			return -1;
		}

	fprintf (stdout, _("Waking up %s"), mac_str);
	if (verbose) fprintf (stdout, _(" with %s:%d"), ip_str, portnum);
	fprintf (stdout, _("...\n"));

	usleep (msecs);

	return 0;
}



int 
main (int argc, char *argv[])
{
	int i;
	int ret = 0;

	/* my name is ... */
	name = argv[0];

#if ENABLE_NLS
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, LOCALEDIR);
	textdomain (PACKAGE);
#endif /* ENABLE_NLS */
	
	i = parse_args (argc, argv);

	magic = magic_create (passwd != NULL);
	if (magic == NULL) exit (1);

	sockfd = net_open ();
	if (sockfd < 0) exit (1);


	/* loop through possible MAC addresses */
	for (; i < argc; i++)
		{
			ret -= assemble_and_send (magic, argv[i], in_addr_str, port, passwd,
																sockfd);
		}


	/* -f given */
	if (pathname)
		{
			FILE *fp;

			fp = fopen (pathname, "r");
			if (fp == NULL)
				{
					fprintf (stderr, "%s: %s: %s\n",
														name, pathname, strerror (errno));
					exit (1);
				}

			mac_str = (char *) xmalloc (MAC_STRING_LEN);
			in_addr_str = (char *) xmalloc (IP_STRING_LEN);
			passwd = (char *) xmalloc (PASSWD_STRING_LEN);

			/* loop through fp */
			for (;;)
				{
					if (macfile_parse (fp, mac_str, in_addr_str, &port, passwd)) break;

					ret -= assemble_and_send (magic, mac_str, in_addr_str, port, passwd,
																		sockfd);
				}

			xfree (mac_str);
			xfree (in_addr_str);
			xfree (passwd);

			fclose (fp);
		}

	net_close (sockfd);

	magic_destroy (magic);

	exit (ret != 0);
}
