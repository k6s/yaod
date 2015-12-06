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

void							dump_new_frame(struct user_regs_struct *regs,
                                 struct user_regs_struct *old_regs,
                                 void *stack_frame, WINDOW **wins)
{
  wclear(wins[WIN_REGS]);
  dump_regs_name(wins[WIN_REGS]);
  if (old_regs)
    dump_regs(old_regs, regs, wins, 1);
  werase(wins[WIN_STACK]);
  addr_prefix(regs->rbp, 0, regs->rbp - regs->rsp + 8, wins[WIN_STACK]);
/*  dump_data((char *)stack_frame, (char *)((long)stack_frame + regs->rbp
										  - regs->rsp + 8),
            wins[WIN_STACK], 0, 0); */
  update_var(wins[WIN_STACK], stack_frame, 0, regs->rbp - regs->rsp + 8, NULL, 1);
  touchwin(wins[WIN_STACK]);
  prefresh(wins[WIN_STACK], 0, 0, WIN_BORDER_LEN, WIN_BORDER_LEN, WIN_STACK_LI, 
		   WIN_STACK_CO);
}
void		update_stack_window(WINDOW **wins, struct user_regs_struct *regs,
								struct user_regs_struct *old_regs,
								void *stack_frame, void *old_stack_frame,
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

void		clean_stack_step(WINDOW **wins, unsigned *hl, void *stack_frame)
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
	memset(hl, 0, sizeof(*hl) * MAX_STACK_FRAME * 2);
	wmove(wins[WIN_MAIN], 1, 3);
	wprintw(wins[WIN_MAIN], "%-50s", " ");
	wrefresh(wins[WIN_MAIN]);
}

int						dump_stack(pid_t pid, struct user_regs_struct *old_regs,
								   struct user_regs_struct *regs,
								   WINDOW **wins, char clean)
{
	static void			*stack_frame = NULL;
	static char			*old_stack_frame = NULL;
	static unsigned		hl[MAX_STACK_FRAME * 2] = {};

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
