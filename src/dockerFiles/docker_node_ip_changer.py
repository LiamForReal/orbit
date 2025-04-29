import random
import ipaddress
import yaml
import sys
from pathlib import Path

# Define the network range
subnet = ipaddress.IPv4Network("192.168.2.0/24")

# Generate unique random IPs
def generate_random_ip(subnet, count):
    used_ips = set()
    ips = []
    for _ in range(count):
        while True:
            ip = str(subnet.network_address + random.randint(1, subnet.num_addresses - 2))
            if ip not in used_ips:
                used_ips.add(ip)
                ips.append(ip)
                break
    return ips

def main():
    if len(sys.argv) < 2:
        print("Usage: python script.py <service1> <service2> ...")
        sys.exit(1)

    services_to_update = sys.argv[1:]

    with open(f"{Path(__file__).parent}/Docker-compose.yaml", "r") as file:
        compose_data = yaml.safe_load(file)

    # Filter services present in the Docker Compose file
    services = [service for service in services_to_update if service in compose_data["services"]]

    if not services:
        print("No matching services found in the Docker Compose file.")
        sys.exit(1)

    random_ips = generate_random_ip(subnet, len(services))

    for service, ip in zip(services, random_ips):
        compose_data["services"][service]["networks"]["TOR_NETWORK"]["ipv4_address"] = ip
        compose_data["services"][service]["environment"]["NODE_IP"] = ip

    with open(f"{Path(__file__).parent}/Docker-compose.yaml", "w") as file:
        yaml.dump(compose_data, file)

    print("Assigned random IPs:", random_ips)

if __name__ == "__main__":
    main()
