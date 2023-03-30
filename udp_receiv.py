import socket
from sql_write import *
from datetime import datetime

UDP_IP = "192.168.0.175"
UDP_PORT = 8080
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))
# чтение последнего id в существующей базе данных, при отсутствии записей назначить id = 0
try:
    id_count = last_id() + 1
except TypeError:
    id_count = 0
print("start udp_receiv -> sql_write")

while True:

    data, addr = sock.recvfrom(1024)  # buffer size is 1024 bytes  получение данных по udp
    splt_data = data.decode().split(' ')  # парсинг данных
    time = datetime.now().replace(microsecond=0)   # локальное время на сереве
    sql_status = sql_write(id_count, time, splt_data)   # запись в БД
    id_count += 1   # увеличение id для следующей записи
    print(time, "__", sql_status, "__", data)