#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#ifdef HAVE_NETINET_ETHER_H
#include <netinet/ether.h>
#endif /* HAVE_NETINET_ETHER_H */


#ifndef _ETHER_H
#define _ETHER_H


#if !HAVE_STRUCT_ETHER_ADDR_ETHER_ADDR_OCTET && !HAVE_STRUCT_ETHER_ADDR_OCTET
#define HAVE_STRUCT_ETHER_ADDR_ETHER_ADDR_OCTET 1
struct
ether_addr
{
	unsigned char ether_addr_octet[MAC_LEN];
};
#endif /* !HAVE_STRUCT_ETHER_ADDR_ETHER_ADDR_OCTET && !HAVE_STRUCT_ETHER_ADDR_OCTET */

#if HAVE_STRUCT_ETHER_ADDR_OCTET
#define ETHER_ADDR_OCTET octet
#endif /* HAVE_STRUCT_ETHER_ADDR_OCTET */

#if HAVE_STRUCT_ETHER_ADDR_ETHER_ADDR_OCTET
#define ETHER_ADDR_OCTET ether_addr_octet
#endif /* HAVE_STRUCT_ETHER_ADDR_ETHER_ADDR_OCTET */

#if !defined(HAVE_ETHER_HOSTTON)
int ether_hostton (const char *str, struct ether_addr *ea);
#endif /* HAVE_ETHER_HOSTTON */

#endif /* _ETHER_H */
