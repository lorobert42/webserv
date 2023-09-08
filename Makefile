
NAME		=	webserv

SRCS		= srcs/main.cpp \
					srcs/hello/Hello.cpp \

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

.PHONY:		all clean fclean re
