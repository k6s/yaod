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

int							sbp_disable(t_slave *s_slave, t_sbp *sbp)
{
	sbp_unset(s_slave->pid, &s_slave->regs, sbp);
	sbp_remove(&s_slave->e_sbp, sbp);
	if (sbp->current != BP_STEP)
		sbp_append(&s_slave->d_sbp, sbp, -1);
	else
		free(sbp);
	return (0);
}

int							sbp_enable(t_slave *s_slave, t_sbp *sbp)
{
	sbp_set(s_slave->pid, sbp);
	sbp_remove(&s_slave->d_sbp, sbp);
	sbp_append(&s_slave->e_sbp, sbp, 1);
	return (0);
}

int							hbp_disable(t_slave *s_slave, t_hbp *hbp)
{
	int						ret;

	ret = hbp_unset(s_slave->pid, hbp);
	hbp_remove(&s_slave->e_hbp, hbp);	
	hbp_append(&s_slave->d_hbp, hbp, -1);
	return (ret);
}

int							hbp_enable(t_slave *s_slave, t_hbp *hbp)
{
	int						ret;

	ret = hbp_set(s_slave->pid, hbp);
	hbp_remove(&s_slave->d_hbp, hbp);
	hbp_append(&s_slave->e_hbp, hbp, 1);
	return (ret);
}

t_sbp						*step_hdl(t_slave *s_slave)
{
	t_sbp					*sbp;

	if (HBP_DR6_STEP(get_debug_register(s_slave->regs.rip, 6)))
	{
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

int							hbp_hdl(t_slave *slave, t_hbp *hbp)
{
	while (hbp && hbp->addr != slave->regs.rip)
		hbp = hbp->nxt;
	if (hbp && !hbp->current)
	{
		hbp->current = BP_CURRENT;
		hbp_disable(slave, hbp);
		wprintw(slave->wins[WIN_SH], "\t[+] Interrupt by user set watchpoint \
@ %p\n", hbp->addr);
		return (0);
	}
	return (1);
}

int							sbp_hdl(t_slave *s_slave, t_sbp *sbp)
{
	if (!sbp)
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
		refresh_exe_state(s_slave, 0);
		wrefresh(s_slave->wins[WIN_SH]);
		sbp->current = BP_CURRENT;
		return (sbp_disable(s_slave, sbp));
	}
	return (1);
}

int							bp_hdl(t_term *s_term)
{
	t_slave					*s_slave;
	t_sbp					*sbp;
	int						ret;

	s_slave = &s_term->slave;
	ret = hbp_hdl(s_slave, s_term->slave.e_hbp);
	sbp = step_hdl(&s_term->slave);
	if (!sbp_hdl(s_slave, sbp))
		return (0);
	return (ret);
}
