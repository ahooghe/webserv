NAME = webserv_real
CC = g++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g
SRCS = \
    src/Request.cpp \
    src/Config-Http.cpp \
    src/WebServer.cpp \
    src/Response.cpp \
    src/Config-Location.cpp \
    src/main.cpp \
    src/Server.cpp \
    src/Config-Server.cpp \

HEADERS = \
    include/Server.hpp \
    include/Request.hpp \
    include/WebServer.hpp \
    include/Error.hpp \
    include/Config.hpp \
    include/Response.hpp \

OBJS = $(SRCS:.cpp=.o)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp $(HEADERS)
	$(CC) -I./include $(CFLAGS) -c $< -o $@

clean:
		rm -f $(OBJS)

fclean: clean
		rm -f $(NAME)

