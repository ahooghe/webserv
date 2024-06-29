# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    makefile-maker.sh                                  :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: brmajor <brmajor@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/25 17:13:28 by ahooghe           #+#    #+#              #
#    Updated: 2024/02/20 13:20:53 by brmajor          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/bash

SHELL_TYPE=$(basename "$SHELL")

if [ "$SHELL_TYPE" = "bash" ] || [ "$SHELL_TYPE" = "sh" ]; then
    ECHO_FLAG="-e"
else
    ECHO_FLAG=""
fi

NAME=$(basename "$(pwd)")
INCLUDES=include
SOURCES=src

echo "NAME = $NAME" > Makefile
echo "CC = g++" >> Makefile
echo "CFLAGS = -Wall -Wextra -Werror -std=c++98 -g" >> Makefile
echo "SRCS = \\" >> Makefile

# Find all .cpp files in the srcs directory and add them to the makefile
for file in $(find $SOURCES -name "*.cpp"); do
    echo "    $file \\" >> Makefile
done

echo >> Makefile
echo "HEADERS = \\" >> Makefile

# Find all .h files in the inc directory and add them to the makefile
for file in $(find $INCLUDES -name "*.hpp"); do
    echo "    $file \\" >> Makefile
done

echo >> Makefile
echo "OBJS = \$(SRCS:.cpp=.o)" >> Makefile
echo >> Makefile
echo "\$(NAME): \$(OBJS)" >> Makefile
echo $ECHO_FLAG "\t\$(CC) \$(CFLAGS) -o \$(NAME) \$(OBJS)" >> Makefile
echo >> Makefile
echo '%.o: %.cpp $(HEADERS)' >> Makefile
echo $ECHO_FLAG "\t\$(CC) -I./$INCLUDES \$(CFLAGS) -c \$< -o \$@" >> Makefile
echo >> Makefile
echo "clean:" >> Makefile
echo $ECHO_FLAG "\t\trm -f \$(OBJS)" >> Makefile
echo >> Makefile
echo "fclean: clean" >> Makefile
echo $ECHO_FLAG "\t\trm -f \$(NAME)" >> Makefile
echo >> Makefile
