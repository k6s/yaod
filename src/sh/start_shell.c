/*
 * ** main.c for minishell1 in /home/cano_c/rendu/UNIX/PSU_2014_minishell1
 * ** 
 * ** Made by Chloe Cano
 * ** Login   <cano_c@epitech.net>
 * ** 
 * ** Started on  Tue Jan 13 13:47:28 2015
 * ** Last update Sun Jan 25 03:52:57 2015 
 * */
#include <minishell1.h>

static int			init_mysh(t_term *s_term, char **environ)
{
	char			*env_path;

	my_bzero((void *)s_term, sizeof(*s_term));
	if (environ && *environ)
	{
		get_environ(s_term, environ);
		static_term(s_term);
		init_signals();
		s_term->tsave = init_termcap(s_term->environ);
		s_term->pid = my_getpid();
		s_term->progs = NULL;
		if ((env_path = my_getenv(environ, "PATH="))
			&& (s_term->progs = get_path_progs(env_path))
			&& !add_builtins(s_term->progs))
			return (1);

	}
	return (0);
}

int	i				start_shell(char **environ)
{
	t_term			s_term;
	int				ret;

	ret = -1;
	if (init_mysh(&s_term, environ))
		ret = shell_readloop(&s_term);
	free_stuff(&s_term);
	restore_termcap(s_term.tsave);
	return (ret);
}
