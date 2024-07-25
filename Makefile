NAME = webserver
CC = g++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g
SRCS = \
    src/Response-Helpers.cpp \
    src/WebServer.cpp \
    src/main.cpp \
    src/Response.cpp \
    src/Request.cpp \
    src/Server.cpp \
    src/Config-Http.cpp \
    src/Config-Server.cpp \
    src/Config-Location.cpp \
    src/Config-Getters.cpp \
    src/Response-Getters.cpp \

HEADERS = \
    include/Server.hpp \
    include/Error.hpp \
    include/Request.hpp \
    include/Config.hpp \
    include/Response.hpp \
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

