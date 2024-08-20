import socket
import json
from global_consts import *
import time

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
                NODES : int(nodes),
                PATH_NODES : int(path_nodes), 
            }
            
            print(f"Sending: {message}")
            serialized_message = json.dumps(message).encode()
            client_socket.sendall(serialized_message)
            
            data = client_socket.recv(AMOUNT_OF_BYTES)
            print(f"Received: {data.decode()}")
            
            path_data = json.loads(client_socket.recv(AMOUNT_OF_BYTES).decode())
            print(path_data)
            
            time.sleep(2)
            
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_to_first_node_socket:
                print("Client opened new socket to talk with the first node")
                print((path_data[0][0], path_data[0][1]))
                client_to_first_node_socket.connect((path_data[0][0], path_data[0][1]))
                print("CONNECTED TO FIRST NODE!")
            
                for node_data in path_data[1:]:
                    print("Client Before Sending Data")
                    client_to_first_node_socket.sendall(json.dumps(node_data).encode())
                    time.sleep(1)
                    node_response = client_to_first_node_socket.recv(AMOUNT_OF_BYTES).decode()
                    print(node_response)
                
                client_to_first_node_socket.sendall(url.encode())
                html_response = client_to_first_node_socket.recv(AMOUNT_OF_BYTES).decode()
                print(html_response)

                
    except BaseException as be:
        print(be, "Client crash")
    
    # לולאה:
    # לפי המסלול להעביר לכל נואוד בהתאמה את הנקסט שלו 
    # כל נואוד שהעברנו אליו את הנקסט ישמור אותו וישלח הודעת קבלה שעל פיהם נוכל לדעת מתי הגענו לשרת ווב 
    # סוף לולאה   
    # כשהגענו לשרת ווב נשלח בקשה אליו

def main() -> None:
    print("Running client...")
    client = start_client()
    

if __name__ == "__main__":
    main()
