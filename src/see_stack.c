/*
 ** see_stack.c for dump_stack in /home/k6/null/dump_stack
 ** 
 ** Made by Chloe Cano
 ** Login   <k6@epitech.net>
 ** 
 ** Started on  Thu Jan 22 18:43:36 2015
 ** Last update Thu Jan 22 18:43:49 2015 
 */
#include <minishell1.h>

WINDOW          **dump_stack_start(int argc, char **argv, char **environ,
								   t_slave *s_slave)
{
	if (argc > 1)
	{
		if ((s_slave->wins = curses_init()))
		{
			s_slave->filename = argv[1];
			start_slave(argv[1], argv + 1, environ, s_slave);
		}
		return (s_slave->wins);
	}
	return (NULL);
}
