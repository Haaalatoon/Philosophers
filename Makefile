NAME = philo

CC = cc
CFLAGS = -Wall -Wextra -Werror
RM = rm -f

SRCS = \
	main.c \
	init.c \
	init_helpers.c \
	monitor.c \
	routine.c \
	utils.c \
	print.c \
	cleanup.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
