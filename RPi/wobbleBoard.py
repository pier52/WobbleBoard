import dataLogger
import imu
import time
import signal
import sys
import WebStreaming

class WobbleBoard:
	def __init__(self):
		self.IMU = imu.MPU9250()
		self.data_logger = dataLogger.dataLogger()
		self.logging = False
		signal.signal(signal.SIGINT, self.SIGINT)
		self.is_running = False
		self.web_streamer = WebStreaming.WebStreaming()

	def run(self,frequency):
		self.is_running = True
		self.IMU.start()
		try:
			print("Waiting for connection")
			self.web_streamer.initSocket()
			print("Got connection")
			self.web_streamer.check()
			print("Got message")
			self.web_streamer.sendHello()
			print "Sent hello"
		except:
			print  "Error"
			self.SIGINT(0,0)
			return
		self.SIGINT(0,0)
		return
		time.sleep(1)
		while self.is_running:
			time.sleep(1.0/frequency)
			print self.IMU.angles
			self.web_streamer.sendAngles(1,self.IMU.angles)
			if(self.logging):
				self.data_logger.logAngles(self.IMU.angles)

	def enableLogging(self, path):
		if(not(self.logging)):
			self.data_logger.openLog(path)
			self.logging = True

	def disableLogging(self):
		if(self.logging):
			self.logging = False
			self.data_logger.closeLog()

	def restartLogging(self,path):
		self.disableLogging()
		self.enableLogging(path)

	def SIGINT(self,signal, frame):
		if(self.is_running):
			self.is_running = False
			self.disableLogging()
			self.IMU.stop()
			self.web_streamer.closeAll()
