import sys
import yaml

def update_docker_compose(services, base_port=9050):
    # Load Docker Compose file
    compose_file = "C:/Users/Magshimim/orbit/src/dockerFiles/Docker-compose.yaml"
    
    # Read the YAML file
    with open(compose_file, "r") as file:
        compose_data = yaml.safe_load(file)
    
    # Update NODE_PORT for specified services
    for service in services:
        if service in compose_data.get("services", {}):
            # Get current NODE_PORT
            original_port = compose_data["services"][service]["environment"]["NODE_PORT"]
            
            # Update NODE_PORT
            compose_data["services"][service]["environment"]["NODE_PORT"] = base_port
            print(f"Updated {service}: NODE_PORT from {original_port} to {base_port}")
        else:
            print(f"Service {service} not found in Docker-compose.yaml. Skipping...")
    
    # Write the updated data back to the YAML file
    with open(compose_file, "w") as file:
        yaml.dump(compose_data, file, default_flow_style=False)
    
    print("Docker-compose.yaml updated successfully!")

def main():
    # Check if services are passed as arguments
    if len(sys.argv) < 2:
        print("Usage: adjust_ports_to_talk_in_subnet.py <service1> [<service2> ...]")
        sys.exit(1)

    # Get services from command-line arguments
    services = sys.argv[1:]

    # Update the compose file
    update_docker_compose(services)
    
if __name__ == "__main__": 
    main()
