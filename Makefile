NAME = philo

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
RM = rm -f

SRCS = main.c init.c utils.c queue.c another_queue.c philosopher.c routine.c manager.c forks.c watchdog.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.c philo.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re