/*
** my_strccmp.c for my_strcmp.c in /home/k6/rendu/Piscine_C_J06
** 
** Made by Cano Chloe
** Login   <k6@epitech.net>
** 
** Started on  Mon Oct  6 13:10:34 2014 Cano Chloe
** Last update Sun Nov 30 18:08:36 2014 Cano Chloe
*/

#include <my.h>

int		my_strccmp(char *s1, char *s2)
{
  int		ret;
  char		c1;
  char		c2;

  c1 = *s1 >= 'A' && *s1 <= 'Z' ? *s1 + 'a' - 'A' : *s1;
  c2 = *s2 >= 'A' && *s2 <= 'Z' ? *s2 + 'a' - 'A' : *s2;
  if (c1 == c2 && c1 && c2)
    return (my_strcmp(++s1, ++s2));
  ret = c1 - c2 > 0 ? 1 : c1 - c2;
  ret = ret < 0 ? -1 : ret;
  return (ret);
}
