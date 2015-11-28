/*
** get_base_len.c for get_base_len.c in /home/k6/rendu/Piscine_C_J08
** 
** Made by Cano Chloe
** Login   <k6@epitech.net>
** 
** Started on  Wed Oct  8 17:23:46 2014 Cano Chloe
** Last update Mon Oct 20 22:15:28 2014 Cano Chloe
*/
#include <my.h>

int		get_base_len(char *base)
{
  char		chars[256];
  int		base_len;

  base_len = 0;
  my_bzero(chars, 256);
  if (!*base || !(*(base + 1)))
    return (0);
  while (*base && *base > 31 && *base < 127 && *base != '-' && *base != '+')
    {
      if (chars[(int)*base])
	return (0);
      chars[(int)*base] = 1;
      ++base_len;
      ++base;
    }
  if (*base)
    return (0);
  return (base_len);
}
