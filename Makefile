CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
LDFLAGS = -pthread
SRC =	main.c \
		parsing.c \
		init.c \
		dongles.c \
		action.c \
		time.c \
		simulation.c \
		thread.c \
		useful.c \
		monitor.c \
		scheduler.c
OBJ = $(SRC:.c=.o)
NAME = ./codexion

GREEN		= \033[1;32m
BLUE		= \033[1;34m
RED			= \033[1;31m
RESET		= \033[1;36m

all: $(NAME)

$(NAME): $(OBJ)
	@printf "  $(GREEN)Executable file:$(RESET) %s\n" $@
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)

$(OBJ): codexion.h struct.h

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
