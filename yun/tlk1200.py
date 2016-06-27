#!/usr/bin/python

#
# tlk1200	Trackball on Logiteck K1200
#
###############################################################
#	On Arduino
#	works with tty2kbdandmouse
###############################################################

#import sys, select, tty, termios
import struct
import os
import sys
import time
import serial
from evdev import InputDevice
from select import select



###############################################################
# Format
# key 1    key down
# key 0    key up
###############################################################
# 7f	Right Win

os.system("/usr/bin/reset-mcu")

#isCtlDown = 0
isLWinDown = 0

asciiTable = [0 * 256]
asciiTable =    [  0, 		# 00
		0xb1 , 
		0x31,		# 02  
		0x32, 
		0x33, 
		0x34, 
		0x35, 
		0x36, 
		0x37, 
		0x38, 
		0x39, 
		0x30,  
		0x2d,  		# 0c	=
		0x3d,		# 0d 	-
		0xb2, 		# 14 backspace 
		0xb3,		# 15 tab
		
		#10	
		0x71, 
		0x77, 
		0x65, 
		0x72, 
		0x74, 
		0x79, 
		0x75, 
		0x69, 
		0x6f, 
		0x70, 
		0x5b,  
		0x5d, 
		0x0a,  
		0x80,		# 1d left ctrl 
		0x61, 
		0x73,
		
		# 20		
		0x64, 
		0x66, 
		0x67, 
		0x68, 
		0x6a, 
		0x6b, 
		0x6c, 
		0x3b,
		0x27, 
		0x60, 
		0x81,  
		0x5c, 
		0x7a, 
		0x78, 
		0x63, 
		0x76,
		
		# 30		
		0x62, 
		0x6e, 
		0x6d, 
		0x2c, 
		0x2e,
		0x2f,   
		0x85,   		#36 Right shift
		7,   
		0x82, 		# 38 Left Alt
		0x20, 
		10,  
		0xc2,  		# 3b		F1
		0xc3,  
		0xc4,  
		0xc5,  
		0xc6,		# 3f		F5
		
		# 40
		0xc7,   		# 40		F6
		0xc8,   
		0xc9,   
		0xca,   
		0xcb,   		# 44		F10
		5,   
		6,   
		7,   
		8,   
		9, 
		10,  
		11,  
		12,  
		13,  
		14,  
		15,
		
		# 50
		50,   
		1,   
		2,   
		3,   
		4,   
		5,   
		0x5c,   	# 56	back slash
		0xce,  		# 57	F11 ==>> Print screen
		0xd4,   	# 58	F12 ==>> Delete
		9, 
		10,  
		11,  
		12,  
		13,  
		14,  
		15,
		
		# 60
		60,   
		0x84,   # 61	Right Control
		2,   
		0xce,	# 63	Print Screen   usb hid print screen is 0x46. 0x46 + 0x88 = 0xce
		4,   
		5,   
		0xd2,   # 66 	Home
		0xda,   # 67	Up
		0xd3, 	# 68	PgUp  
		0xd8,	# 69	Left 
		0xd7,  	# 6a	Right
		0xd5, 	# 6b	End 
		0xd9,  	# 6c	Down
		0xd6,  	# 6d	PgDown
		14,  
		0xD4,	# 6f	Del
		
		# 70
		70,   
		1,   
		2,   
		3,   
		4,   
		5,   
		6,   
		7,   
		8,   
		9, 
		10,  
		11,  
		12,  
		0x83,  		# 7d left Win
		0x87,  		# 7e	Right Win
		0xed		# 7f 	Win App
		]
		
altTable = [0 for i in range(256)]

altTable[0x67] = 0xd3  	# arrow up is page up
altTable[0x6c] = 0xd6 	# arrow up is page down
altTable[0x69] = 0xd2 	# arrow up is home
altTable[0x6a] = 0xd5 	# arrow up is  end	
altTable[0x3b] = 0xcc 	# Alt F1 ==>> F11
altTable[0x3c] = 0xcd 	# Alt F2 ==>> F12


##############################################################

s=serial.Serial("/dev/ttyATH0",115200,timeout=1)



#long int, long int, unsigned short, unsigned short, unsigned int
FORMAT = 'llHHI'
EVENT_SIZE = struct.calcsize(FORMAT)

devices = map(InputDevice, ('/dev/input/event1', '/dev/input/event2'))

for dev in devices: 
	print(dev)


while True:
	r, w, x = select(devices, [], [])
	for fd in r:
		for event in fd.read():
			print(event)
		
		
			code = event.code		
			type = event.type
			value = event.value		


			if (type == 1):
		
				#if ((code == 0x61) & (value > 0)): 	# Right Control				
					#print "Right Ctrl Down"
					#isCtlDown = 1			
				#elif ((code == 0x61) & (value == 0)): 	# Right Control				
				#	print "Right Ctrl Up"
					#isCtlDown = 0			
					
				if ((code == 0x7d) & (value > 0)): 	# Right Control				
					print "Left Win Down"
					isLWinDown = 1			
				elif ((code == 0x7d) & (value == 0)): 	# Right Control				
					print "Left Win Up"
					isLWinDown = 0			
					
					
					
				if code in [		# Skip the keypad ?
					0x6e, 
					0x63, 
					0x46, 
					0x77, 
					0x6e, 
					0x63, 
					0x46, 
					0x77,
					0x45,
					0x62,
					0x4a, 
					0x47, 
					0x48,
					0x49,
					0x4e,
					0x4d,
					0x4c,
					0x4b,
					0x4f,
					0x37,
					0x50,
					0x51,
					0x60,
					0x53,
					0x52
					]:
			
		
					print(str(format(code, '#02x')))
					print "Skipping"
					
				# Caps lock handling - left mouse click
				elif(code == 0x3a):
					print("Caps Lock - left click")
					msg = "1 0 " + str(value) + "\n"
					print(msg)
					s.write(msg)
						
				# Right "\" is right click
				elif(code == 0x56):
					print("Right Gui - right mouse click")
					msg = "1 1 " + str(value) + "\n"
					print(msg)
					s.write(msg)
		
				
				
							# General case for keyboard				
				elif (code < 256):	# sometimes receives 0x110 and would fail
					print("isLWinDown is " + str(isLWinDown))
					#if ((isCtlDown == 1) & (altTable[code] != 0)):
					if ((isLWinDown == 1) & (altTable[code] != 0)):
						print("Using alt table")	
						print(str(format(code, '#02x')))
						msg = "0 " + str(format(altTable[code], '#02x')) + " " + str(value) + "\n"
						print(msg)
						s.write(msg)
					else:
						print("Using standard table")	
						print(str(format(code, '#02x')))
						msg = "0 " + str(format(asciiTable[code], '#02x')) + " " + str(value) + "\n"
						print(msg)
						s.write(msg)
			
			elif(type == 2):
				msg = "2 " + str(code) + " " + str(value) + "\n"
				print(msg)
				s.write(msg)
	
	
#	event = in_file.read(EVENT_SIZE)

in_file.close()
s.close()



#result=s.read(5)
#print result


