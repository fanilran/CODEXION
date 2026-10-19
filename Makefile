CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
SRC =   coders/main.c \
        coders/parsing.c \
        coders/init.c \
        coders/dongles.c \
        coders/action.c \
        coders/time.c \
        coders/simulation.c \
        coders/thread.c \
        coders/useful.c \
        coders/scheduler.c
OBJ = $(SRC:.c=.o)
NAME = ./codexion

GREEN       = \033[1;32m
BLUE        = \033[1;34m
RED         = \033[1;31m
RESET       = \033[1;36m

all: $(NAME)

$(NAME): $(OBJ)
	@printf "  $(GREEN)Executable file:$(RESET) %s\n" $@
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c
	@printf "  $(BLUE)CC$(RESET) %s\n" $<
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	@printf "  $(RED)RM:$(RESET) %s\n" $(OBJ)
	@rm -f $(OBJ)

fclean: clean
	@printf "  $(RED)RM:$(RESET) %s\n" $(NAME)
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
