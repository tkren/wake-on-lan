%{
/*
 *	wol - wake on lan client
 *
 *	$Id$
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


#define YY_DECL \
				int yylex (char *mac, char *ip, unsigned short *port, char *passwd)

%}


%x further


%%

[[:blank:]]*[0-9a-fA-F]{1,2}:[0-9a-fA-F]{1,2}:[0-9a-fA-F]{1,2}:[0-9a-fA-F]{1,2}:[0-9a-fA-F]{1,2}:[0-9a-fA-F]{1,2}[[:blank:]]+[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}	{
	sscanf (yytext, " %s %s", mac, ip);
	BEGIN(further);
}

<further>{

:[0-9]{1,5}	{
	sscanf (yytext, ":%u", (unsigned int *) port);
}

[[:blank:]]+[0-9a-fA-F]{1,2}-[0-9a-fA-F]{1,2}-[0-9a-fA-F]{1,2}-[0-9a-fA-F]{1,2}	{
	sscanf (yytext, " %s", passwd);
}

\n|.	{
	BEGIN(INITIAL);
	return 0;
}

}


<<EOF>>	{
	BEGIN(INITIAL);
	return -1;
}


.	{
	BEGIN(INITIAL);
}


%%



int
macfile_parse (FILE *fp, char *mac, char *ip, unsigned short *port,
								char *passwd)
{
	yyin = fp;
	yyout = fopen ("/dev/null", "w");
	return yylex (mac, ip, port, passwd);
}
