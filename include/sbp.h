#ifndef SBP_H_
# define SBP_H_

# define BP_STEP			2
# define BP_CURRENT			1

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <stdlib.h>
#include <errno.h>

typedef struct s_sbp		t_sbp;	/*! @brief typedef to @ref s_sbreak */

/*!
 * @brief Software breakpoint.
 */
struct						s_sbp
{
	unsigned long			addr;		/*!< @brief breakpoint address */
	unsigned long			saved;		/*!< @brief saved data to restore */
	char					current;	/*!< @brief Disable flag */
	t_sbp					*nxt;		/*!< @brief pointer to next bp */
	t_sbp					*prv;		/*!< @brief pointer to previous bp */
	ssize_t					id;			/*!< @brief Breakpoint ID */
};

void						sbp_append(t_sbp **r, t_sbp *n, char inc);
void						sbp_remove(t_sbp **r, t_sbp *sbp);
int							sbp_set(pid_t pid, t_sbp *sbp);
int							sbp_unset(pid_t pid, struct user_regs_struct *regs,
									t_sbp *sbp);

#endif
