import serial # for serial connection to Arduino
import psutil # to retrieve computer information
import time   # to delay information

ser = serial.Serial()
#ser.baudrate = 9600
ser.baudrate = 115200
ser.port = 2 # corresponding to COM3 - CHANGE IT WITH YOUR COM
ser.open()


while True :
	
	#cast XX,X as XX
	cpupercent = '%(number)02d' % \
		{"language": "Python", "number": psutil.cpu_percent()}

	#force the cpupercent to not exceed 99
	if(cpupercent == '100'):
		cpupercent = '99'

	#cast XX,X as XX
	rampercent = '%(number)02d' % \
		{"language": "Python", "number": psutil.virtual_memory().percent}

	#force the cpupercent to not exceed 99
	if(rampercent == '100'):
		rampercent = '99'

	total = '1' + cpupercent + rampercent # concatenation of each information

	print total # print information in console
	ser.write(total) #sending to the serial port
	time.sleep(1) # waiting for 1 second
