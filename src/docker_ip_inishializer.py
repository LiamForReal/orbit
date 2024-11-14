import random
import ipaddress
import yaml

# Define the network range
subnet = ipaddress.IPv4Network("192.0.0.0/8")

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

def main():
    with open("Docker-compose.yaml", "r") as file:
        compose_data = yaml.safe_load(file)

    num_services = len(compose_data["services"])

    random_ips = generate_random_ip(subnet, num_services)

    for i, (service, ip) in enumerate(zip(compose_data["services"], random_ips)):
        compose_data["services"][service]["networks"]["TOR_NETWORK"]["ipv4_address"] = ip

    with open("docker-compose.yaml", "w") as file:
        yaml.dump(compose_data, file)

    print("Assigned random IPs:", random_ips)
    
if __name__ == "__main__": 
    main()