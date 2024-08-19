import socket

Routing_Table = {}

class Node:
    def __init__(self, ip: str, port: int) -> None:
        pass

    def run(self) -> None:
        try:
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as self.socket:
                self.socket.bind(self.__addr)
                self.socket.listen(1)
                client_socket, client_addr = self.socket.accept()

                with client_socket:
                    print(f"Connected by {client_addr}")
                    data = client_socket.recv(1024)
                    data = data.decode()
                    Routing_Table['next'] = (data.split(',')[0], data.split(',')[1])
                    msg = "ok next updated"
                    client_socket.sendall(msg.encode())
                    while True:
                        data = client_socket.recv(1024)
                        next_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                        next_socket.bind(Routing_Table['next'])
                        next_socket.sendall(data)
                        data = next_socket.recv(1024)
                        client_socket.sendall(data)
        except:
            print("node crush!")

def main() -> None:
    print("Running node...")
    node = Node('localhost', 8550)
    node.run()

if __name__ == "__main__":
    main()
