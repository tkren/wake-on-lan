#!/usr/bin/env python

from socket import *
from struct import *

while 1:
	s = socket (AF_INET, SOCK_DGRAM)
	s.bind (("localhost", 40000))
	r = s.recv (1024)
	l = len (r)
	data = unpack (`l` + 'B', r)

	header = data[0:6]

	if l > 102:
		magic = data[6:l-5]
		secureon = data[l-6:]
	else:
		magic = data[6:]
		secureon = 0

	print
	print "NEW PACKET RECEIVED: " + `l` + " bytes"
	print "Header: " + `header`
	print "Magic: " + `magic`
	print "SecureOn: " + `secureon`
	print
	print "Woke up "
	for i in range (0, 5):
		print `magic[i]` + ":"
	print `magic[i]`
