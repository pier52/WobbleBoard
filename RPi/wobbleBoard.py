import dataLogger
import imu
import time
import signal
import sys

class WobbleBoard:
	def __init__(self):
		self.IMU = imu.MPU9250()
		self.data_logger = dataLogger.dataLogger()
		self.logging = False
		signal.signal(signal.SIGINT, self.SIGINT)
		self.is_running = False

	def run(self,frequency):
		self.is_running = True
		imu_.start()
			while self.is_running:
				time.sleep(1.0/frequency)

				if(self.logging):
					self.dataLogger.logAngles(self.IMU.angles)

	def enableLogging(self, path):
		if(not(self.logging)):
			data_logger.openLog(path)
			self.logging = True

	def disableLogging(self):
		if(self.logging):
			self.logging = False
			data_logger.closeLog()

	def restartLogging(self,path):
		self.disableLogging()
		self.enableLogging(path)

	def SIGINT(self):
		if(self.is_running):
			self.is_running = False
			self.disableLogging()
			self.IMU.stop()
