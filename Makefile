.PHONY: all re clean fclean run

FILES=srcs/main.c srcs/codex_input_functions.c srcs/codex_funcs.c\
	  srcs/codex_helper_funcs.c srcs/codex_line_1.c srcs/codex_line_2.c
OBJS=$(FILES:.c=.o)
HEADERS=includes/codex.h
NAME=codexion
FLAGS= -Wall -Wextra -Werror -g

all: $(NAME)

$(NAME):$(OBJS) 
	cc -g $(FLAGS) $^ -o $@
%.o:%.c $(HEADERS)
	cc -c -g $< -I./includes -o $@

clean:
	rm -f ./srcs/*.o
fclean: clean
	rm -f ./$(NAME)
re: fclean all
run:
	./$(NAME) 10 100 10 10 10 10 10 fifo