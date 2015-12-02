/*
 ** sh_cmd.c for minishell1 in /home/cano_c/rendu/UNIX/PSU_2014_minishell1
 ** 
 ** Made by Chloe Cano
 ** Login   <cano_c@epitech.net>
 ** 
 ** Started on  Mon Jan 19 16:21:51 2015 
 ** Last update Wed Jan 28 19:50:12 2015 
 */
#include <minishell1.h>

static char		process_line(t_term *s_term, char *line, WINDOW **wins)
{
	t_prog		*s_prog;
	char		**av;

	if (*line && (av = str_split_whitespc(line)))
	{
		if ((s_prog = find_prog(s_term->progs, av[0])))
		{
			if (s_prog->builtfp)
				return (exe_builtin(s_prog, av, s_term));
			else
				return (exe_cmd(s_prog, av, s_term->environ, wins));
		}
		else
			wprintw(wins[WIN_SH], "command not found\n");
		free_strtab(av);
	}
	return (0);
}

char			*concat_buff(t_buff *s_buff)
{
	size_t		tlen;
	t_buff		*b_buff;
	char			*line;

	tlen = 0;
	b_buff = s_buff;
	while (s_buff)
	{
		tlen += s_buff->len;
		s_buff = s_buff->next;
	}
	if ((line = malloc(sizeof(*line) * (tlen + 1))))
	{
		tlen = 0;
		while (b_buff)
		{
			my_memcpy(line + tlen, b_buff->buff, b_buff->len);
			tlen += b_buff->len;
			s_buff = b_buff;
			b_buff = b_buff->next;
			/*  free(s_buff); */
		}
		line[tlen] = 0;
	}
	return (line);
}

int			add_char(WINDOW *win, t_buff **c_buff, char c)
{
	t_buff		*s_buff;

	if ((*c_buff)->next)
	{
		winsch(win, c);
		wmove(win, getcury(win), getcurx(win) + 1);
	}
	else
		waddch(win, c);
	sh_refresh(win, 0, 0);
	if ((*c_buff)->len == BUFF_SIZE)
	{
		s_buff = (*c_buff)->next;
		if (((*c_buff)->next = my_memalloc(sizeof(**c_buff))))
		{
			(*c_buff)->next->prev = *c_buff;
			*c_buff = (*c_buff)->next;
			(*c_buff)->next = s_buff;
			if (s_buff)
				s_buff->prev = *c_buff;
		}
		else
			return (-1);
	}
	(*c_buff)->buff[(*c_buff)->len] = c;
	++(*c_buff)->len;
	return (0);
}

t_hist			*add_hist(t_buff *new, t_hist **hist)
{
	t_hist		*nhist;

	if ((nhist = my_memalloc(sizeof(*nhist))))
	{
		nhist->buff = new;
		if (*hist)
		{
			while ((*hist)->next)
				*hist = (*hist)->next;
			if ((*hist)->next)
				*hist = (*hist)->next;
			(*hist)->next = nhist;
			nhist->prev = *hist;
			*hist = (*hist)->next;
		}
		else
			*hist = nhist;
	}
	return (nhist);
}

int			next_line(WINDOW *win, t_line *s_line, t_buff **s_buff)
{
	waddstr(win, ":> ");
	sh_refresh(win, 0, 0);
	if ((s_line->line && *s_line->line->buff) || !s_line->line)
	{
		if (!(s_line->line = my_memalloc(sizeof(*s_line->line))))
			return (-1);
/*		if (!(add_hist(s_line->line, &s_line->hist)))
			return (-1); */
	}
	*s_buff = s_line->line;
	return (0);
}

int			shell_readloop(t_term *s_term, WINDOW **wins)
{
	t_buff		*s_buff;
	int			ret;
	char			c[4];
	ssize_t		read_len;
	t_line		s_line;
	int			x;
	int			y;

	ret = 0;
	my_bzero((void *)&s_line, sizeof(s_line));
	s_line.hist = NULL;
	bzero(c, 4);
	wmove(s_term->slave.wins[WIN_SH], 0, 0);
	while (!s_term->end && s_term->c_win == WIN_SH)
	{
		if (!next_line(wins[WIN_SH], &s_line, &s_buff))
		{
			while (!s_term->end && !s_term->intr
				   && (read_len = read(0, c, 3)) > 0
				   && c[0] != '\r' && c[0])
			{
				if (c[0] >= ' ' && c[0] <= '~')
				{
					if (add_char(wins[WIN_SH], &s_buff, *c) < 0)
						return (-1);
				}
				else
					special_key(&s_buff, &s_line, c, s_term);
				my_bzero(c, 4);
				sh_refresh(s_term->slave.wins[WIN_SH], 0, 0);
			}
			getmaxyx(wins[WIN_SH], y, x);
			wmove(wins[WIN_SH], y, x);
			waddch(wins[WIN_SH], '\n');
			sh_refresh(s_term->slave.wins[WIN_SH], 0, 0);
			if ((s_line.s = concat_buff(s_line.line)))
			{
				if (s_term->c_win == WIN_SH)
					ret = process_line(s_term, s_line.s, wins);
				free(s_line.s);
			}
		}
		else
			s_term->end = 1;
		s_term->intr = 0;
	}
	if (!s_term->end)
		ret = 0;
	return (ret);
}
