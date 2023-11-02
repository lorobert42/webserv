FROM debian:latest

RUN apt-get update && apt-get upgrade -y && apt-get install -y \
    make \
    g++ \
    nginx \
    vim \
    valgrind \
    php \
    php-fpm \
    php-cgi \
    python3 \
    && rm -rf /var/lib/apt/lists/*

COPY . /home/webserv
COPY ./docker_conf/php.local.ini /etc/php/8.2/conf.d/php.local.ini

WORKDIR /home/webserv

EXPOSE 8080 80 8081

CMD ["nginx", "-g", "daemon off;"]