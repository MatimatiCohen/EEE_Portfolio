#!/usr/bin/python2.7 -tt
from socket import *
import sys
import time


BUFFER_INIT = 1024

def get_port_number():
    return int(sys.argv[1])

def recv_msg(s):
    message = ""

    while True:
        message += s.recv(BUFFER_INIT)
        if "\r\n\r\n" in message:
            return message

def send_error_msg(s):
    error_msg = \
    """HTTP/1.1 404 Not Found\r
    Content-type: text/html\r
    Content-length: 113\r
    \r
    <html><head><title>Not Found</title></head><body>\r
    Sorry, the object you requested was not found.\r
    </body></html>\r\n\r\n\r\n\r\n"""
    s.send(error_msg)

def send_counter_msg(s, counter):
    counter_msg = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n"
    counter_msg += "Content-Length: "+str(len(str(counter)))+"\r\n\r\n"
    counter_msg += str(counter)+"\r\n\r\n"
    s.send(counter_msg)

def main():
    port_number = get_port_number()
    address_counter = 0
    server_socket = socket()
    server_socket.connect(('localhost',port_number))

    while True:
        time.sleep(0.1)
        browser_request = recv_msg(server_socket)
        first_line = browser_request.split("\r\n")[0]

        if "GET" not in first_line:
            send_error_msg(server_socket)
            continue

        address = first_line.split("GET")[1].split()[0]
        if address == "/counter":
            address_counter += 1
            send_counter_msg(server_socket, address_counter)
        else:
            send_error_msg(server_socket)

main()
