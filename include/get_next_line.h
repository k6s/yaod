/*
** get_next_line.h for get_next_line in ~/rendu/ELEM/CPE_2014_get_next_line
** 
** Made by Chloe Cano
** Login   <cano_c@epitech.net>
** 
** Started on  Mon Nov 17 11:13:18 2014
** Last update Sun Nov 23 23:21:11 2014 Cano Chloe
*/

#ifndef GET_NEXT_LINE_H_
# define GET_NEXT_LINE_H_

# include <stdlib.h>
# include <stddef.h>
# include <unistd.h>
# include <sys/types.h>

# define READ_LEN		10
# define EOL			'\n'

typedef struct s_gblk		t_gblk;
typedef struct s_glst		t_glst;

char		*get_next_line(const int fd);

struct		s_gblk
{
  char		data[READ_LEN];
  size_t	len;
};

struct		s_glst
{
  t_gblk		blk;
  t_glst		*next;
};

t_glst		**get_fd_tab(const int fd);
t_glst		*glst_memchr(t_glst *haystack, char needle, ptrdiff_t *off);
void		next_elem(t_glst **lst, t_glst *new, t_glst **tmp);

#endif /* !GET_NEXT_LINE_H_ */
