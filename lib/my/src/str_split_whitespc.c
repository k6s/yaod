/*
** str_split_whitespc.c for lib in /home/cano_c/rendu/UNIX/PSU_2014_minishell1
** 
** Made by Chloe Cano
** Login   <cano_c@epitech.net>
** 
** Started on  Tue Jan 13 18:31:05 2015
** Last update Tue Jan 13 20:11:48 2015 
*/
#include <my.h>

static size_t	get_nb_str(char *s)
{
  size_t	nb_str;

  nb_str = 0;
  while (*s)
    {
      while (*s == ' ' || *s == '\t')
	++s;
      if (*s)
	++nb_str;
      while (*s && *s != ' ' && *s != '\t')
	++s;
    }
  return (nb_str);
}

char		**str_split_whitespc(char *s)
{
  char		*begin;
  char		**splitted;
  size_t       	nb_str[2];
  char		save;

  nb_str[0] = get_nb_str(s);
  nb_str[1] = 0;
  if (!(splitted = malloc(sizeof(*splitted) * (nb_str[0] + 1))))
    return (NULL);
  splitted[nb_str[0]] = 0;
  while (*s)
    {
      while (*s == ' ' || *s == '\t')
	++s;
      if (*s)
	{
	  begin = s;
	  while (*s != ' ' && *s != '\t' && *s)
	    ++s;
	  save = *s;
	  *s = 0;
	  if (!(splitted[(nb_str[1]++)] = my_strdup(begin)))
	    return (NULL);
	  if ((*s = save))
	    ++s;
	}
    }
  return (splitted);
}
