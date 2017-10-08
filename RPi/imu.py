import sys, getopt

sys.path.append('.')
import RTIMU
import os.path
import time
import math
import threading

SETTINGS_FILE = "RTIMULib"

class MPU9250:
  def __init__(self):
    self.run = False
    self.imu_thread =  None
    self.angles = self.angles = {'roll': 0, 'pitch' : 0, 'yaw' : 0}


  def stop(self):
    if(self.run):
      self.run = False
      self.imu_thread.join()

  def start(self):
    self.run = True
    self.imu_thread = threading.Thread(target=self.run_imu)
    self.imu_thread.start()
  
  def run_imu(self):
    if not os.path.exists(SETTINGS_FILE + ".ini"):
      print("Settings file does not exist, will be created")

    s = RTIMU.Settings(SETTINGS_FILE)
    self.imu = RTIMU.RTIMU(s)

    if (not self.imu.IMUInit()):
        print("IMU Init Failed")
        sys.exit(1)
    else:
        print("IMU Init Succeeded")

    self.imu.setSlerpPower(0.02)
    self.imu.setGyroEnable(True)
    self.imu.setAccelEnable(True)
    self.imu.setCompassEnable(True)
    self.poll_interval = self.imu.IMUGetPollInterval()
    print("Recommended Poll Interval: %dmS\n" % self.poll_interval)

    while self.run:
      if self.imu.IMURead():
        data = self.imu.getIMUData()
        fusionPose = data["fusionPose"]
        #print("r: %f p: %f y: %f" % (math.degrees(fusionPose[0]),math.degrees(fusionPose[1]), math.degrees(fusionPose[2])))
        roll = math.degrees(fusionPose[0])
	pitch = math.degrees(fusionPose[1])
	yaw = math.degrees(fusionPose[2])

	if roll < 0:
		roll += 360

	if pitch < 0:
		pitch += 360

	if yaw < 0:
		yaw +=360

	self.angles['roll'] = roll
        self.angles['pitch'] = pitch
        self.angles['yaw'] = yaw

	time.sleep(self.poll_interval*1.0/1000.0)



#imu = MPU9250()
#imu.start()
#for x in range(0,10):
#	time.sleep(1)
#	print imu.angles

#imu.stop()

