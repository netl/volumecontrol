import serial
import time
import alsaaudio

def drawscreen():
	ser.write(" ")
	ser.write(time.strftime("%H:%M"))
	ser.write("  vol:")
	vol = str(m.getvolume()).strip('[]L')
	ser.write(vol.ljust(3))
	ser.write("%\n")

tim = time.gmtime()
vol = 0
ser = serial.Serial('/dev/ttyACM0', 9600)
m = alsaaudio.Mixer('Master')
while True:
	if tim[5] != time.gmtime()[5]:
		tim = time.gmtime()
		drawscreen()
	if ser.inWaiting() > 0:
		data =  ser.read()
		vol = m.getvolume()[0]
		if(data == '2'):
			if(vol < 96):
				vol += 5
			else:
				vol = 100
			print "volume up"
		elif(data == '1'):
			if(vol > 4):
				vol -= 5
			else:
				vol = 0
			print "volume down"
		m.setvolume(vol)
		drawscreen()
