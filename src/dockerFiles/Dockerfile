# Use a Windows Server Core image as the base
FROM mcr.microsoft.com/windows/servercore:ltsc2022

# Set the working directory inside the container
WORKDIR /app

# Copy the built executable (you can use other COPY commands for other dependencies)
COPY ../Node/x64/Debug/Node.exe /app/

# Expose the required ports (example: port 8080)
EXPOSE 8080

# Command to run your app (replace with your actual app's executable name)
ENTRYPOINT ["Node.exe"]