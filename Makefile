NAME = webserver
CC = g++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g
SRCS = \
    src/Request.cpp \
    src/Config-Http.cpp \
    src/Config-ErrorCheck.cpp \
    src/WebServer.cpp \
    src/CGI.cpp \
    src/Request-Getters.cpp \
    src/CGI-helper.cpp \
    src/Response-Helpers.cpp \
    src/Response-Getters.cpp \
    src/Request-Helpers.cpp \
    src/Response.cpp \
    src/Config-Location.cpp \
    src/main.cpp \
    src/Config-Operators.cpp \
    src/Server.cpp \
    src/Config-Server.cpp \
    src/Config-Getters.cpp \

HEADERS = \
    include/Server.hpp \
    include/Request.hpp \
    include/CGI.hpp \
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

