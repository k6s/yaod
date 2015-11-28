##
## Makefile for eval_expr in /home/cano_c/rendu/Piscine_C_eval_expr
## 
## Made by Chloe Cano
## Login   <cano_c@epitech.net>
## 
## Started on  Sun Oct 26 22:45:57 2014
## Last update Mon Jan 19 17:12:29 2015 
##

NAME	= ../libmy.a
SRCS	= my_putchar.c my_getnbr.c my_putstr.c \
	my_memcpy.c my_bzero.c my_strlen.c my_getenv.c \
	my_strcmp.c my_memcmp.c my_memmove.c my_strncmp.c \
	my_strdup.c my_memalloc.c my_putendl.c my_strncpy.c \
	str_split_whitespc.c my_strncat.c my_strcat.c
OBJS	= $(addprefix obj/, $(notdir $(SRCS:.c=.o)))
OBJ	= $(SRCS:.c=.o)
CFLAGS	+= -Wall -Wextra -O3
INC	= include

all: obj $(NAME)

$(NAME): $(OBJS)
	ar rc $(NAME) $(OBJS)
	ranlib $(NAME)

obj:
	mkdir obj/

obj/%.o: src/%.c
	gcc $(CFLAGS) -c -o $@ $< -I $(INC)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all
