/*
** my_stracmp.c for lib in /home/cano_c/rendu/UNIX/PSU_2014_my_ls
** 
** Made by Chloe Cano
** Login   <cano_c@epitech.net>
** 
** Started on  Sun Nov 30 16:08:26 2014
** Last update Sun Nov 30 18:00:15 2014 Cano Chloe
*/

#include <my.h>

static char	is_alnum(char c)
{
  if ((c <= 'z' && c >= 'a') || (c <= 'Z' && c >= 'A')
      || (c <= '9' && c >= '0'))
    return (1);
  return (0);
}

static void	init_stracmp(char **s1, char **s2)
{
  while (**s1 && **s2 == **s1)
    {
      ++(*s1);
      ++(*s2);
    }
}

static int	cmp_alpha(char *s1, char *s2, int s1_idx, int s2_idx)
{
  size_t	s1_len;
  size_t	s2_len;

  s1_len = my_strlen(s1);
  s2_len = my_strlen(s2);
  if (*s1 == '.' && *s2 != '.')
    return (-1);
  if (*s1 != '.' && *s2 == '.')
    return (1);
  if (s1_idx > s2_idx)
    return (1);
  if (s2_idx > s1_idx)
    return (-1);
  if (s1_len > s2_len)
    return (1);
  if (s1_len < s2_len)
    return (-1);
  return (0);
}

int		my_stracmp(char *s1, char *s2)
{
  size_t	s1_idx;
  size_t	s2_idx;
  int		ret;

  s1_idx = 0;
  s2_idx = 0;
  init_stracmp(&s1, &s2);
  if (!*s1)
    return (-1);
  if (!*s2)
    return (1);
  while (*(s1 + s1_idx) && !is_alnum(*(s1 + s1_idx)))
    ++s1_idx;
  while (*(s2 + s2_idx) && !is_alnum(*(s2 + s2_idx)))
    ++s2_idx;
  if (!(ret = my_strccmp(s1 + s1_idx, s2 + s2_idx)))
    {
      if ((ret = cmp_alpha(s1, s2, s1_idx, s2_idx)))
	return (ret);
    }
  else
    return (ret);
  return (my_strccmp(s1, s2));
}
