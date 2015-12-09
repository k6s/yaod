/*
 ** sh_key.c for minishell1 in /home/cano_c/rendu/UNIX/PSU_2014_minishell1
 ** 
 ** Made by Chloe Cano
 ** Login   <cano_c@epitech.net>
 ** 
 ** Started on  Mon Jan 26 02:35:13 2015
 ** Last update Mon Jan 26 02:35:25 2015 
 */
#include <minishell1.h>

int			cu_left(t_buff **c_buff, t_buff **b_buff, WINDOW *win)
{
	if ((*c_buff)->prev)
	{
		wmove(win, getcury(win), getcurx(win) - 1);
		*c_buff = (*c_buff)->prev;
	}
	else
	{
		if ((*c_buff)->len)
		{
			if (!((*c_buff)->prev = my_memalloc(sizeof(**c_buff))))
				return (-1);
			(*c_buff)->prev->next = *c_buff;
			*c_buff = (*c_buff)->prev;
			*b_buff = *c_buff;
			wmove(win, getcury(win), getcurx(win) - 1);
		}
	}
	wrefresh(win);
	return (0);
}

int			cu_right(t_buff **c_buff, t_buff UN **b_buff, WINDOW *win)
{
	if ((*c_buff)->next)
	{
		wmove(win, getcury(win), getcurx(win) + 1);
		*c_buff = (*c_buff)->next;
	}
	wrefresh(win);
	return (0);
}

int			cu_del(t_buff **c_buff, t_buff UN **b_buff, WINDOW *win)
{
	t_buff		*tmp;

	if ((*c_buff)->prev)
	{
		tmp = *c_buff;
		(*c_buff)->prev->next = (*c_buff)->next;
		if ((*c_buff)->next)
			(*c_buff)->next->prev = (*c_buff)->prev;
		*c_buff = (*c_buff)->prev;
		free(tmp);
		mvwdelch(win, getcury(win), getcurx(win) - 1);
	}
	else if ((*c_buff)->len)
	{
		my_bzero((*c_buff)->buff, BUFF_SIZE);
		(*c_buff)->len = 0;
		mvwdelch(win, getcury(win), getcurx(win) - 1);
	}
	wrefresh(win);
	return (0);
}

/*
int		cu_do(t_buff **c_buff, t_buff **b_buff, t_hist **hist, WINDOW *win)
{
	if (*hist && (*hist)->next)
	{
		*hist = (*hist)->next;
		tputs(tgetstr("cr", NULL), 0, &tput_putc);
		tputs(tgetstr("ce", NULL), 0, &tput_putc);
		write(1, ":> ", 3);
		*c_buff = (*hist)->buff;
		*b_buff = (*hist)->buff;
		while ((*c_buff)->next)
		{
			write(1, (*c_buff)->buff, (*c_buff)->len);
			*c_buff = (*c_buff)->next;
		}
		write(1, (*c_buff)->buff, (*c_buff)->len);
	}
	return (0);
}

int			cu_up(t_buff **c_buff, t_buff **b_buff, t_hist **hist, WINDOW *win)
{
	if (*hist && (*hist)->prev)
	{
		*hist = (*hist)->prev;
		tputs(tgetstr("cr", NULL), 0, &tput_putc);
		tputs(tgetstr("ce", NULL), 0, &tput_putc);
		write(1, ":> ", 3);
		*c_buff = (*hist)->buff;
		*b_buff = (*hist)->buff;
		while ((*c_buff)->next)
		{
			write(1, (*c_buff)->buff, (*c_buff)->len);
			(*c_buff) = (*c_buff)->next;
		}
		write(1, (*c_buff)->buff, (*c_buff)->len);
	}
	return (0);
}


int			cu_cmd(t_term *s_term)
{
	char	k;

	k = 0;
	while (k != K_ESC)
	{
		k = fgetc(stdin);
		if (k == K_TAB)
			s_term->c_win = (s_term->c_win + 1) % (WIN_SH + 1);
	}
	return (0);
}

int			cu_pup(t_term *s_term)
{
	return (0);
}

int			cu_pdo(t_term *s_term)
{
	return (0);
}
*/

int			special_key(t_buff **s_buff, t_line *s_line, char *key,
						t_term *s_term)
{
	if (key[2] == K_LE)
		return (cu_left(s_buff, &s_line->line, s_term->slave.wins[WIN_SH]));
	else if (key[2] == K_RI)
		return (cu_right(s_buff, &s_line->line, s_term->slave.wins[WIN_SH]));
	else if (key[0] == K_DEL)
		return (cu_del(s_buff, &s_line->line, s_term->slave.wins[WIN_SH]));
/*	else if (key[2] == K_UP)
		return (cu_pup(s_term));
	else if (key[2] == K_DO)
		return (cu_pdo(s_term));
	else if (key[0] == 27)
		return (cu_cmd(s_term)); */
/*	else if (key[2] == K_UP)
		return (cu_up(s_buff, &s_line->line, &s_line->hist, s_term->slave.wins[WIN_SH]));
	else if (key[2] == K_DO)
		return (cu_do(s_buff, &s_line->line, &s_line->hist, s_term->slave.wins[WIN_SH])); */
	else if (key[0] == 4)
	{
		clear_line(s_term->line);
		s_term->end = 1;
	}
	else if (key[0] == 3)
		kill(s_term->pid, SIGINT);
	else if (key[0] == 13)
		waddch(s_term->slave.wins[WIN_SH], '\n');
	else
		wprintw(s_term->slave.wins[WIN_SH], "%d\n", key[0]);
	return (0);
}