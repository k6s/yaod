#include <see_stack.h>

int							sbp_restore(t_slave *s_slave)
{
	t_sbp					*sbp;
	t_hbp					*hbp;

	sbp = s_slave->d_sbp;
	while (sbp)
	{
		sbp_enable(s_slave, sbp);
		sbp->current = 0;
		sbp = sbp->nxt;
	}
	hbp = s_slave->d_hbp;
	while (hbp)
	{
		hbp_enable(s_slave, hbp);
		hbp->current = 0;
		hbp = hbp->nxt;
	}
	return (0);
}

int							sbp_lst(t_term *s_term, char UN **av)
{
	t_slave					*s_slave;
	t_sbp					*sbp;

	s_slave = &s_term->slave;
	sbp = s_slave->e_sbp;
	wprintw(s_slave->wins[WIN_SH], "\t -= Enabled =-\n");
	while (sbp)
	{
		wprintw(s_slave->wins[WIN_SH], " [%ld] @ %p\n", sbp->id,
				(void *)sbp->addr);
		sbp = sbp->nxt;
	}
	sbp = s_slave->d_sbp;
	wprintw(s_slave->wins[WIN_SH], "\t -= Disabled =-\n");
	while (sbp)
	{
		wprintw(s_slave->wins[WIN_SH], " [%ld] @ %p\n", sbp->id,
				(void *)sbp->addr);
		sbp = sbp->nxt;
	}
	sh_refresh(s_slave->wins[WIN_SH], 0, 0);
	return (0);
}

t_sbp						*sbp_by_id(t_sbp *sbp, ssize_t id)
{
	while (sbp && sbp->id != id)
		sbp = sbp->nxt;
	return (sbp);
}

int							sbp_delete(t_term *s_term, char **av)
{
	ssize_t					id;
	t_sbp					*to_rm;

	if (av[1])
	{
		id = strtol(av[1], NULL, 10);
		if (id != LONG_MAX && id != LONG_MIN)
		{
			if ((to_rm = sbp_by_id(id < 0 ? s_term->slave.d_sbp
								   : s_term->slave.e_sbp, id)))
			{
				sbp_unset(s_term->slave.pid, &s_term->slave.regs, to_rm);
				sbp_remove(id < 0 ? &s_term->slave.d_sbp
						   : &s_term->slave.e_sbp, to_rm);
				free(to_rm);
				return (0);
			}
			wprintw(s_term->slave.wins[WIN_SH],
					"\t[X] Cannot remove breakpoint %d", id);
		}
	}	
	else
		wprintw(s_term->slave.wins[WIN_SH],
				"\t[X] Need a breakpoint id (bl might be helpfull).\n");
	sh_refresh(s_term->slave.wins[WIN_SH], 0, 0);
	return (1);
}

int							new_sbp(t_term *s_term, char **av)
{
	t_slave					*s_slave;
	t_sbp					*sbp;	
	int						ret;

	if (av[1])
	{
		s_slave = &s_term->slave;
		if (!(sbp = malloc(sizeof(*sbp))))
			return (-1);
		memset(sbp, 0, sizeof(*sbp));
		if ((sbp->addr = strtol(av[1], NULL, 16)) != LONG_MAX)
		{
			if ((ret = sbp_enable(s_slave, sbp)))
			{
				wprintw(s_slave->wins[WIN_SH], "\t[X] Cannot set breakpoint \
at %p\n", (void *)sbp->addr);
				free(sbp);
			}
			else
				wprintw(s_slave->wins[WIN_SH], "\t[+] New breakpoint at %p\n",
						(void *)sbp->addr);
			return (ret);
		}
	}
	waddstr(s_slave->wins[WIN_SH], "\t[X] Need an address to break at.\n");
	sh_refresh(s_term->slave.wins[WIN_SH], 0, 0);
	return (1);
}
