# Use a Windows Server Core image as the base
FROM mcr.microsoft.com/windows/servercore:ltsc2019

# Copy the built executable and required DLLs into the container
ADD https://aka.ms/vs/17/release/vc_redist.x64.exe vc_redist.x64.exe
RUN vc_redist.x64.exe /quiet /install

COPY entrypoint.bat ./app/
COPY Node.exe ./app/
COPY Node.pdb ./app/
COPY libcurl.dll ./app/
COPY zlib1.dll ./app/

# Set the working directory inside the container
WORKDIR /app

# Expose the required ports
EXPOSE 9050
EXPOSE 9051

# Add the application directory to the PATH
# ENV PATH="C:\\app;${PATH}"

# Set the default command to run the Node executable
# ENTRYPOINT ["/app/Node.exe"]
# CMD ["%NODE_IP%", "%NODE_PORT%"]