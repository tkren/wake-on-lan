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



#ifndef _MAGIC_H
#define _MAGIC_H


#define MAGIC_HEADER 6
#define MAC_LEN      6
#define MAGIC_TIMES  16


struct
magic_header
{
	unsigned char magic_num[MAGIC_HEADER];
};


struct
magic_data
{
	unsigned char mac_address[MAGIC_TIMES][MAC_LEN];
};


typedef struct
{
	struct magic_header header;
	struct magic_data data;
} magic_packet;



magic_packet *magic_create (void);


void magic_destroy (magic_packet *m);


int magic_assemble (magic_packet *magic_buf, const char *mac_str);


#endif /* _MAGIC_H */
