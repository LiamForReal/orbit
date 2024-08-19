import dpkt
import socket

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
                    data = client_socket.recv(1024)
                    data = data.decode()
                    if not data:
                        break
                    else:
                        nodes_data = data.split(',')
                        nodes = int(nodes_data[0])
                        path_nodes = int(nodes_data[1])
                        
                    client_socket.sendall(f"Opening {nodes} nodes, selected path length: {path_nodes}".encode())
                
    

def main() -> None:
    print("Running server...")
    server = Server('localhost', 8770)
    server.run()
    

if __name__ == "__main__":
    main()
