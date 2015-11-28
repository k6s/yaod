/*
 ** sh_signal.c for minishell1 in /home/cano_c/rendu/UNIX/PSU_2014_minishell1
 ** 
 ** Made by Chloe Cano
 ** Login   <cano_c@epitech.net>
 ** 
 ** Started on  Mon Jan 19 16:16:41 2015 
 ** Last update Tue Jan 20 18:48:00 2015 
 */
#include <minishell1.h>

void			handle_term(int UN sig)
{
	t_term		*s_term;

	s_term = static_term(NULL);
	s_term->end = 1;
}

void		  	clear_line(t_buff *p_buff)
{
	t_buff		*tmp;

	tmp = p_buff;
	if (p_buff)
	{
		p_buff = p_buff->next;
		tmp->next = NULL;
		my_bzero(tmp->buff, BUFF_SIZE);
		tmp->len = 0;
	}
	while (p_buff)
	{
		tmp = p_buff;
		p_buff = p_buff->next;
		free(tmp);
	}
}

void			handle_int(int UN signal)
{
	t_term		*s_term;

	if ((s_term = static_term(NULL)))
	{
		waddstr(s_term->slave.wins[WIN_SH], "^C");
		if (s_term->line)
			clear_line(s_term->line);
		s_term->line = NULL;
		s_term->intr = 1;
	}
	init_signals();
}

void			init_signals(void)
{
	signal(SIGTERM, &handle_term);
	signal(SIGINT, &handle_int);
	signal(SIGQUIT, &handle_term);
	signal(SIGHUP, &handle_term);
}
