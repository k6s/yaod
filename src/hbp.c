#include <hbp.h>

int					set_debug_register(pid_t pid, int regnum, int val)
{
	errno = 0;
	ptrace(PTRACE_POKEUSER, pid, offsetof(struct user, u_debugreg)
			+ sizeof(((struct user *)0)->u_debugreg[0]) * regnum, val);
	if (errno)
		perror("POKEUSER");
	return (errno);
}

int					get_debug_register(pid_t pid, int regnum)
{
	long					dr_val;

	errno = 0;
	dr_val = ptrace(PTRACE_PEEKUSER, pid, offsetof(struct user, u_debugreg)
		   + sizeof(((struct user *)0)->u_debugreg[0]) * regnum, 0);
	if (errno)
		perror("PEEKUSER");
	return ((int)dr_val);
}

int							hbp_set(pid_t pid, t_hbp *hbp)
{
	int						dr7;

	errno = 0;
	dr7 = get_debug_register(pid, 7);
	dr7 |= HBP_SET_DR7(hbp->regnum, hbp->scope, hbp->access, hbp->len);
	printf("setting dr7 to: %x\n", dr7);
	set_debug_register(pid, 7, dr7);
	set_debug_register(pid, 6, 0);
	set_debug_register(pid, hbp->regnum, hbp->addr);
	dr7 = get_debug_register(pid, 7);
	printf("dr7 is %x and dr%d is %x\n", dr7, hbp->regnum, hbp->addr);
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
}
