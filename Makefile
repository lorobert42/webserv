
NAME		= webserv
NAME_TEST	= webserv_test

SRCS		= srcs/main.cpp \
			srcs/server/ServerManager.cpp \
			srcs/server/Server.cpp \
			srcs/client/Client.cpp \
			srcs/client/ClientHelper.cpp \
			srcs/client/Request.cpp \
			srcs/client/Response.cpp \
			srcs/config/Config.cpp \
			srcs/config/ConfigServer.cpp \
			srcs/config/ConfigHostname.cpp \
			srcs/config/ConfigRoute.cpp \
			srcs/config/ConfigHelper.cpp \
			srcs/cgi/CgiHandler.cpp \
			srcs/autoindex/AutoIndex.cpp \
			srcs/utils/strtolower.cpp \
			srcs/utils/readFile.cpp \

SRCS_TEST	= srcs/epoll_serv.cpp

OBJS		= $(SRCS:.cpp=.o)
OBJS_TEST	= $(SRCS_TEST:.cpp=.o)

ifeq ($(shell uname), Linux)
 CC			= g++ #-g
else
 CC			= c++ #-g
endif

CPPFLAGS	= -Wall -Werror -Wextra -std=c++98 #-fsanitize=address

EXE_NAME	= -o $(NAME)
EXE_NAME_T	= -o $(NAME_TEST)

all:		$(NAME)

test:		$(OBJS_TEST)
			$(CC) $(CPPFLAGS) $(OBJS_TEST) $(EXE_NAME_T)

$(NAME):	$(OBJS)
			$(CC) $(CPPFLAGS) $(OBJS) $(EXE_NAME)

clean:
			rm -rf $(OBJS) $(OBJS_TEST)
	
fclean:		clean
			rm -rf $(NAME) $(NAME_TEST)

re:			fclean all

.cpp.o:
			$(CC) $(CPPFLAGS) -c $< -o $(<:.cpp=.o)

docker-build:
			docker-compose -p webserv build

docker-up:
			docker-compose -p webserv up --build -d
			docker cp ${PWD}/nginx/nginx.conf webserv:/etc/nginx/nginx.conf
			docker exec -it webserv /bin/bash -c "make re && nginx -s reload"
			docker exec -it webserv /bin/bash

docker-down:
			docker-compose -p webserv down

docker:
			docker-compose -p webserv build
			docker-compose -p webserv up -d

.PHONY:		all clean fclean re
