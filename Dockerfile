FROM gcc:latest

COPY . /usr/webserve

WORKDIR /usr/webserve

RUN make re

EXPOSE 8080