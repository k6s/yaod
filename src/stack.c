/*
 ** stack.c for dump_stack in /home/k6/null/dump_stack
 ** 
 ** Made by Chloe Cano
 ** Login   <k6@epitech.net>
 ** 
 ** Started on  Fri Jan 30 04:26:27 2015
 ** Last update Fri Jan 30 04:27:04 2015 
 */
#include <see_stack.h>
#include <stdio.h>
#include <wchar.h>
#include <unistd.h>

void							addr_prefix(long base, long idx, long var_len,
									WINDOW *win)
{
	if (idx % 16)
		idx -= idx % 16;
	while (idx < var_len)
	{
		wmove(win, idx / 16, 0);
		wattron(win, COLOR_PAIR(3));
		wprintw(win, "%lx: ", (void *)((long)(base - idx)));
		wattron(win, COLOR_PAIR(1));
		idx += 16;
	}
}

void							new_stack_msg(WINDOW **wins)
{
	wmove(wins[WIN_MAIN], 1, 3);
	wprintw(wins[WIN_MAIN], "%s", NEW_FRAME_MSG);
	wrefresh(wins[WIN_MAIN]);
}

char							is_sane_stack_addr(long sp, long bp,
												   WINDOW **wins)
{
	if (bp == sp)
		new_stack_msg(wins);
	if (bp - sp >= 0 && bp - sp < MAX_STACK_FRAME && (bp > 0xfffffff || !bp))
		return (1);
	return (0);
}

void							adump_dword(WINDOW *win, char *var, ssize_t i,
											size_t li, size_t co, int color)
{
	size_t						c_co;

	c_co = (31 % 16) * 2 + ((31 + i) % 16) / 2;
	c_co += c_co / 19 ? 1 : 0;
	c_co += co + 6;
	if (c_co == 44)
	{
		wattron(win, COLOR_PAIR(1));
		wmove(win, li, 43 + ADDR_PLEN);
		waddch(win, '|');
		wattron(win, color);
	}
	wmove(win, li, c_co + 1 + ADDR_PLEN);
	if (*(unsigned char *)(var + i) >= ' ' && *(unsigned char *)(var + i)
		<= '~')
		waddch(win, *(unsigned char *)(var + i));
	else
		waddch(win, '.');
}

void							axdump_dword(WINDOW *win, char *var, size_t li,
											 size_t co, ssize_t len, int color)
{
	int							i;
	unsigned char				c_co;
	char						*x = "0123456789abcdef";

	i = 0;
	while (i < len && (!(i + co) || (i + co) % 16))
	{
		c_co = ((co + i) % 16) * 2 + ((co + i) % 16) / 2;
		c_co += c_co / 19 ? 1 : 0;
		if (c_co + 1 > 0 && c_co + 1 < WIN_STACK_CO)
		{
			wmove(win, li, c_co + 1 + ADDR_PLEN);
			wattron(win, color);
			waddch(win, x[*(unsigned char *)(var + i) / 16 % 16]);
			waddch(win, x[*(unsigned char *)(var + i) % 16]);
			wrefresh(win);
			adump_dword(win, var, i, li, co, color);
		}
		++i;
	}
}

void						add_hl(unsigned *hl, size_t idx, size_t len)
{
	size_t				i;

	i = 0;
	while (i < MAX_STACK_FRAME && *(hl + i * 2 + 1))
		++i;
	*(hl + i * 2) = idx;
	*(hl + i * 2 + 1) = len;
}

void						update_var(WINDOW *win, char *var, size_t off,
									   size_t var_len, unsigned *hl, int color)
{
	size_t					li;
	size_t					co;
	size_t					start;
	size_t					l_line;

	li = off / 16;
	co = off % 16;
	start = off;
	if (hl)
		add_hl(hl, off, var_len);
	off = 0;
	while (off < var_len)
	{
		/*l_line = (var_len - off + start) / 16 ? 16 : (var_len - off + start % 16); */
		l_line = 1;
		co = (off + start) % 16;
		li = (off + start) / 16;
		axdump_dword(win, var + off, li, co, l_line, color);
		off += l_line;
	}
	prefresh(win, 0, 0, WIN_BORDER_LEN, WIN_BORDER_LEN,
			 WIN_STACK_LI, WIN_STACK_CO);
}

void							update_vars(struct user_regs_struct *regs,
											long stack_frame,
											long old_stack_frame, WINDOW **wins,
											unsigned *hl)
{
	size_t						idx = 0;
	size_t						var_len;

	while (regs->rbp - idx + 8 > regs->rsp)
	{
		var_len = 0;
		while (regs->rbp - idx + 8 > regs->rsp
			   && *(char *)(stack_frame + idx)
			   == *(char *)(old_stack_frame + idx))
			++idx;
		while (regs->rbp - idx - var_len + 8 > regs->rsp
			   && *(char *)(stack_frame + idx + var_len)
			   != *(char *)(old_stack_frame + idx + var_len))
			++var_len;
		if (var_len)
		{
			update_var(wins[WIN_STACK], (char *)stack_frame + idx, idx, var_len,
					   hl, COLOR_PAIR(2));
			idx += var_len;
		}
	}
}

int								dump_new_vars(struct user_regs_struct *regs,
											  struct user_regs_struct *old_regs,
											  void *stack_frame, WINDOW *win,
											  unsigned *hl)
{
	size_t					idx;
	size_t					var_len;
	size_t					c_co;
	char					disa;

	if (old_regs->rsp > regs->rsp)
	{
		idx = regs->rbp - old_regs->rsp + 8;
		var_len = old_regs->rsp - regs->rsp;
		addr_prefix(regs->rbp, idx, var_len + idx, win);
		if ((var_len + idx) / 16 < WIN_OSTACK_LI)
			update_var(win, (char *)stack_frame + idx, idx, var_len, hl,
					   COLOR_PAIR(2));
		prefresh(win, 0, 0, WIN_BORDER_LEN, WIN_BORDER_LEN,
			WIN_STACK_LI, WIN_STACK_CO);
	}
	else
	{
		disa = (regs->rbp % 16 ? 1 : 0) ^ (regs->rbp - regs->rsp % 16 ? 1 : 0);
		idx = regs->rbp - regs->rsp + 8;
		c_co = idx % 16;
		c_co += c_co + c_co / 2 + c_co / 8;
		wmove(win, idx / 16, c_co + (disa ? ADDR_PLEN : 0));
		if (disa)
			wmove(win, idx / 16, 45 + idx % 16 + ADDR_PLEN);
		wclrtobot(win);
		prefresh(win, 0, 0, WIN_BORDER_LEN, WIN_BORDER_LEN,
			WIN_STACK_LI, WIN_STACK_CO);
	}
	return (0);
}

void										un_hl(WINDOW *win, unsigned *hl,
												  char *data)
{
	size_t								i;

	i = 0;
	while (i < MAX_STACK_FRAME && *(hl + i * 2 + 1))
	{
		update_var(win, *(hl + i * 2) + data, *(hl + i * 2), *(hl + i * 2 + 1),
				   NULL, COLOR_PAIR(1));
		++i;
	}
}

void					update_stack_window(WINDOW **wins,
											struct user_regs_struct *regs,
											struct user_regs_struct *old_regs,
											void *stack_frame,
											void *old_stack_frame,
											unsigned *hl)
{
	if (old_regs->rsp == regs->rsp && old_stack_frame)
		update_vars(regs, (long)stack_frame, (long)old_stack_frame, wins,
					(unsigned *)hl);
	if (old_regs->rbp != regs->rbp || !old_regs->rbp)
		dump_new_frame(regs, old_regs, stack_frame, wins);
	else if (old_regs->rsp != regs->rsp)
		dump_new_vars(regs, old_regs, stack_frame, wins[WIN_STACK],
					  (unsigned *)hl);
}

void							clean_stack_step(WINDOW **wins, unsigned *hl,
												 void *stack_frame)
{
	touchwin(wins[WIN_STACK]);
	prefresh(wins[WIN_OSTACK], 0, 0, WIN_BORDER_LEN, WIN_BORDER_LEN,
			 WIN_STACK_LI, WIN_STACK_CO);
	wrefresh(wins[WIN_STACK]);
	usleep(100000);
	un_hl(wins[WIN_STACK], (unsigned *)hl, stack_frame);
	touchwin(wins[WIN_STACK]);
	prefresh(wins[WIN_OSTACK], 0, 0, WIN_BORDER_LEN, WIN_BORDER_LEN,
			 WIN_STACK_LI, WIN_STACK_CO);
	wrefresh(wins[WIN_STACK]);
	bzero(hl, sizeof(*hl) * MAX_STACK_FRAME * 2);
	wmove(wins[WIN_MAIN], 1, 3);
	wclrtoeol(wins[WIN_MAIN]);
	wrefresh(wins[WIN_MAIN]);
}

int                             dump_stack(pid_t pid,
										   struct user_regs_struct *old_regs,
										   struct user_regs_struct *regs,
										   WINDOW **wins, char clean)
{
	static void					*stack_frame = NULL;
	static char					*old_stack_frame = NULL;
	static unsigned				hl[MAX_STACK_FRAME * 2] = {};

	if (is_sane_stack_addr(regs->rsp, regs->rbp, wins))
	{
		if (clean)
			clean_stack_step(wins, hl, stack_frame);
		free(old_stack_frame);
		old_stack_frame = stack_frame;
		if (!(stack_frame = get_rev_data(pid, regs->rbp, regs->rsp)))
			return (-1);
		update_stack_window(wins, regs, old_regs, stack_frame, old_stack_frame,
							(unsigned *)hl);
		return (0);
	}
	else
		new_stack_msg(wins);
	return (-1);
}
