#!/bin/sh

NICFAKE=./nic-fake.py
WOL=../src/wol
fifo=/tmp/test1.fifo

mkfifo $fifo
test $? -ne 0 && echo "Couldn't create fifo"; exit 1

$NICFAKE 10:11:22:33:44:55 > $fifo &
sleep 1
$WOL -i 127.0.0.1 00:11:22:33:44:55 > /dev/null
test $? -ne 0 && echo "$WOL failed"; exit 1

msg=`cat $fifo`
rm $fifo

result=`echo $msg | tail -n 1`

test $result = "ERROR" && echo "Error: $msg"; exit 1
