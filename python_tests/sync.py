#!/usr/bin/python
# -*- coding: utf-8 -*-
import serial
import sys

port = serial.Serial("/dev/ttyUSB1", baudrate=9600, timeout=3.0)
# synchronizace
ts = port.read(10)
i=0
while ts == "":
	port.write(b"\x33")
	ts = port.read(20)
	print "{:02x}".format(ord(ts)) 
