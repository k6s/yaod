/*
** my_getnbr.c for my_getnbr.c in /home/k6/rendu/Piscine_C_J04
** 
** Made by Cano Chloe
** Login   <k6@epitech.net>
** 
** Started on  Fri Oct  3 09:21:15 2014 Cano Chloe
** Last update Mon Oct 20 22:31:42 2014 Cano Chloe
*/

#include <limits.h>

int			my_getnbr(char *str)
{
  int		res;
  char		sign;

  res = 0;
  sign = 1;
  while (*str == '+' || *str == '-')
    {
      if (*str == '-')
	sign *= -1;
      ++str;
    }
  while (*str && (*str >= '0' && *str <= '9'))
    {
      res *= 10;
      res += *str - '0';
      ++str;
      if (res < 0 && !((res == INT_MIN) && (sign == -1)))
	return (0);
    }
  if (res != INT_MIN && sign == -1)
    res *= (int)sign;
  return (res);
}
