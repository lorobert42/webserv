FROM alpine:latest

RUN apk update && apk upgrade && apk add --no-cache \
    bash \
    make \
    g++ \
    nginx \
    vim \
    valgrind \
    php \
    php-fpm \
    php-cgi \
    && rm -rf /var/cache/apk/*

COPY . /home/webserv
COPY ./docker_conf/php.local.ini /etc/php81/conf.d/php.local.ini

WORKDIR /home/webserv

EXPOSE 8080 80 8081

CMD ["nginx", "-g", "daemon off;"]