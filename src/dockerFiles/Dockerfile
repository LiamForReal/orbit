# Using Nanoserver image
FROM mcr.microsoft.com/windows/nanoserver:ltsc2019

# Copy runtime DLLs of vctools
COPY msvcp140.dll ./app/
COPY vcruntime140.dll ./app/
COPY vcruntime140_1.dll ./app/
COPY ucrtbase.dll ./app/

# Copy Node files and DLLs
COPY entrypoint.bat ./app/
COPY entrypoint_first.bat ./app/
COPY Node.exe ./app/
COPY Node.pdb ./app/
COPY libcurl.dll ./app/
COPY zlib1.dll ./app/

# Set the working directory inside the container
WORKDIR /app

# Expose the required ports
EXPOSE 9050
EXPOSE 9051

# Set the default command to run the Node executable
# ENTRYPOINT ["/app/Node.exe"]
