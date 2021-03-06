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

void		welcome_user(WINDOW *win)
{
	wattrset(win, A_BOLD);
	wattron(win, COLOR_PAIR(3));
	wmove(win, WIN_SH_OY - 2, 30);
	wprintw(win, "<<< Welcome to YAOD >>>");
	wmove(win, WIN_SH_OY - 2, WIN_CALL_OX + WIN_BORDER_LEN);
	wprintw(win, ">>> Calls / Jumps list <<<");
	wrefresh(win);
	wattroff(win, A_BOLD);
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
		if (!(s_term.slave.wins = curses_init(s_term.slave.s_win)))
			return (-1);
		wins = s_term.slave.wins;
		start_slave(argv[1], argv + 1, environ, &s_term.slave);
		welcome_user(wins[WIN_MAIN]);
		wmove(s_term.slave.wins[WIN_SH], 0, 0);
		s_term.c_win = WIN_SH;
		if (init_mysh(&s_term, environ))
		while (!s_term.end)
			ret = shell_readloop(&s_term, wins);
		free_stuff(&s_term);
		endwin();
	}
	else
		fprintf(stderr, "usage: ./yaod elf64_path\n");
	return (ret);
}
