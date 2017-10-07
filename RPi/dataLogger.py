import csv
import time

class dataLogger:
	def __init__(self):
		self.path = ""

	def openLog(path):
		self.path = path
		log_file = open(path,'wb')
		self.open_time = time.time()
		self.writer = csv.writer(csvfile, delimiter=' ',
                            quotechar='|', quoting=csv.QUOTE_MINIMAL)

	def logAngles(self,angles):
    		self.writer.writerow([time.time()-self.open_time(),angles['roll'], angles['pitch'], angles['yaw']])