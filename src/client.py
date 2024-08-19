import socket
import json
from global_consts import *

class Client:
    def __init__(self, ip: str, port: int) -> None:
        pass

def start_client():
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:

            server_address = ('localhost', 8770)
            client_socket.connect(server_address)

            url = input("Enter url: ")
            nodes = input("Enter amount of nodes to open: ")
            path_nodes = input("Enter amount of nodes for the path of the message: ")
            
            if not nodes.isnumeric() or not path_nodes.isnumeric():
                print("Bad input")
                client_socket.close()
                exit(1)
                
            message = {
                URL : url,
                NODES : int(nodes),
                PATH_NODES : int(path_nodes), 
            }
            
            print(f"Sending: {message}")
            serialized_message = json.dumps(message).encode()
            client_socket.sendall(serialized_message)
            
            data = client_socket.recv(AMOUNT_OF_BYTES)
            print(f"Received: {data.decode()}")
            
            node_data = json.loads(client_socket.recv(AMOUNT_OF_BYTES).decode())
            print(node_data)
                

        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_to_first_node_socket:
            print("Client opened new socket to talk with the node")
            client_to_first_node_socket.connect((node_data[NODE_IP], node_data[NODE_PORT]))
            client_to_first_node_socket.sendall(b"localhost,8551")
    except BaseException as be:
        print(be, "Client crash")
        

def main() -> None:
    print("Running client...")
    client = start_client()
    

if __name__ == "__main__":
    main()
