FROM rhub/ubuntu-gcc:latest

RUN apt-get update -y && apt-get upgrade -y

RUN apt-get install libboost-all-dev -y

RUN mkdir /usr/src/node

COPY ./networking/node.cpp /usr/src/node

COPY ./networking/node.h /usr/src/node

WORKDIR /usr/src/node

RUN g++ -o Node node.cpp node.h

CMD ["./Node"]