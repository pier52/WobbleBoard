
import OSC

class Sound:
	def __init__(self):
		self.client = OSC.OSCClient()

	def connect(self,ip,port,address):
		self.client.connect((ip, port))
		self.address = address

	def disconnect(self):
		client.disconnect()

	def send(self,data):
		oscmsg = OSC.OSCMessage()
		oscmsg.setAddress(self.address)
		oscmsg.append(data)
		self.client.send(oscmsg)

	def sendAngles(self,angles):
		oscmsg = OSC.OSCMessage()
		oscmsg.setAddress(self.address)
		oscmsg.append(angles['roll'])
		oscmsg.append(angles['pitch'])
		oscmsg.append(angles['yaw'])
		self.client.send(oscmsg)		
