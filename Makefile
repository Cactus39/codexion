.PHONY: all re clean fclean

FILES=srcs/main.c srcs/codex_input_functions.c srcs/codex_funcs.c srcs/codex_helper_funcs.c
OBJS=$(FILES:.c=.o)
HEADERS=includes/codex.h
NAME=a.out
FLAGS= -Wall -Wextra -Werror -g

all: $(NAME)

$(NAME):$(OBJS) 
	cc $(FLAGS) $^ -o $@
%.o:%.c $(HEADERS)
	cc -c $< -I./includes -o $@

clean:
	rm -f ./srcs/*.o
fclean: clean
	rm -f ./$(NAME)
re: fclean all