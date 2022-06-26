#!/bin/bash
make
echo ">>>>> Loading md1.ko..."
sleep 0.8
insmod md1.ko
lsmod | grep md
sleep 1
echo ">>>>> System log after module insert:"
dmesg | tail -5 | grep +
echo ">>>>> Remove md1..."
rmmod md1
sleep 0.8
echo ">>>>> System log after module removal:"
dmesg | tail -5 | grep +
