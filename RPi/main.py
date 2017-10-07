import time
import dataLogger
import imu

def main():
	data_logger = dataLogger.dataLogger()
	data_logger.openLog("log.csv")
	imu_ = imu.MPU9250()
	imu_.start()

	last_time = time.time()
	for x in range(0,10):
			time.sleep(1)
		#if (time.time()-last_time) > 1:
		# Log data at 10Hz
		        print imu_.angles	
                        data_logger.logAngles(imu_.angles)
			last_time = time.time()
	imu_.stop()
	data_logger.closeLog()
main()
