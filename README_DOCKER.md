first you need to open the docker platform (docker client),
then you need to open cmd with admin!
last you enter 3 comends:
folder name is the librery you work on in the docker file.
1.docker build -t <folder name> . 
2.docker images - and trak the tag of the docker you want to run
3.docker run -it --rm --name <folder name> <folder name>:<tag>

after all this actions you suppose to see in the docker client the current docker running