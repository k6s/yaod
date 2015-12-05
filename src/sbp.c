#include <sbp.h>

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

int							sbp_set(pid_t pid, t_sbp *sbp)
{
	errno = 0;
	sbp->saved = ptrace(PTRACE_PEEKTEXT, pid, (void *)sbp->addr, 0);
	if (errno)
		return (-1);
	ptrace(PTRACE_POKETEXT, pid, (void *)sbp->addr, (void *)
		   ((sbp->saved & ~(0xff)) | 0xcc), 0);
	return (errno);
}

int							sbp_unset(pid_t pid, struct user_regs_struct *regs,
									t_sbp *sbp)
{
	regs->rip = sbp->addr;
	errno = 0;
	ptrace(PTRACE_SETREGS, pid, 0, regs);
	if (errno)
		return (errno);
	ptrace(PTRACE_POKETEXT, pid, (void *)sbp->addr,
		   (void *)sbp->saved, 0);
	return (errno);
}
