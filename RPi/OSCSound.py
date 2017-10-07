
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

