FROM alpine:latest

RUN apk update && apk upgrade && apk add --no-cache \
    bash \
    make \
    g++ \
    && rm -rf /var/cache/apk/*

COPY . /home/webserv

WORKDIR /home/webserv

EXPOSE 8080