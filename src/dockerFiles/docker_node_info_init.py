import random
import ipaddress
import yaml
import socket

# Define the network range
subnet = ipaddress.IPv4Network("192.168.1.0/24")

# Generate unique random IPs
def generate_random_ip(subnet, count):
    used_ips = set()
    ips = []
    for _ in range(count):
        while True:
            # Generate a random IP in the subnet
            ip = str(subnet.network_address + random.randint(1, subnet.num_addresses - 2))
            if ip not in used_ips:
                used_ips.add(ip)
                ips.append(ip)
                break
    return ips

# Generate unique random ports
def generate_random_port(count):
    used_ports = set()
    ports = []
    for _ in range(count):
        while True:
            # Generate a random port between 9050 and 9100
            port = random.randint(9050, 9100)
            if port not in used_ports:
                used_ports.add(port)
                ports.append(port)
                break
    return ports

def main():
    with open("../dockerFiles/Docker-compose.yaml", "r") as file:
        compose_data = yaml.safe_load(file)

    num_services = len(compose_data["services"])

    random_ips = generate_random_ip(subnet, num_services)
    random_ports = generate_random_port(num_services * 2)  # Two ports per service

    # Assign IPs
    for i, (service, ip) in enumerate(zip(compose_data["services"], random_ips)):
        compose_data["services"][service]["networks"]["TOR_NETWORK"]["ipv4_address"] = ip
        compose_data["services"][service]["environment"]["NODE_IP"] = ip

    # Assign Ports
    port_index = 0
    for service in compose_data["services"]:
        updated_ports = []
        compose_data["services"][service]["environment"]["NODE_PORT"] = random_ports[port_index]
        for port_mapping in compose_data["services"][service]["ports"]:
            container_port = port_mapping.split(":")[1]  # Extract container port
            host_port = random_ports[port_index]  # Assign a random host port
            updated_ports.append(f"{host_port}:{container_port}")
            port_index += 1
        compose_data["services"][service]["ports"] = updated_ports

    # Write back to the Docker-compose file
    with open("../dockerFiles/Docker-compose.yaml", "w") as file:
        yaml.dump(compose_data, file)
    
    content = ""
    # with open("../dockerFiles/Docker-compose.yaml", "r") as file:
    #     content = file.read()
    # with open("../dockerFiles/Docker-compose.yaml", "w") as file:
    #     hostname = socket.gethostname()
    #     ip_address = '"' + socket.gethostbyname(hostname) + '"'
    #     content = content.replace('command:', f"command: -server -advertise {ip_address} -bootstrap")
    #     file.write(content)

    print("Assigned random IPs:", random_ips)

if __name__ == "__main__":
    main()
