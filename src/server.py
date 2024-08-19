import os
import dpkt
import socket
import json
from global_consts import *
from node import Node
import subprocess
import sys

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
                                
                        client_socket.sendall(f"Url: {data[URL]}, Opening {data[NODES]} nodes, selected path length: {data[PATH_NODES]}".encode())
                        
                        node_data = {
                            NODE_IP: 'localhost',
                            NODE_PORT: 8550,
                        }
                        
                        #os.system(f"python node.py {node_data[NODE_IP]} {node_data[NODE_PORT]}")
                        
                        subprocess.Popen(["python", "./node.py", str(node_data[NODE_IP]), str(node_data[NODE_PORT])], shell = True)
                        
                        print("Node is running...")
                        
                        serialized_node_data = json.dumps(node_data)
                        client_socket.sendall(serialized_node_data.encode())
                    
                        
                    except BaseException as be:
                        print(be)
    

def main() -> None:
    print("Running server...")
    server = Server('localhost', 8770)
    server.run()
    

if __name__ == "__main__":
    main()
