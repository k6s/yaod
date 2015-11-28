/**
 * @file get_slave_data.c
 * @brief Get slave process memory content.
 * @author K6
 */
#include <see_stack.h>
#include <errno.h>
#include <elf.h>

void							*get_data(pid_t pid, unsigned long addr,
										  size_t len)
{
	unsigned long				*data;
	size_t						i;
	size_t						j;

	i = 0;
	j = 0;
	if ((data = malloc(len + len % sizeof(*data))))
	{
		while (i < len)
		{
			errno = 0;
			data[j++] = ptrace(PTRACE_PEEKTEXT, pid, addr + i, NULL);
			/* if (errno && data[j - 1] == ~((unsigned)0)) */
			if (errno)
			{
				perror("ptrace()");
				if (j == 1)
				{
					free(data);
					data = NULL;
				}
				return (data);
			}
			i += 8;
		}
	}
	return (data);
}

char							*get_str(pid_t pid, long addr)
{
	char						*s;
	long						*word;
	size_t						i;

	s = NULL;
	i = 0;
	word = NULL;
	while (!word || !memchr(word, 0, sizeof(*word)))
	{
		if (!(s = realloc(s, i + sizeof(*word))))
			return (NULL);
		if (!(word = get_data(pid, addr + i, sizeof(*word))))
			return (NULL);
		strncpy(s + i, (const char *)word, sizeof(*word));
		i += sizeof(*word);
	}
	return (s);
}

void							*get_rev_data(pid_t pid, long rbp, long rsp)
{
	long						*data;
	size_t						idx;

	if ((data = malloc(rbp - rsp + 32)))
	{
		idx = 0;
		bzero(data, rbp - rsp + 32);
		while ((off_t)(rbp - idx) >= rsp)
		{
			data[idx / sizeof(long)] = ptrace(PTRACE_PEEKTEXT, pid,
											  (void *)(rbp - idx), NULL);
			idx += sizeof(long);
		}
	}
	return (data);
}
