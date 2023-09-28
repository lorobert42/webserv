
NAME		= webserv
NAME_TEST	= webserv_test

SRCS		= srcs/main.cpp \
			  srcs/server/Server.cpp

SRCS_TEST	= srcs/epoll_serv.cpp

OBJS		= $(SRCS:.cpp=.o)
OBJS_TEST	= $(SRCS_TEST:.cpp=.o)

ifeq ($(shell uname), Linux)
 CC			= g++ -g
else
 CC			= c++ -g
endif

CPPFLAGS	= -Wall -Werror -Wextra -std=c++98

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

docker-start:
			docker build -t webserv .
			docker run -it --rm -p 8080:8080 --name=webserv webserv /bin/bash

docker-stop:
			docker stop webserv

.PHONY:		all clean fclean re
