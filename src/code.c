#include <see_stack.h>
#include <capstone/capstone.h>
#include <ptrace_get.h>
#include <math.h>

static int			get_ins(csh handle, unsigned long *rip, unsigned char *data,
							t_sbp *p, cs_insn *ins, int max_ins)
{
	int				n;
	int				n_ins;
	struct cs_insn	*tmp;
	int				j;

	n_ins = 0;
	n = -1;
	while (p)
	{
		if (!p->current && p->addr >= *rip && p->addr < *rip + 16)
			data[p->addr - *rip] = (p->saved & (unsigned)0xff);
		p = p->nxt;
	}
	if ((n = cs_disasm(handle, data, 16, *rip, 0, &tmp)) > 0)
	{
		j = 0;
		while (j < n && (j + n_ins) < max_ins)
		{
			memcpy(&(ins[j]), &(tmp[j]), sizeof(*ins));
			*rip += tmp[j].size;
			++j;
		}
		n_ins += n;
		free(tmp);
	}
	return (n_ins);
}

int					get_code(pid_t pid, unsigned long rip, int max_ins,
							 struct cs_insn **ins, t_sbp *sbp)
{
	int				n_ins;
	unsigned char	*data;
	int				n;
	csh				handle;
	unsigned long	s_rip;

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
	n = 1;
	while (n_ins < max_ins && n > 0)
	{
		if (!(data = get_data(pid, s_rip, 16)))
			break ;
		if ((n = get_ins(handle, &s_rip, data, sbp,
						 &((*ins)[n_ins]), max_ins - n_ins)) > 0)
			n_ins += n;
		free(data);
	}
	if (!n_ins || n < 0)
	{
		free(*ins);
		*ins = NULL;
		n_ins = -1;
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

int					dump_code(WINDOW *win, struct cs_insn *ins,
								int n_ins)
{
	int				i;

	wmove(win, 0, 0);
	wclrtobot(win);
	i = 0;
	while (i < n_ins)
	{
		if (!i)
			print_ins(win, ins + i, 1);
		else
			print_ins(win, ins + i, 0);
		++i;
	}
	code_refresh(win, 0, 0);
	return (0);
}
