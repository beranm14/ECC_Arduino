#!/usr/bin/python
# -*- coding: utf-8 -*-
import serial
import sys

port = serial.Serial("/dev/ttyUSB0", baudrate=9600, timeout=3.0)
# synchronizace
ts = port.read(10)
i=0
while ts == "":
	port.write(b"\x01")
	ts = port.read(20)
	#print "ts: ", ts

#print "cipher: ", ", ".join("{:02x}".format(ord(c)) for c in ts)

print "P + Q"

# předání P_x
port.write(b"\x10")
# ========================= unsigned char x_P[SIZE] = { 0x0F, 0xF1, 0x92, 0x27, 0xE4, 0x63, 0x7B, 0x12, 0xCB, 0x30 };
port.write(b"\x0F") 
port.write(b"\xF1")
port.write(b"\x92")
port.write(b"\x27")
port.write(b"\xE4")
port.write(b"\x63")
port.write(b"\x7B")
port.write(b"\x12")
port.write(b"\xCB")
port.write(b"\x30")
ts = port.read(10)
print "Px: ", ", ".join("{:02x}".format(ord(c)) for c in ts)

# předání P_y
port.write(b"\x11")
# ========================= unsigned char y_P[SIZE] = { 0x3B, 0x71, 0x4F, 0xB0, 0x6B, 0x26, 0x88, 0x2C, 0x7B, 0x54 };
port.write(b"\x3B") 
port.write(b"\x71")
port.write(b"\x4F")
port.write(b"\xB0")
port.write(b"\x6B")
port.write(b"\x26")
port.write(b"\x88")
port.write(b"\x2C")
port.write(b"\x7B")
port.write(b"\x54")
ts = port.read(10)
print "Py: ", ", ".join("{:02x}".format(ord(c)) for c in ts)

# předání Q_x
port.write(b"\x20")
# ========================= unsigned char x_Q[SIZE] = { 0x25, 0x73, 0x49, 0x14, 0x50, 0x03, 0x9F, 0x2A, 0x20, 0x00 };
port.write(b"\x25") 
port.write(b"\x73")
port.write(b"\x49")
port.write(b"\x14")
port.write(b"\x50")
port.write(b"\x03")
port.write(b"\x9F")
port.write(b"\x2A")
port.write(b"\x20")
port.write(b"\x00")
ts = port.read(10)
print "Qx: ", ", ".join("{:02x}".format(ord(c)) for c in ts)

# předání Q_y
port.write(b"\x21")
# ========================= unsigned char y_Q[SIZE] = { 0x29, 0xC1, 0x97, 0x2F, 0x55, 0x59, 0x48, 0xA6, 0x75, 0x51 };
port.write(b"\x29") 
port.write(b"\xC1")
port.write(b"\x97")
port.write(b"\x2F")
port.write(b"\x55")
port.write(b"\x59")
port.write(b"\x48")
port.write(b"\xA6")
port.write(b"\x75")
port.write(b"\x51")
ts = port.read(10)
print "Qy: ", ", ".join("{:02x}".format(ord(c)) for c in ts)

#
port.write(b"\x01")
ts = port.read(20)
#print "Recieved: ", ts
print "Rx: ", ", ".join("{:02x}".format(ord(c)) for c in ts[0:10])
print "Ry: ", ", ".join("{:02x}".format(ord(c)) for c in ts[10:20])

print "P + P"

# předání P_x
port.write(b"\x10")
# ========================= unsigned char x_P[SIZE] = { 0x0F, 0xF1, 0x92, 0x27, 0xE4, 0x63, 0x7B, 0x12, 0xCB, 0x30 };
port.write(b"\x0F") 
port.write(b"\xF1")
port.write(b"\x92")
port.write(b"\x27")
port.write(b"\xE4")
port.write(b"\x63")
port.write(b"\x7B")
port.write(b"\x12")
port.write(b"\xCB")
port.write(b"\x30")
ts = port.read(10)
print "Px: ", ", ".join("{:02x}".format(ord(c)) for c in ts)

# předání P_y
port.write(b"\x11")
# ========================= unsigned char y_P[SIZE] = { 0x3B, 0x71, 0x4F, 0xB0, 0x6B, 0x26, 0x88, 0x2C, 0x7B, 0x54 };
port.write(b"\x3B") 
port.write(b"\x71")
port.write(b"\x4F")
port.write(b"\xB0")
port.write(b"\x6B")
port.write(b"\x26")
port.write(b"\x88")
port.write(b"\x2C")
port.write(b"\x7B")
port.write(b"\x54")
ts = port.read(10)
print "Py: ", ", ".join("{:02x}".format(ord(c)) for c in ts)

# předání P_x
port.write(b"\x20")
# ========================= unsigned char x_P[SIZE] = { 0x0F, 0xF1, 0x92, 0x27, 0xE4, 0x63, 0x7B, 0x12, 0xCB, 0x30 };
port.write(b"\x0F") 
port.write(b"\xF1")
port.write(b"\x92")
port.write(b"\x27")
port.write(b"\xE4")
port.write(b"\x63")
port.write(b"\x7B")
port.write(b"\x12")
port.write(b"\xCB")
port.write(b"\x30")
ts = port.read(10)
print "Px: ", ", ".join("{:02x}".format(ord(c)) for c in ts)

# předání P_y
port.write(b"\x21")
# ========================= unsigned char y_P[SIZE] = { 0x3B, 0x71, 0x4F, 0xB0, 0x6B, 0x26, 0x88, 0x2C, 0x7B, 0x54 };
port.write(b"\x3B") 
port.write(b"\x71")
port.write(b"\x4F")
port.write(b"\xB0")
port.write(b"\x6B")
port.write(b"\x26")
port.write(b"\x88")
port.write(b"\x2C")
port.write(b"\x7B")
port.write(b"\x54")
ts = port.read(10)
print "Py: ", ", ".join("{:02x}".format(ord(c)) for c in ts)

port.write(b"\x01")
ts = port.read(20)
#print "Recieved: ", ts
print "Rx: ", ", ".join("{:02x}".format(ord(c)) for c in ts[0:10])
print "Ry: ", ", ".join("{:02x}".format(ord(c)) for c in ts[10:20])



 # ot
#"0123456701234567"
#print "ot: ", "".join("{:02x}".format(ord(c)) for c in "0123456701234567") # št
#print "ot: ", ", ".join("{:02x}".format(ord(c)) for c in "0123456701234567") # št
#port.write("0123456701234567") # ot
#out = port.read(16)
#print "cipher", out
#print "cipher: ", ", ".join("{:02x}".format(ord(c)) for c in out) # št
#port.write(b'd')
#port.write(bytes(out))
#out = port.read(16)
#print "open: ", out # ot

