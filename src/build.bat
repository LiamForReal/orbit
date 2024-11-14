rmdir "build" /S /Q
mkdir "build"
cmake -G "Unix Makefiles" -S . -B build/
Copy docker_ip_inishializer.py build/docker_ip_inishializer.py
Copy Docker-compose.yaml build/Docker-compose.yaml
cd build/
make
pause