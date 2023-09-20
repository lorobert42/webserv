
NAME		=	webserv

SRCS		=	srcs/simple_server/main.cpp \
				srcs/simple_server/Server.cpp

OBJS		= $(SRCS:.cpp=.o)

ifeq ($(shell uname), Linux)
 CC			= g++ -g
else
 CC			= c++ -g
endif

CPPFLAGS	= -Wall -Werror -Wextra -std=c++98

EXE_NAME	= -o $(NAME)

all:		$(NAME)

$(NAME):	$(OBJS)
			$(CC) $(CPPFLAGS) $(OBJS) $(EXE_NAME)

clean:
			rm -rf $(OBJS)
	
fclean:		clean
			rm -rf $(NAME)

re:			fclean all

.cpp.o:
			$(CC) $(CPPFLAGS) -c $< -o $(<:.cpp=.o)

docker-start:
			docker build -t webserv .
			docker run -it --rm -p 8080:8080 --name=webserv webserv /bin/bash

docker-stop:
			docker stop webserv

.PHONY:		all clean fclean re
