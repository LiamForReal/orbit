import socket
import json
from global_consts import *

class Client:
    def __init__(self, ip: str, port: int) -> None:
        pass

def start_client():
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    server_address = ('localhost', 8770)
    client_socket.connect(server_address)

    try:
        url = input("Enter url: ")
        nodes = input("Enter amount of nodes to open: ")
        path_nodes = input("Enter amount of nodes for the path of the message: ")
        
        if not nodes.isnumeric() or not path_nodes.isnumeric():
            print("Bad input")
            client_socket.close()
            exit(1)

        message = {
            URL : url,
            NODES : nodes,
            PATH_NODES : path_nodes, 
        }
        
        print(f"Sending: {message}")
        serialized_message = json.dumps(message).encode()
        client_socket.sendall(serialized_message)

        data = client_socket.recv(AMOUNT_OF_BYTES)
        print(f"Received: {data.decode()}")
    finally:
        client_socket.close()

def main() -> None:
    print("Running client...")
    client = start_client()
    

if __name__ == "__main__":
    main()
