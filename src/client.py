import socket

class Client:
    def __init__(self, ip: str, port: int) -> None:
        pass

def start_client():
    # Create a TCP/IP socket
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Connect the socket to the server's address and port
    server_address = ('localhost', 8770)  # Ensure this matches the server's address and port
    client_socket.connect(server_address)

    try:
        # Send data
        message = "Hello, Server!"
        print(f"Sending: {message}")
        client_socket.sendall(message.encode())

        # Look for the response
        data = client_socket.recv(1024)  # Receive 1024 bytes of data
        print(f"Received: {data.decode()}")
    finally:
        # Close the socket
        client_socket.close()

def main() -> None:
    print("Running client...")
    client = start_client()
    

if __name__ == "__main__":
    main()
