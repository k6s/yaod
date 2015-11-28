/*
** my_putendl.c for libmy in /home/cano_c/my_select
** 
** Made by Chloe Cano
** Login   <cano_c@epitech.net>
** 
** Started on  Wed Jan  7 16:44:36 2015
** Last update Wed Jan  7 16:46:32 2015 
*/

#include <my.h>

int		my_putendl(char *s)
{
  char		*sp;
  int		ret;

  sp = s;
  while (*sp)
    ++sp;
  ret = write(1, s, my_strlen(s));
  ret += write(1, "\n", 1);
  return (ret);
}
