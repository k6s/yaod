/*
 ** run_slave.c for dump_stack in /home/k6/null/dump_stack
 ** 
 ** Made by Chloe Cano
 ** Login   <k6@epitech.net>
 ** 
 ** Started on  Fri Jan 30 04:43:33 2015
 ** Last update Fri Jan 30 04:43:41 2015 
 */
#include <see_stack.h>
#include <elf_parse.h>
#include <errno.h>

void						sbp_append(t_sbp **r, t_sbp *n, char inc)
{
	t_sbp					*sbp;

	n->nxt = NULL;
	if ((sbp = *r))
	{
		while (sbp->nxt)
			sbp = sbp->nxt;
		sbp->nxt = n;
		n->prv = sbp;
		n->id = sbp->id + inc;
	}
	else
	{
		*r = n;
		n->id = inc;
	}
}

void						sbp_remove(t_sbp **r, t_sbp *sbp)
{
	t_sbp					*prv;

	if (*r)
	{
		if (*r == sbp)
			*r = (*r)->nxt;
		else
		{
			prv = *r;
			while (prv && prv->nxt != sbp)
				prv = prv->nxt;
			if (prv)
			{
				prv->nxt = sbp->nxt;
				if (sbp->nxt)
					sbp->nxt->prv = prv;
			}
		}
	}
	sbp->nxt = NULL;
	sbp->prv = NULL;
}

int							sbp_disable(t_slave *s_slave, t_sbp *sbp)
{
	s_slave->regs.rip = sbp->addr;
	ptrace(PTRACE_SETREGS, s_slave->pid, 0, &s_slave->regs);
	ptrace(PTRACE_POKETEXT, s_slave->pid, (void *)sbp->addr,
		   (void *)sbp->saved, 0);
	sbp_remove(&s_slave->e_sbp, sbp);
	if (sbp->current != SBP_STEP)
		sbp_append(&s_slave->d_sbp, sbp, -1);
	else
		free(sbp);
	return (0);
}

int							sbp_enable(t_slave *s_slave, t_sbp *sbp)
{
	errno = 0;
	sbp->saved = ptrace(PTRACE_PEEKTEXT, s_slave->pid, (void *)sbp->addr, 0);
	if (errno)
		return (-1);
	ptrace(PTRACE_POKETEXT, s_slave->pid, (void *)sbp->addr, (void *)
		   ((sbp->saved & ~(0xff)) | 0xcc), 0);
	sbp_remove(&s_slave->d_sbp, sbp);
	sbp_append(&s_slave->e_sbp, sbp, 1);
	return (0);
}

t_sbp						*sbp_hdl_step(t_slave *s_slave)
{
	t_sbp					*sbp;

	sbp = s_slave->e_sbp;
	while (sbp && (sbp->addr != s_slave->old_regs.rip
				   || sbp->current != SBP_STEP))
		sbp = sbp->nxt;
	if (sbp)
	{
		sbp_remove(&s_slave->e_sbp, sbp);
		sbp_remove(&s_slave->d_sbp, sbp);
		free(sbp);
		sbp = s_slave->e_sbp;
		while (sbp && (sbp->addr != s_slave->regs.rip || sbp->current))
			sbp = sbp->nxt;
		if (sbp)
		{
			s_slave->regs.rip += 1;
			return (sbp);
		}
	}
	return (NULL);
}

int							sbp_hdl(t_term *s_term)
{
	t_slave					*s_slave;
	t_sbp					*sbp;
	
	s_slave = &s_term->slave;
	if (!(sbp = sbp_hdl_step(&s_term->slave)))
	{
		sbp = s_slave->e_sbp;
		while (sbp && (sbp->current || sbp->addr != s_slave->regs.rip - 1))
			sbp = sbp->nxt;
	}
	if (sbp)
	{
		s_slave->regs.rip -= 1;
		ptrace(PTRACE_SETREGS, s_slave->pid, NULL, s_slave->regs);
		wprintw(s_slave->wins[WIN_SH], "\t[+] Reached breakpoint at %p\n",
				(void *)s_slave->regs.rip);
		refresh_exe_state(&s_term->slave, 0);
		wrefresh(s_slave->wins[WIN_SH]);
		sbp->current = SBP_CURRENT;
		return (sbp_disable(s_slave, sbp));
	/*	return (take_step(s_term, &step_slave)); */
	}
	return (1);
}

int							sbp_restore(t_slave *s_slave)
{
	t_sbp					*sbp;

	sbp = s_slave->d_sbp;
	while (sbp)
	{
		sbp_enable(s_slave, sbp);
		sbp->current = 0;
		sbp = sbp->nxt;
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
				wprintw(s_slave->wins[WIN_SH], "\t[X] Cannot set breakpoint at \
%p\n", (void *)sbp->addr);
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
