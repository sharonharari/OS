# !/usr/bin/python
import socket
import sys

def main():
    port_number = int(sys.argv[1])
    print("port number is : " + str(port_number))
    localhost = '127.0.0.1'
    server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_sock.bind((localhost, port_number))
    server_sock.listen(1)
    conn,addr = server_sock.accept()
    HTML_file = 'https://grades.cs.technion.ac.il/grades.cgi?dbfghiced9d01178b276eaa5882d94+2+236350+Spring2022+hw/WCFiles/example.html+56'
    with conn :
        print('connected by',addr)
        while True :
            data = conn.recv(1024)
            if not data : break
            #if data == 'GET / HTTP/1.1\r\nHost: '+host+ '\r\n\r\n':
            conn.send('HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n'+str(HTML_file))


main()
# # Echo server program
# import socket

    # HOST = ''                 # Symbolic name meaning all available interfaces
    # PORT = 50007              # Arbitrary non-privileged port
    # s= socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # s.bind((localhost, port_number))
    # s.listen(1)
    # conn, addr = s.accept()
    # with conn:
    #     print('Connected by', addr)
    #     while True:
    #         data = conn.recv(1024)
    #         if not data: break
    #         conn.sendall(data)