rmdir "build" /S /Q
mkdir "build"
cmake -G "Unix Makefiles" -S . -B build/
Copy Docker-compose.yaml build/Docker-compose.yaml
Copy docker_ip_inishializer.py build/docker_ip_inishializer.py
cd build/
make
pause