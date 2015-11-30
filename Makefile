##
## Makefile for minishell1 in /home/cano_c/rendu/UNIX/PSU_2014_minishell1
## 
## Made by Chloe Cano
## Login   <cano_c@epitech.net>
## 
## Started on  Tue Jan 13 15:34:51 2015
## Last update Tue Jan 20 19:03:41 2015 
##

NAME	= yaod
SRCS	= 		sh/main.c \
				sh/get_next_line.c sh/get_next_line_2.c \
				sh/builtins_tree.c sh/progs_tree.c \
				sh/env_builtins.c sh/my_exit.c sh/my_cd.c \
				sh/sh_exe.c \
				sh/sh_cmd.c \
				sh/sh_env.c \
				sh/sh_free.c \
				sh/sh_signal.c \
				sh/sh_termcap.c \
				sh/sh_hist.c \
				sh/sh_key.c \
				sh/my_getpid.c \
				curses.c \
				output.c \
				update_slave_state.c \
				ptrace_get.c \
				stack.c \
				regs.c \
				color_output.c \
				run_slave.c \
				bp.c \
				elf_parse.c \
				elf_file.c \
				elf_process.c \
				segment.c \
				code.c \
				print_mem.c \
				showmem.c
OBJS	= $(addprefix obj/, $(SRCS:.c=.o))
CFLAGS	+= -Wall -Wextra -ansi -lcurses -ggdb -std=gnu11
LDFLAGS	+= -L lib/ -lmy -l capstone -lm

all: lib/libmy.a obj/ $(NAME)


obj/:
	mkdir $@
	mkdir obj/sh/

lib/libmy.a:
	make -C lib/my

$(NAME): $(OBJS)
	gcc -o $@ $(CFLAGS) $(OBJS) $(LDFLAGS)

obj/%.o: src/%.c
	gcc -c $(CFLAGS) -o $@ $< -I include -I ~/null/libft/include

clean:
	make -C lib/my clean
	rm -rf obj/

fclean:
	$(MAKE) -C lib/my fclean
	rm -rf obj/
	rm -rf $(NAME)

re: fclean all
