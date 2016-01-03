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

void			unborder(WINDOW *win)
{
	wborder(win, ' ', ' ', ' ', ' ', 0, 0, 0, 0);
}

void			munborder(WINDOW *win, int x, int y, int len_x, int len_y)
{
	wmove(win, y, x);
	whline(win, ' ', len_x);
	wmove(win, y + 1, x);
	wvline(win, ' ', len_y);
	wmove(win, y + 1, x + len_x);
	wvline(win, ' ', len_y);
	wmove(win, y + len_y, x);
	whline(win, ' ', len_x);
}

void			mborder(WINDOW *win, int x, int y, int len_x, int len_y)
{
	wmove(win, y, x);
	whline(win, '_', len_x);
	wmove(win, y + 1, x);
/*	wvline(win, '|', len_y);
	wmove(win, y + 1, x + len_x);
	wvline(win, '|', len_y); */
	wmove(win, y + len_y, x);
	whline(win, '_', len_x);
}

int			cu_up(WINDOW *win, int *x, int *y,
				  void (*ft_refresh)(WINDOW *, int, int))
{
	if (*y > 0)
	{
		ft_refresh(win, *y - 1, *x);
		--(*y);
	}
}

int			cu_do(WINDOW *win, int *x, int *y,
				  void (*ft_refresh)(WINDOW *, int, int))
{
	ft_refresh(win, *y + 1, *x);
	++(*y);
}

int			edit_stack(t_term UN *s_term)
{
	char	c;
	t_slave	*slave;
	int		x;
	int		y;

	x = 0;
	y = 0;
	slave = &s_term->slave;
	mborder(s_term->slave.wins[WIN_MAIN], 1, 2, WIN_STACK_CO,
			WIN_STACK_LI);
	wrefresh(s_term->slave.wins[WIN_MAIN]);
	stack_refresh(s_term->slave.wins[WIN_STACK], x, y);
	wrefresh(slave->wins[WIN_STACK]);
	while ((c = getchar()))
	{
		if (c == K_TAB)
		{
			munborder(s_term->slave.wins[WIN_MAIN], 1, 2, WIN_STACK_CO,
					  WIN_STACK_LI);
			wrefresh(slave->wins[WIN_MAIN]);
			stack_refresh(s_term->slave.wins[WIN_STACK], y, x);
			return (0);
		}
		else if (c == K_UP)
			cu_up(s_term->slave.wins[WIN_STACK], &x, &y, &stack_refresh);
		else if (c == K_DO)
			cu_do(s_term->slave.wins[WIN_STACK], &x, &y, &stack_refresh);
	}
	return (-1);
}

int			edit_code(t_term UN *s_term)
{
	char	c;
	t_slave	*slave;

	slave = &s_term->slave;
	code_refresh(slave->wins[WIN_CODE], 0, 0);
	mborder(s_term->slave.wins[WIN_MAIN], WIN_CODE_OX - 1, WIN_CODE_OY - 1,
			WIN_CODE_CO + 1, WIN_CODE_LI + 1);
	wrefresh(s_term->slave.wins[WIN_MAIN]);
	code_refresh(slave->wins[WIN_CODE], 0, 0);
	while ((c = getchar()))
	{
		if (c == K_TAB)
		{
			munborder(s_term->slave.wins[WIN_MAIN], WIN_CODE_OX - 1,
					  WIN_CODE_OY - 1, WIN_CODE_CO + 1, WIN_CODE_LI + 1);
			wrefresh(s_term->slave.wins[WIN_MAIN]);
			code_refresh(slave->wins[WIN_CODE], 0, 0);
			return (0);
		}
	}
	return (-1);
}

int			edit_regs(t_term *s_term)
{
	char	c;
	t_slave	*slave;

	slave = &s_term->slave;
	mborder(slave->wins[WIN_MAIN], WIN_REGS_OX - 2, WIN_REGS_OY - 1,
			WIN_REGS_CO + 3, WIN_REGS_LI + 1);
	wrefresh(slave->wins[WIN_MAIN]);
	wrefresh(slave->wins[WIN_REGS]);
	while ((c = getchar()))
	{
		if (c == K_TAB)
		{
			munborder(slave->wins[WIN_MAIN], WIN_REGS_OX - 2, WIN_REGS_OY - 1,
					WIN_REGS_CO + 3, WIN_REGS_LI + 1);
			wrefresh(slave->wins[WIN_MAIN]);
			wrefresh(slave->wins[WIN_REGS]);
			return (0);
		}
	}
	return (-1);
}

int			edit_call(t_term UN *s_term)
{
	char	c;
	t_slave	*slave;

	slave = &s_term->slave;
	mborder(slave->wins[WIN_MAIN], WIN_CALL_OX - 1, WIN_CALL_OY - 1,
			WIN_CALL_CO + 1, WIN_CALL_LI + 1);
	wrefresh(slave->wins[WIN_MAIN]);
	call_refresh(slave->wins[WIN_CALL], 0, 0);
	while ((c = getchar()))
	{
		if (c == K_TAB)
		{
			munborder(slave->wins[WIN_MAIN], WIN_CALL_OX - 1, WIN_CALL_OY - 1,
					WIN_CALL_CO + 1, WIN_CALL_LI + 1);
			wrefresh(slave->wins[WIN_MAIN]);
			call_refresh(slave->wins[WIN_CALL], 0, 0);
			return (0);
		}
	}
	return (-1);
}

int			cu_tab(t_term *s_term)
{
	t_slave	*slave;
	int		(*ft_wins[WIN_SCR])(t_term *) = {
		[WIN_SH] = NULL,
		[WIN_STACK] = &edit_stack,
		[WIN_REGS] = &edit_regs,
		[WIN_CODE] = &edit_code,
		[WIN_CALL] = &edit_call
	};

	slave = &s_term->slave;
	munborder(slave->wins[WIN_MAIN], WIN_SH_OX - 1, WIN_SH_OY - 1,
			WIN_SH_CO + 1, WIN_SH_LI + 1);
	wrefresh(slave->wins[WIN_MAIN]);
	sh_refresh(slave->wins[WIN_SH], 0, 0);
	while (1)
	{
		slave->c_win = (slave->c_win + 1) % WIN_SCR;
		if (slave->c_win == WIN_SH)
		{
			mborder(slave->wins[WIN_MAIN], WIN_SH_OX - 1, WIN_SH_OY - 1,
					  WIN_SH_CO + 1, WIN_SH_LI + 1);
			wrefresh(slave->wins[WIN_MAIN]);
			sh_refresh(slave->wins[WIN_SH], 0, 0);
			return (0);
		}
		if (ft_wins[slave->c_win])
			ft_wins[slave->c_win](s_term);
	}
}

int			special_key(t_buff **s_buff, t_line *s_line, char *key,
						t_term *s_term)
{
	if (key[2] == K_LE)
		return (cu_left(s_buff, &s_line->line, s_term->slave.wins[WIN_SH]));
	else if (key[2] == K_RI)
		return (cu_right(s_buff, &s_line->line, s_term->slave.wins[WIN_SH]));
	else if (key[0] == K_DEL)
		return (cu_del(s_buff, &s_line->line, s_term->slave.wins[WIN_SH]));
	else if (key[0] == K_TAB)
		return (cu_tab(s_term));
	else if (key[0] == 3)
		kill(s_term->pid, SIGINT);
	else if (key[0] == 4)
		s_term->end = 1;
	else if (key[0] == 13)
		waddch(s_term->slave.wins[WIN_SH], '\n');
	return (0);
}
