#ifndef HBP_
# define HBP_

# include <stdint.h>
# include <sys/ptrace.h>
# include <stddef.h>
# include <sys/types.h>
# include <sys/user.h>
# include <errno.h>

# define HBP_ACCESS_EXEC		0b00
# define HBP_ACCESS_WRITE		0b01
# define HBP_ACCESS_ANY			0b10
# define HBP_ACCESS_MEM			0b11

# define HBP_LEN_1				0b00
# define HBP_LEN_2				0b01
# define HBP_LEN_4				0b11
/* Not defined on every architecture */
# define HBP_LEN_8				0b10


# define HBP_SCOPE_LOCAL		0
# define HBP_SCOPE_GLOBAL		1

# define HBP_REG_DR0			0
# define HBP_REG_DR1			1
# define HBP_REG_DR2			2
# define HBP_REG_DR3			3

# define HBP_DR7_SCOPE(reg, scope)			((1 << (reg)) + (scope))
# define HBP_DR7_TYPE(reg, access)			(((access) << 15) + 4 * (reg))
# define HBP_DR7_LEN(reg, len)				(((len) << 18) + 4 * (reg))

# define HBP_SET_DR7(reg, scope, access, len)	(HBP_DR7_SCOPE(reg, scope) \
											 | HBP_DR7_TYPE(reg, access) \
											 | HBP_DR7_LEN(reg, len))

# define HBP_DR6_STEP(reg)					(((reg) >> 14) & 1)

typedef struct		s_hbp
{
	unsigned long	addr;
	unsigned char	access;
	unsigned char	len;
	unsigned char	scope;
	unsigned char	regnum;
	struct s_hbp	*nxt;
	struct s_hbp	*prv;
	int				id;
	char			current;
} t_hbp;

int					hbp_set(pid_t pid, t_hbp *hbp);
int					hbp_unset(pid_t pid, t_hbp *hbp);
void				hbp_append(t_hbp **r, t_hbp *n, char inc);
void				hbp_remove(t_hbp **r, t_hbp *hbp);

#endif
