#!/usr/bin/env python


from socket import *
from struct import *
from string import split, atoi
import os, sys, thread, threading




def getout (i):
	if i:
		print "ERROR"
		sys.exit (i)
	else:
		print "GOOD"
		sys.exit (0)




def wait_for_packet (lck, mac, sec):

	s = socket (AF_INET, SOCK_DGRAM)
	s.bind (("localhost", 40000))

	lck.acquire ()
	lck.notify ()
	lck.release ()

	r = s.recv (1024)
	l = len (r)

	if l < 102:
		print "Received " + `l` + " octets, too less"
		getout (1)

	data = unpack (`l` + 'B', r)

	header = data[0:6]

	if l > 102: # secureon
		magic = data[6:l-5]
		secureon = data[l-6:]
	else:
		magic = data[6:]


	print "Received " + `l` + " octets"

	for h in header:
		if h != 0xff:
			print "Error: Header not conforming"
			print "Header:   %2x %2x %2x %2x %2x %2x" % (header[0], header[1], header[2], header[3], header[4], header[5])
			getout (1)

	print "Header GOOD"

	for i in range (1, 16):
		off = i * 6
		for j in range (0, 6):
			if magic[off + j] != mac[j]:
				print "Error: Magic Data not conforming"
				print "Magic(" + `off` + "): %2x %2x %2x %2x %2x %2x" % (magic[0+off], magic[1+off], magic[2+off], magic[3+off], magic[4+off], magic[5+off])
				getout (1)

	print "Data GOOD"

	if l > 102: # SecureON
		for i in range (0, 6):
			if secureon[i] != sec[i]:
				print "Error: SecureON password not conforming"
				print "SecureON: %2x %2x %2x %2x %2x %2x" % (secureon[0], secureon[1], secureon[2], secureon[3], secureon[4], secureon[5])
				getout (1)

		print "SecureON GOOD"



if __name__ == "__main__":

	mac = []
	sec = []

	argc = len (sys.argv)


	if argc == 2:
		dmac = split (sys.argv[1], ":")
		for m in dmac: mac.append (atoi (m, 16))
		sec = [0x55, 0x44, 0x33, 0x22, 0x11, 0x00]
	elif argc == 3:
		dmac = split (sys.argv[1], ":")
		for m in dmac: mac.append (atoi (m, 16))
		dsec = split (sys.argv[2], "-")
		for s in dsec: sec.append (atoi (s, 16))
	else:
		mac = [0x00, 0x11, 0x22, 0x33, 0x44, 0x55]
		sec = [0x55, 0x44, 0x33, 0x22, 0x11, 0x00]

	lck = threading.Condition ()
	lck.acquire ()

	thread.start_new_thread (wait_for_packet, (lck, mac , sec))

	lck.wait ()
	lck.release ()

	macstr = "%x:%x:%x:%x:%x:%x" % (mac[0], mac[1], mac[2], mac[3], mac[4], mac[5])

	if sec != []:
		secstr = "%x-%x-%x-%x-%x-%x" % (sec[0], sec[1], sec[2], sec[3], sec[4], sec[5])
		wolout = os.popen ("../src/wol -i 127.0.0.1 " + macstr + " -P " + secstr)
	else:
		wolout = os.popen ("../src/wol -i 127.0.0.1 " + macstr)
