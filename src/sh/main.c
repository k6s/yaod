/*
 ** main.c for minishell1 in /home/cano_c/rendu/UNIX/PSU_2014_minishell1
 ** 
 ** Made by Chloe Cano
 ** Login   <cano_c@epitech.net>
 ** 
 ** Started on  Tue Jan 13 13:47:28 2015
 ** Last update Sun Jan 25 03:52:57 2015 
 */
#include <minishell1.h>
#include <see_stack.h>

int			init_mysh(t_term *s_term, char **environ)
{
	char			*env_path;

	if (environ && *environ)
	{
		get_environ(s_term, environ);
		static_term(s_term);
		init_signals();
		/* s_term->tsave = init_termcap(s_term->environ); */
		s_term->pid = my_getpid();
		s_term->progs = NULL;
		if ((env_path = my_getenv(environ, "PATH="))
			&& (s_term->progs = get_path_progs(env_path))
			&& !add_builtins(s_term->progs))
			return (1);
	}
	return (0);
}

int			stack_readloop(t_term *s_term, WINDOW **wins)
{
	char	k;

	box(wins[WIN_STACK], '|', '-');
	while (s_term->c_win == WIN_OSTACK)
	{
		k = fgetc(stdin);
		if (k == K_UP)
			wmove(wins[WIN_STACK], getcury(wins[WIN_STACK]) + 1, getcurx(wins[WIN_STACK]));
		else if (k == K_DO)
			wmove(wins[WIN_STACK], getcury(wins[WIN_STACK]) - 1, getcurx(wins[WIN_STACK]));
		else if (k == K_ESC)
		{
			while (k != K_ESC)
			{
				k = fgetc(stdin);
				if (k == K_TAB)
					s_term->c_win = (s_term->c_win + 1) % (WIN_SH + 1);
			}
		}
		prefresh(wins[WIN_STACK], getcury(wins[WIN_STACK]),
				 getcurx(wins[WIN_STACK]), WIN_BORDER_LEN, WIN_BORDER_LEN,
				 WIN_STACK_LI, WIN_STACK_CO);
	}
	box(wins[WIN_STACK], ' ', ' ');
	return (0);
}

void		welcome_user(WINDOW *win)
{
	wattron(win, COLOR_PAIR(3));
	wmove(win, WIN_SH_OY - 2, 20);
	wprintw(win, "<<< Welcome to YAOD - h to see command list >>>");
	wrefresh(win);
	wattron(win, COLOR_PAIR(1));
}

int			main(int argc, char **argv, char **environ)
{
	t_term		s_term;
	int			ret;
	WINDOW        **wins;

	if (argc == 2)
	{
		ret = -1;
		my_bzero((void *)(&s_term), sizeof(s_term));
		if (!(wins = dump_stack_start(argc, argv, environ, &s_term.slave)))
			return (-1);
/*		welcome_user(wins[WIN_MAIN]); */
		wmove(s_term.slave.wins[WIN_SH], 0, 0);
		s_term.c_win = WIN_SH;
		if (init_mysh(&s_term, environ))
			while (!s_term.end)
			{
				if (s_term.c_win == WIN_SH)
					ret = shell_readloop(&s_term, wins);
				else
					ret = stack_readloop(&s_term, wins);
			}
		free_stuff(&s_term);
		endwin();
	}
	else
		fprintf(stderr, "usage: ./yaod elf64_path\n");
	return (ret);
}
