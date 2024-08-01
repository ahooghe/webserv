NAME = webserv
CC = g++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g
SRCS = \
    src/Config-ErrorCheck.cpp \
    src/Config-Getters.cpp \
    src/Config-Http.cpp \
    src/Config-Location.cpp \
    src/Config-Operators.cpp \
    src/Config-Server.cpp \
    src/Request-Getters.cpp \
    src/Request-Helpers.cpp \
    src/Response-Getters.cpp \
    src/Response-Helpers.cpp \
    src/Server.cpp \
    src/WebServer.cpp \
    src/main.cpp \
    src/Request.cpp \
    src/CGI-helper.cpp \
    src/CGI.cpp \
    src/Response.cpp \

HEADERS = \
    include/CGI.hpp \
    include/Config.hpp \
    include/Error.hpp \
    include/Request.hpp \
    include/Response.hpp \
    include/Server.hpp \
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

