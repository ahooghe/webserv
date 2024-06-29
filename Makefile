NAME = webserver
CC = g++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g
SRCS = \
    src/WebServer.cpp \
    src/main.cpp \
    src/Config-Http.cpp \
    src/Config-Server.cpp \
    src/Config-Location.cpp \

HEADERS = \
    include/Config.hpp \
    include/WebServer.hpp \

OBJS = $(SRCS:.cpp=.o)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp $(HEADERS)
	$(CC) -I./include $(CFLAGS) -c $< -o $@

clean:
		rm -f $(OBJS)

fclean: clean
		rm -f $(NAME)

