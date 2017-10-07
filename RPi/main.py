import time
import dataLogger
import imu

data_logger = dataLogger()
imu = MPU9250()
data_logger.openLog("log.csv")
imu.start()
last_time = time.time()
def main():
	while True:
		if (time.time()-last_time) > 0.1:
		# Log data at 10Hz
			data_logger.logAngles(imu.angles)
			last_time = time.time()

