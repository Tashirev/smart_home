import socket

UDP_IP = "192.168.0.200"
UDP_PORT = 8080
MESSAGE = b"boiler 30.2 30.3"  # над телевизором: ночь 26.7 - 26.9
print("UDP target IP: %s" % UDP_IP)
print("UDP target port: %s" % UDP_PORT)
print("message: %s" % MESSAGE)
sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))