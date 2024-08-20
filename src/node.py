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
                print(self.__addr)
                self.socket.bind(self.__addr)
                self.socket.listen()
                prev_socket, prev_addr = self.socket.accept()
                
                with prev_socket:
                    if len(self.next) == 0:
                        print(f"Node {self.__addr} connected by {prev_addr}\n")
                        data = json.loads(prev_socket.recv(1024).decode())
                        print(f"Node {self.__addr} Received:", data, "\n")
                        
                        msg = ""
                        if len(data) == 2:
                            self.next = (data[0], data[1])
                            msg = "ok next updated"
                        else:
                            web_server_response = requests.get(data[0])
                            msg = web_server_response.text
                            
                            print("Sends HTTPS request\n")
                            
                        prev_socket.sendall(msg.encode())
                        
                    # making 'proxy' between prev_socket and next_socket
                    if len(data) == 2:
                        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as next_socket:
                            next_socket.connect(self.next)
                            while True:
                                data = prev_socket.recv(1024)
                                print(f"Node {self.__addr} Received:", data, "\n")
                                next_socket.sendall(data)
                                data = next_socket.recv(1024)
                                print(f"Node {self.__addr} Sends:", data, "\n")
                                prev_socket.sendall(data)                        
                    
        except BaseException as be:
            print(be)
            print("node crush!")

def main() -> None:        
    if len(sys.argv) != 3:
        #print("DEFAULT")
        node = Node('localhost', 8550)
        print("Node ('127.0.0.1', 8550) is running...")
    else:
        #print("Using ARGV")
        node = Node(sys.argv[1], int(sys.argv[2]))
        print(f"Node ({sys.argv[1]}, {sys.argv[2]}) is running...")

    
    node.run()

if __name__ == "__main__":
    main()