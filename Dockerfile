FROM alpine:latest

RUN apk update && apk upgrade && apk add --no-cache \
    bash \
    make \
    g++ \
    && rm -rf /var/cache/apk/*

COPY . /usr/webserv

WORKDIR /usr/webserv

RUN make re

EXPOSE 8080