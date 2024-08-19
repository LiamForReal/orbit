import dpkt
import socket
import pickle
from global_consts import *

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
                        data = client_socket.recv(AMOUNT_OF_BYTES)
                        
                        if not data:
                            break
                        
                        data = pickle.loads(data)
                        print(data)
                                
                        client_socket.sendall(f"Url: {data[URL]}, Opening {data[NODES]} nodes, selected path length: {data[PATH_NODES]}".encode())
                    except BaseException as be:
                        print(be)
    

def main() -> None:
    print("Running server...")
    server = Server('localhost', 8770)
    server.run()
    

if __name__ == "__main__":
    main()
