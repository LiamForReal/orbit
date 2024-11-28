# Use a Windows Server Core image as the base
FROM mcr.microsoft.com/windows/servercore:ltsc2019

# Set the working directory inside the container
WORKDIR C:\app

# Copy the built executable and required DLLs into the container
COPY ../Node/x64/Release/Node.exe C:\app\
COPY ../Node/x64/Release/libcurl.dll C:\app\
COPY ../Node/x64/Release/zlib1.dll C:\app\

# Install dependencies (if needed, like Visual C++ Redistributable)
RUN curl -L -o vc_redist.x64.exe https://aka.ms/vs/16/release/vc_redist.x64.exe && \
    vc_redist.x64.exe /quiet /install && \
    del vc_redist.x64.exe

# Expose the required ports
EXPOSE 9050 9051

# Add to PATH
ENV PATH="C:\app;${PATH}"

CMD ["echo", "/c", "hello"]
CMD ["C:\\app\\Node.exe"]

