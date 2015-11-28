#include <see_stack.h>
#include <capstone/capstone.h>
#include <ptrace_get.h>
#include <math.h>

int					get_code_bef(pid_t pid, unsigned long rip, int max_ins,
								 struct cs_insn **ins, t_sbp *sbp)
{
	int				n_ins;
	int				n;
	struct cs_insn	*t;
	unsigned long	rrip;

	n_ins = max_ins;
	if (!(*ins = malloc(sizeof(**ins) * max_ins)))
		return (-1);
	while (n_ins)
	{
		rrip = rip - sizeof((*ins)->bytes);
		if ((n = get_code(pid, rrip, sizeof((*ins)->bytes), &t, sbp)) < 1)
			return (max_ins - n_ins);
		rip -= (&t[n - 1])->size;
		--n_ins;
		memcpy(&((*ins)[n_ins]), t + n - 1, sizeof(*t));
		free(t);
	}
	return (max_ins - n_ins);
}

int					get_code(pid_t pid, unsigned long rip, int max_ins,
							 struct cs_insn **ins, t_sbp *sbp)
{
	int				n_ins;
	unsigned char	*data;
	struct cs_insn	*tmp;
	int				n;
	csh				handle;
	unsigned long	s_rip;
	int				j;
	t_sbp			*p;


	*ins = NULL;
	if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK)
		return (-1);
	if (!(*ins = malloc(sizeof(**ins) * max_ins)))
	{
		cs_close(&handle);
		return (-1);
	}
	n_ins = 0;
	s_rip = rip;
	while (n_ins < max_ins)
	{
		if (!(data = get_data(pid, rip, 16)))
			break ;
		p = sbp;
		while (p)
		{
			if (!p->current && p->addr >= rip && p->addr < rip + 16)
				data[p->addr - rip] = (p->saved & (unsigned)0xff);
			p = p->nxt;
		}
		if ((n = cs_disasm(handle, data, 16, s_rip, 0, &tmp)) > 0)
		{
			j = 0;
			while (j < n && (j + n_ins) < max_ins)
			{
				memcpy(&((*ins)[n_ins + j]), &(tmp[j]), sizeof(**ins));
				rip += tmp[j].size;
				++j;
			}
			n_ins += n;
			free(tmp);
		}
		else
		{
			free(data);
			break ;
		}
		free(data);
	}
	if (!n_ins)
	{
		free(*ins);
		*ins = NULL;
	}
	cs_close(&handle);
	return (n_ins < max_ins ? n_ins : max_ins);
}

void				print_ins(WINDOW *win, struct cs_insn *ins, char bold)
{
	int				ib;

	if (bold)
		wattrset(win, A_BOLD);
	wattron(win, COLOR_PAIR(3));
	wprintw(win, "%p\n   ", ins->address);
	wattron(win, COLOR_PAIR(1));
	ib = 0;
	while (ib < (8 < ins->size ? 8 : ins->size))
	{
		wprintw(win, "%02x ", ins->bytes[ib]);
		++ib;
	}
	while (ib++ < 8)
		wprintw(win, "   ");
	wattron(win, COLOR_PAIR(4));
	wprintw(win, " %-8s  ", ins->mnemonic,
			ins->op_str);
	wattron(win, COLOR_PAIR(1));
	wprintw(win, "%s\n", ins->op_str);
	wattroff(win, A_BOLD);
}

int					update_code(t_slave *s_slave)
{
	struct cs_insn	*ins;
	int				n_ins;
	int				i;
	unsigned long	rrip;

	rrip = s_slave->regs.rip;
	/*
	if ((n_ins = get_code_bef(s_slave->pid, s_slave->regs.rip, 4, &ins,
						  s_slave->e_sbp)) > 0)
	{
		i = 0 + 5 - n_ins;
		while (i < n_ins)
		{
			print_ins(s_slave->wins[WIN_CODE], ins + i, 0);
			++i;
		}
		code_refresh(s_slave->wins[WIN_CODE], 0, 0);
	}
	*/
	wmove(s_slave->wins[WIN_CODE], 0, 0);
	wclrtobot(s_slave->wins[WIN_CODE]);
	if ((n_ins = get_code(s_slave->pid, rrip, 10, &ins,
						  s_slave->e_sbp)) > 0)
	{
		i = 0;
		while (i < n_ins)
		{
			if (!i)
				print_ins(s_slave->wins[WIN_CODE], ins + i, 1);
			else
				print_ins(s_slave->wins[WIN_CODE], ins + i, 0);
			++i;
		}
		code_refresh(s_slave->wins[WIN_CODE], 0, 0);
	}
	free(ins);
	return (0);
}
