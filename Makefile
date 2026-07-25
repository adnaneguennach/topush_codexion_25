NAME	= codexion

CC		= cc
CFLAGS	= -Wall -Wextra -Werror -pthread
INC		= -Iincludes

SRC		= src/main.c \
		  src/parse.c \
		  src/init.c \
		  src/coder.c \
		  src/dongle.c \
		  src/heap.c \
		  src/monitor.c \
		  src/log.c \
		  src/time.c \
		  src/clean.c

OBJ		= $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c includes/codexion.h
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
