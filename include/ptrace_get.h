#ifndef PTRACE_GET
# define PTRACE_GET

# include <sys/ptrace.h>
# include <sys/types.h>
# include <stdlib.h>
# include <errno.h>
# include <string.h>
# include <strings.h>

/*
 * get_slave_data.c
 * ================
 */
void			*get_rev_data(pid_t pid, long rbp, long rsp);
void			*get_data(pid_t pid, unsigned long addr, size_t len);
unsigned char	*get_str(pid_t pid, unsigned long addr);

#endif
