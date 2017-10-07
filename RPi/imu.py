

class MPU9250:
  def __init__(self):
    if not os.path.exists(SETTINGS_FILE + ".ini"):
      print("Settings file does not exist, will be created")

    s = RTIMU.Settings(SETTINGS_FILE)
    self.imu = RTIMU.RTIMU(s)

    if (not imu.IMUInit()):
        print("IMU Init Failed")
        sys.exit(1)
    else:
        print("IMU Init Succeeded")

    sellf.imu.setSlerpPower(0.02)
    self.imu.setGyroEnable(True)
    self.imu.setAccelEnable(True)
    self.imu.setCompassEnable(True)

  def getData(self):
  
    if imu.IMURead():
      data = imu.getIMUData()
      fusionPose = data["fusionPose"]
      print("r: %f p: %f y: %f" % (math.degrees(fusionPose[0]), 
        math.degrees(fusionPose[1]), math.degrees(fusionPose[2])))

      return {'roll': math.degrees(fusionPose[0]), 'pitch' : math.degrees(fusionPose[1]), 'yaw' : math.degrees(fusionPose[2])}



imu = MPU9250()
print(imu.getData())



