#include <see_stack.h>

static t_hbp				*hbp_by_id(t_hbp *hbp, ssize_t id)
{
	while (hbp && hbp->id != id)
		hbp = hbp->nxt;
	return (hbp);
}

int							hbp_delete(t_term *s_term, char **av)
{
	ssize_t					id;
	t_hbp					*to_rm;

	if (av[1])
	{
		id = strtol(av[1], NULL, 10);
		if (id != LONG_MAX && id != LONG_MIN)
		{
			if ((to_rm = hbp_by_id(id < 0 ? s_term->slave.d_hbp
								   : s_term->slave.e_hbp, id)))
			{
				hbp_unset(s_term->slave.pid, to_rm);
				hbp_remove(id < 0 ? &s_term->slave.d_hbp
						   : &s_term->slave.e_hbp, to_rm);
				free(to_rm);
				return (0);
			}
			wprintw(s_term->slave.wins[WIN_SH],
					"\t[X] Cannot remove watchpoint %d\n", id);
		}
	}	
	else
		wprintw(s_term->slave.wins[WIN_SH],
				"\t[X] Need a watchpoint id (hbl might be helpfull).\n");
	sh_refresh(s_term->slave.wins[WIN_SH], 0, 0);
	return (1);
}

int								hbp_lst(t_term *s_term, char UN **av)
{
	t_slave						*s_slave;
	t_hbp						*hbp;
	char						access;

	s_slave = &s_term->slave;
	hbp = s_slave->e_hbp;
	while (hbp)
	{
		switch (hbp->access)
		{
			case (HBP_ACCESS_ANY):
				break ;
				access = 'A';
			case (HBP_ACCESS_WRITE):
				access = 'W';
				break ;
			case (HBP_ACCESS_EXEC):
				access = 'X';
				break ;
			case (HBP_ACCESS_MEM):
				access = 'M';
				break ;
		}
		wprintw(s_slave->wins[WIN_SH], " [%ld] @ %p :: %c\n", hbp->id,
				(void *)hbp->addr, access);
		hbp = hbp->nxt;
	}
	sh_refresh(s_slave->wins[WIN_SH], 0, 0);
	return (0);
}

static int						set_access(char *access, char *av)
{
	if (*av && !*(av + 1))
	{
		switch (*av)
		{
		 case ('x'):
			 *access = HBP_ACCESS_EXEC;
			 return (0);
		 case ('a'):
			 *access = HBP_ACCESS_ANY;
			 return (0);
		 case ('m'):
			 *access = HBP_ACCESS_MEM;
			 return (0);
		 case ('w'):
			 *access = HBP_ACCESS_WRITE;
			 return (0);
		 default:
			 return (-1);
		}
	}
	return (-1);
}

static char					hbp_free_slot(t_hbp *hbp)
{
	char					regs[4] = {0};
	int						i;

	while (hbp)
	{
		if (hbp->regnum < 4)
			regs[hbp->regnum] = 1;
		hbp = hbp->nxt;
	}
	i = 0;
	while (i < 4 && regs[i])
		++i;
	return (i < 4 ? i : -1);
}

int							new_hbp(t_term *s_term, char **av)
{
	t_slave					*slave;
	t_hbp					*hbp;
	long					addr;
	char					access;
	char					len;
	char					scope;
	char					reg;

	slave = &s_term->slave;
	if (av[1] && av[2])
	{
		addr = strtol(av[1], NULL, 16);
		if (set_access(&access, av[2]))
		{
			wprintw(slave->wins[WIN_SH], "\t[X] access: 'a'-> ANY 'w' -> WRITE \
					'x' -> EXEC 'm' -> MEM (RDWR)\n");
			return (1);
		}
		len = 0;
		if (av[3] && ((len = atoi(av[3])) > 8 || len % 2 || len == 6))
		{
			wprintw(slave->wins[WIN_SH], "\t[X] len parameter must be 1 2 4 or\
					8\n");
			return (-1);
		}
		if (av[3] && av[4] && atoi(av[4]) != 0)
			scope = HBP_SCOPE_GLOBAL;
		else
			scope = HBP_SCOPE_LOCAL;
		if ((reg = hbp_free_slot(slave->e_hbp)) < 0)
		{
			wprintw(slave->wins[WIN_SH], "\t[X] No space left for another \
					watchpoint.\n");
			return (-1);
		}
		if (!(hbp = malloc(sizeof(*hbp))))
			return (-1);
		memset(hbp, 0, sizeof(*hbp));
		hbp->addr = addr;
		hbp->access = access;
		hbp->len = len;	
		hbp->scope = scope;
		hbp->regnum = reg;
		if (hbp_set(slave->pid, hbp))
		{
			free(hbp);
			return (-1);
		}
		hbp_append(&slave->e_hbp, hbp, 1);
		wprintw(slave->wins[WIN_SH], "\t[+] New watchpoint set at %p\n",
				hbp->addr);
		return (0);
	}
	else
		wprintw(slave->wins[WIN_SH], "args: addr access [len] [scope]\n");
	return (-1);
}
