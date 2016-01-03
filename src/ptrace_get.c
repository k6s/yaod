/**
 * @file get_slave_data.c
 * @brief Get slave process memory content.
 * @author K6
 */
#include <ptrace_get.h>

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
			if (errno)
			{
				free(data);
				return (NULL);
			}
			i += 8;
		}
	}
	return (data);
}

u_char							*get_str(pid_t pid, unsigned long addr)
{
	u_char						*s;
	long						*word;
	size_t						i;

	s = NULL;
	i = 0;
	word = NULL;
	while (!word || !memchr(word, 0, sizeof(*word)))
	{
		if (!(s = realloc(s, i + sizeof(*word))))
			return (NULL);
		free(word);
		if (!(word = get_data(pid, addr + i, sizeof(*word))))
		{
			free(s);
			return (NULL);
		}
		strncpy((char *)(s + i), (const char *)word, sizeof(*word));
		i += sizeof(*word);
	}
	free(word);
	return (s);
}

void							*get_rev_data(pid_t pid, long rbp, long rsp)
{
	long						*data;
	size_t						idx;

	if ((data = malloc(rbp - rsp + 32)))
	{
		idx = 0;
		memset(data, 0, rbp - rsp + 32);
		while ((off_t)(rbp - idx) >= rsp)
		{
			data[idx / sizeof(long)] = ptrace(PTRACE_PEEKTEXT, pid,
											  (void *)(rbp - idx), NULL);
			idx += sizeof(long);
		}
	}
	return (data);
}
