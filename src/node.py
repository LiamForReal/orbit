import socket
import json
from global_consts import *
import sys
import requests

class Node:
    def __init__(self, ip: str, port: int) -> None:
        self.next = ()
        self.__addr = (ip, port)

    def run(self) -> None:
        try:
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as self.socket:
                print(1)
                print(self.__addr)
                self.socket.bind(self.__addr)
                self.socket.listen()
                print(2)
                prev_socket, prev_addr = self.socket.accept()
                print(3)
                print(self.__addr, "is running...")

                if len(self.next) == 0:
                    print("HERE")
                    with prev_socket:
                        print(f"Connected by {prev_addr}")
                        data = json.loads(prev_socket.recv(1024).decode())
                        print("Received:", data)
                        self.next = data

                        msg = f"ok next updated"
                        prev_socket.sendall(msg.encode())
                    print("FINISHED HERE!")
                else: # making 'proxy' between prev_socket and next_socket
                    print("ELSE HERE")
                    while True:
                        data = prev_socket.recv(1024)
                        print(data)
                        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as next_socket:
                            next_socket.connect(self.next)
                            next_socket.sendall(data)
                            data = next_socket.recv(1024)
                            print(data)
                            prev_socket.sendall(data)
                        print("FINISHED ELSE HERE!")
                    
        except BaseException as be:
            print(be)
            print("node crush!")

def main() -> None:
    print("Running node...")
        
    if len(sys.argv) != 3:
        #print("DEFAULT")
        node = Node('localhost', 8550)
    else:
        #print("Using ARGV")
        node = Node(sys.argv[1], int(sys.argv[2]))
    
    
    node.run()

if __name__ == "__main__":
    main()