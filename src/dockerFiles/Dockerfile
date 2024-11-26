# Use a Windows Server Core image as the base
FROM mcr.microsoft.com/windows/servercore:ltsc2022

# Set the working directory inside the container
WORKDIR /app

# Copy the built executable (you can use other COPY commands for other dependencies)
COPY ../Node/x64/Debug/Node.exe /app/

# Expose the required ports to the inside docker network 9050 
EXPOSE 9050 9091

# Command to run your app (replace with your actual app's executable name)
ENTRYPOINT ["Node.exe"]