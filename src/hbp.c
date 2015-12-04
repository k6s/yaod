#include <hbp.h>

void						hbp_remove(t_hbp **r, t_hbp *hbp)
{
	t_hbp					*prv;

	if (*r)
	{
		if (*r == hbp)
			*r = (*r)->nxt;
		else
		{
			prv = *r;
			while (prv && prv->nxt != hbp)
				prv = prv->nxt;
			if (prv)
			{
				prv->nxt = hbp->nxt;
				if (hbp->nxt)
					hbp->nxt->prv = prv;
			}
		}
	}
	hbp->nxt = NULL;
	hbp->prv = NULL;
}

void						hbp_append(t_hbp **r, t_hbp *n, char inc)
{
	t_hbp					*hbp;

	n->nxt = NULL;
	if ((hbp = *r))
	{
		while (hbp->nxt)
			hbp = hbp->nxt;
		hbp->nxt = n;
		n->prv = hbp;
		n->id = hbp->id + inc;
	}
	else
	{
		*r = n;
		n->id = inc;
	}
}

int					set_debug_register(pid_t pid, int regnum, long val)
{
	errno = 0;
	ptrace(PTRACE_POKEUSER, pid, offsetof(struct user, u_debugreg)
			+ sizeof(((struct user *)0)->u_debugreg[0]) * regnum, val);
	return (errno);
}

int					get_debug_register(pid_t pid, int regnum)
{
	long					dr_val;

	errno = 0;
	dr_val = ptrace(PTRACE_PEEKUSER, pid, offsetof(struct user, u_debugreg)
		   + sizeof(((struct user *)0)->u_debugreg[0]) * regnum, 0);
	return ((int)dr_val);
}

int							hbp_set(pid_t pid, t_hbp *hbp)
{
	int						dr7;

	errno = 0;
	dr7 = get_debug_register(pid, 7);
	dr7 |= HBP_SET_DR7(hbp->regnum, hbp->scope, hbp->access, hbp->len);
	set_debug_register(pid, 7, dr7);
	set_debug_register(pid, 6, 0);
	set_debug_register(pid, hbp->regnum, hbp->addr);
	dr7 = get_debug_register(pid, 7);
	return (errno);
}

int							hbp_unset(int pid, t_hbp *hbp)
{
	int						dr7;

	errno = 0;
	dr7 = get_debug_register(pid, 7);
	if (errno)
		return (-1);
	dr7 &= ~(HBP_SET_DR7(hbp->regnum, hbp->scope, hbp->access, hbp->len));
	set_debug_register(pid, 7, dr7);
	return (errno);
}
