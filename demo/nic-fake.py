#!/usr/bin/env python

from socket import *

while 1:
	s = socket (AF_INET, SOCK_DGRAM)
	s.bind (("localhost", 40000))
	r = s.recv (1024)
	print
	print "NEW PACKET RECEIVED: " + `len (r)` + " bytes"
	print "Packet header: " + `r[0:6]`
	print "Packet data:" 
	for i in range (0, 16):
		print "\t" + `r[6+6*i:12+6*i]`

	print'Woke up %s:%s:%s:%s:%s:%s' % (`r[6]`, `r[7]`, `r[8]`, `r[9]`, `r[10]`, `r[11]`)
