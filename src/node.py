import socket
from global_consts import *
import sys

class Node:
    def __init__(self, ip: str, port: int) -> None:
        self.routing_table = {}
        self.__addr = (ip, port)

    def run(self) -> None:
        try:
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as self.socket:
                print(0.4)
                self.socket.bind(self.__addr)
                print(0.5)
                self.socket.listen()
                print(0.6)
                client_socket, client_addr = self.socket.accept()
                print(1)

                with client_socket:
                    print(f"Connected by {client_addr}")
                    data = client_socket.recv(1024)
                    print(2)

                    data = data.decode()
                    print(data)
                    self.routing_table['next'] = (data.split(',')[0], data.split(',')[1])
                    print("liam")

                    msg = "ok next updated"
                    client_socket.sendall(msg.encode())

                    while True:
                        print("while fr")

                        data = client_socket.recv(1024)
                        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as next_socket:
                            next_socket.connect(self.routing_table['next'])
                            next_socket.sendall(data)
                            data = next_socket.recv(1024)
                            client_socket.sendall(data)
                            print("new sock")
        except BaseException as be:
            print(be)
            print("node crush!")

def main() -> None:
    print("Running node...")
        
    if len(sys.argv) != 3:
        print("DEFAULT")
        node = Node('localhost', 8550)
    else:
        print("Using ARGV")
        node = Node(sys.argv[1], int(sys.argv[2]))
    
    
    node.run()

if __name__ == "__main__":
    main()
