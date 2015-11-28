/*
 ** get_next_line_2.c for fdf in /home/cano_c/rendu/IGRAPH/re
 ** 
 ** Made by Chloe Cano
 ** Login   <cano_c@epitech.net>
 ** 
 ** Started on  Sun Nov 23 22:49:44 2014
 ** Last update Sun Nov 23 22:52:05 2014 
 */

#include <get_next_line.h>

static t_glst		**init_fd_tab(t_glst **s_lst, const int fd)
{
	ssize_t	idx;

	idx = -1;
	if (!s_lst)
	{
		if ((s_lst = malloc(sizeof(*s_lst) * (fd + 2))))
		{
			while (++idx <= fd)
				s_lst[idx] = NULL;
			s_lst[idx] = (void *)-1;
		}
	}
	return (s_lst);
}

t_glst		**get_fd_tab(const int fd)
{
	static t_glst	**s_lst;
	ssize_t	idx;
	t_glst		**tmp;

	if (!(s_lst = init_fd_tab(s_lst, fd)))
		return (NULL);
	idx = 0;
	while (s_lst[idx] != (void *)-1 && idx <= fd)
		++idx;
	if (idx < fd + 1 && (idx = -1))
	{
		tmp = s_lst;
		if (!(s_lst = malloc(sizeof(*s_lst) * (fd + 2))))
			return (NULL);
		while (tmp[++idx] != (void *)-1)
			s_lst[idx] = tmp[idx];
		while (idx <= fd)
			s_lst[idx++] = NULL;
		s_lst[idx] = (void *)-1;
		free(tmp);
	}
	return (s_lst);
}

t_glst		*glst_memchr(t_glst *haystack, char needle, ptrdiff_t *off)
{
	char		*p;

	while (haystack)
	{
		p = haystack->blk.data;
		while (p - haystack->blk.data < (unsigned)haystack->blk.len)
		{
			if (*p == needle)
			{
				*off = p - haystack->blk.data;
				return (haystack);
			}
			++p;
		}
		haystack = haystack->next;
	}
	return (NULL);
}

void		next_elem(t_glst **lst, t_glst *new, t_glst **tmp)
{
	if (!(*lst))
	{
		*lst = new;
		*tmp = *lst;
	}
	else
	{
		(*tmp)->next = new;
		*tmp = (*tmp)->next;
	}
}
