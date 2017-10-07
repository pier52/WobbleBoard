import csv
import time

class dataLogger:
	def __init__(self):
		self.path = ""

	def openLog(self,path):
		self.path = path
		self.log_file = open(path,'wb')
		self.open_time = time.time()
		self.writer = csv.writer(self.log_file, delimiter=',',
                            quotechar='|', quoting=csv.QUOTE_MINIMAL)

	def logAngles(self,angles):
    		self.writer.writerow([time.time()-self.open_time,angles['roll'], angles['pitch'], angles['yaw']])

	def closeLog(self):
		self.log_file.close()
