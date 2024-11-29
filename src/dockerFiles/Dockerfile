# Use a Windows Server Core image as the base
FROM mcr.microsoft.com/windows/servercore:ltsc2019

# Set the working directory inside the container
WORKDIR C:\app

# Copy the C++ source file and any necessary DLLs
COPY ../Node/x64/Release/Node.exe C:\app\
COPY ../Node/x64/Release/libcurl.dll C:\app\
COPY ../Node/x64/Release/zlib1.dll C:\app\
COPY ../dockerFiles/src.cpp C:\app\

# Install MinGW (Minimalist GNU for Windows) to get GCC
RUN curl -L -o mingw.exe https://sourceforge.net/projects/mingw/files/latest/download && \
    start /wait mingw.exe /silent && \
    del mingw.exe

# Add MinGW's bin folder to PATH
ENV PATH="C:\\MinGW\\bin;${PATH}"

# Compile the C++ source file with GCC
RUN gcc C:\app\src.cpp -o C:\app\src.exe

# Expose the required ports
EXPOSE 9050
EXPOSE 9051

# Set the default command to run the compiled executable
CMD ["C:\\app\\src.exe"]