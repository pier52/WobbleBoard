#!/usr/bin/env python

import socket
import signal
import sys
import fcntl
import struct
import select

class WebStreaming:
    def __init__(self):
        self.TCP_IP = self.get_ip_address('wlan0')
        print self.TCP_IP
        signal.signal(signal.SIGINT, self.sigint_handler)
        self.connected = False
        self.BUFFER_SIZE = 1024
        self.TCP_PORT = 5000
        self.initSocket()
        self.__old_msg = ""
        self.__to_send = []
        

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
        self.s.bind((self.TCP_IP, self.TCP_PORT))
        self.s.listen(1)
        self.conn, self.addr = self.s.accept()
        print 'Connection address:', self.addr
        self.s.setblocking(False)
        self.s.settimeout(0)
        self.connected = True

    def sigint_handler(self, signal, frame):
        print('Aborting...')
        self.closeAll()
        sys.exit(0)

    def closeAll(self):
        self.connected = False
        self.conn.close()
        self.s.shutdown(socket.SHUT_RDWR)
        self.s.close()

    def string2int8(self,string):
        if len(string) != 1:
            raise AssertionError("In fun TCP::<string2uint16>: wrong length of input string. Is " + str(len(string)) + ", should be 1")
        conv = ord(string)
        if(conv > 127):
            conv -= 256
        return conv

    def string2float(self,string):
        if len(string) != 4:
            raise AssertionError("In fun TCP::<string2uint16>: wrong length of input string. Is " + str(len(string)) + ", should be 4")
        return struct.unpack('f', string)[0]

    def string2uint16(self,string):
        if len(string) != 2:
            raise AssertionError("In fun TCP::<string2uint16>: wrong length of input string. Is " + str(len(string)) + ", should be 2")
        return ord(string[0]) + 256*ord(string[1])

    def send(self,string):
        self.__to_send.append(string)

    def sendAngles(self,time,angles):
        self.send("*i*"  + str(int(angles['roll'])) + "|" + str(int(angles['pitch'])) + "|" + str(int(angles['yaw'])) + "\r\n")

    def handle(self, data):
        tag = data[1]
        msg = data[2:]
        if tag == 's':  #steering
            print self.string2int8(msg)
            self.steering.steer(self.string2int8(msg))
            return
        elif tag == 'm': #motor
            print self.string2int8(msg)
            self.motor.setThrottle(self.string2int8(msg))
            return
        elif tag == 'k': #stop
            print "stop"
            return
        elif tag == 'r': #replay
            print "replay"
            return
        elif tag == 'd': #settings
            kp = self.string2float(msg[0:4])
            ki = self.string2float(msg[4:8])
            kd = self.string2float(msg[8:12])
            period = self.string2uint16(msg[12:14])
            limit = ord(msg[14])
            frequency = ord(msg[15])
            #self.sample_timer.changeTimeout(1000/frequency)
            self.sample_sender.start(1000/frequency)
            sample_motor_power = ord(msg[16])
            sample_pid = ord(msg[17])
            sample_speed = ord(msg[18])
            sample_steering_angle = ord(msg[19])
            self.sample_sender.sampleSteering(sample_steering_angle)
            replay_length = ord(msg[20])
            replay_period = ord(msg[21])
            sample_rssi = ord(msg[22])
            print (kp, ki,kd,period,limit,frequency,sample_motor_power,sample_pid,sample_speed,sample_steering_angle,replay_length,replay_period,sample_rssi)
            self.sample_sender.sampleRSSI(sample_rssi)
            return   
        else:
            self.errors.set("In fun TCP::<handle>: unknown tag:" + tag)


    def check(self):
        readable, writeable, exceptional = select.select([self.conn],[],[],0)
        if(readable):
            data = self.conn.recv(self.BUFFER_SIZE)
            if not data:
                return False


            data_list = data.split('\r')
            if self.__old_msg != "":
                data_list[0] = self.__old_msg + data_list[0]
                self.__old_msg = ""

            if data_list[-1] != "":
                self.__old_msg = data_list[-1]

            del data_list[-1]

            if '' in data_list:
                empty_indexes = [i for i,x in enumerate(data_list) if x=='']
                num_deleted = 0
                if 0 in empty_indexes:
                    raise BufferError("Unexpected carriage return at start of message.")
                for index in empty_indexes:
                    data_list[index-1-num_deleted] += '\r'
                    del data_list[index-num_deleted]
                    num_deleted += 1

            #for stuff in data_list:
                #print "received data:", stuff
                #self.handle(stuff)

        if self.__to_send:
            for msg in self.__to_send:
                self.conn.send(msg)
            self.__to_send = []

        return True

