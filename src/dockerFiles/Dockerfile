# Use a Windows Server Core image as the base
FROM mcr.microsoft.com/windows/servercore:ltsc2019

# Set the working directory inside the container
WORKDIR C:\\app

# Copy the built executable and required DLLs into the container
COPY Node.exe .
COPY libcurl.dll .
COPY zlib1.dll .

# Expose the required ports
EXPOSE 9050
EXPOSE 9051

# Add the application directory to the PATH
ENV PATH="C:\\app;${PATH}"

# Optional: Check environment variables (commented for production)
RUN echo %NODE_IP% && echo %NODE_PORT%

# Set the default command to run the Node executable
CMD ["echo", "hello world"]
# CMD ["C:\\app\\Node.exe %NODE_IP% %NODE_PORT%"]
