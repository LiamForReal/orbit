# Use a Windows Server Core image as the base
FROM mcr.microsoft.com/windows/servercore:ltsc2019

# Set the working directory inside the container
WORKDIR C:\\app

# Copy the built executable and required DLLs into the container
COPY ../Node/x64/Release/Node.exe C:\\app\\
COPY ../Node/x64/Release/libcurl.dll C:\\app\\
COPY ../Node/x64/Release/zlib1.dll C:\\app\\

# Expose the required ports to the inside docker network
EXPOSE 9050 9091

ENV PATH="C:\\app;${PATH}"

# Command to run your app (replace with your actual app's executable name)
ENTRYPOINT ["Node.exe"]
