import os
import dpkt
import socket
import json
from global_consts import *
from node import Node
import subprocess
import sys
import random

class Server:
    def __init__(self, ip: str, port: int) -> None:
       self.__addr = (ip, port)
    
    
    def run(self) -> None:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as self.socket:
            self.socket.bind(self.__addr)
            self.socket.listen()
            
            client_socket, client_addr = self.socket.accept()
            
            with client_socket:
                print(f"Connected by {client_addr}")
                while True:
                    try:
                        data = client_socket.recv(AMOUNT_OF_BYTES).decode()
                        
                        if not data:
                            break
                        
                        data = json.loads(data)
                        print(data)
                                
                        client_socket.sendall(f"Opening {data[NODES]} nodes, selected path length: {data[PATH_NODES]}".encode())
                        
                        nodes_data = []
                                                
                        for i in range(data[NODES]):
                            nodes_data.append(['localhost', FIRST_NODE_PORT+i])
                            subprocess.Popen(["python", "./node.py", 'localhost', str(FIRST_NODE_PORT+i)], shell = True)
                        
                        random.shuffle(nodes_data)
                        nodes_data = nodes_data[:data[PATH_NODES]]
                        print(nodes_data)
                        
                        serialized_node_data = json.dumps(nodes_data)
                        client_socket.sendall(serialized_node_data.encode())
                    
                        
                    except BaseException as be:
                        print(be)
    

def main() -> None:
    print("Running server...")
    server = Server('localhost', 8770)
    server.run()
    

if __name__ == "__main__":
    main()
