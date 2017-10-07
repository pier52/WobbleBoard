

class WebStreaming:
    def __init__(self):
        self.TCP_IP = self.get_ip_address('wlan0')
        print self.TCP_IP
        self.connected = False
        self.BUFFER_SIZE = 1024
        self.TCP_PORT = 5000

    def get_ip_address(self, ifname):
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        return socket.inet_ntoa(fcntl.ioctl(
            s.fileno(),
            0x8915,  # SIOCGIFADDR
            struct.pack('256s', ifname[:15])
        )[20:24])

    def initSocket(self):
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.s.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 60000000)
        self.s.setblocking(False) #Make sure socket is not blocking
        self.s.bind((self.TCP_IP, self.TCP_PORT))
        self.s.listen(1)
        self.conn, self.addr = self.s.accept()
        print 'Connection address:', self.addr
        self.connected = True

    def closeAll(self):
        self.connected = False
        self.conn.close()
        self.s.shutdown(socket.SHUT_RDWR)
        self.s.close()

    def sendAngles(self,time,angles):
    	self.conn.send(str(time)+ "," + str(angles["roll"]) + "," + str(angles["pitch"]) + "," + str(angles["yaw"]))

    def handle(self, json_data):
        data = json.loads(json_data)
        msg = data['msg']
        if msg == 'Hello':  # Handshake
            return '$' + msg + '\n'
        elif msg == 'start_rssi':
            self.send_signal_level = True
            self.sendSignalLevel()
        elif msg == 'stop_rssi':
            self.send_signal_level = False
        else:
            return '$unknown_message'
        return '$' + msg + '\n'

    def check(self):
        try:
            data = self.conn.recv(self.BUFFER_SIZE)
            if not data:
                return True
        except Exception:
            return False
        print "received data:", data
        #self.conn.send(self.handle(data))
        return True