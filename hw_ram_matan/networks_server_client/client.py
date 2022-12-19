
#!/usr/bin/python
import socket
import sys


def main():
    host = str(sys.argv[1]) #type str
    print("host is : " + str(host))
    port_number=int(sys.argv[2]) #type int
    print("port number is : " +str(port_number))
    client_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_sock.connect((host,port_number))
    get_req = 'GET / HTTP/1.1\r\nHost: '+host+'\r\n\r\n'
    client_sock.send(get_req.encode())
    data = client_sock.recv(1024)
    print(data.decode())


    # HOST = 'www.google.com'    # The remote host
    # PORT = 10001             # The same port as used by the server


    # ip=socket.gethostbyname('www.google.com')
    # print ip
    # host = sys.argv[1] #type str
    # print("host is : " + str(host))
    # port_number=int(sys.argv[2]) #type int
    # print("port number is : " +str(port_number))
    # s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # s.connect((socket.gethostbyname(host),PORT))
    # s.send(bytes('GET / HTTP/1.1\r\nHost: '+host+'\r\n\r\n',"utf-8"))
    # data = str(s.recv(1024),"utf-8")
    # print("Received : {}" .format(data))
main()

# Echo client program
# import socket

# HOST = 'daring.cwi.nl'    # The remote host
# PORT = 50007              # The same port as used by the server
# s= socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# s.connect((HOST, PORT))
# s.sendall(b'Hello, world')
# data = s.recv(1024)
# print('Received', repr(data))