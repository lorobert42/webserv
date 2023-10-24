FROM alpine:latest

RUN apk update && apk upgrade && apk add --no-cache \
    bash \
    make \
    g++ \
    nginx \
    vim \
    valgrind \
    php82 \
    php82-fpm \
    php82-cgi \
    && rm -rf /var/cache/apk/*

COPY . /home/webserv

WORKDIR /home/webserv

EXPOSE 8080 80 8081

CMD ["nginx", "-g", "daemon off;"]