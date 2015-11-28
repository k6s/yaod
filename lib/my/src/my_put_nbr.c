/*
** my_put_nbr.c for my_put_nbr.c in /home/k6/rendu/test
** 
** Made by Cano Chloe
** Login   <k6@epitech.net>
** 
** Started on  Thu Oct  2 17:06:43 2014 Cano Chloe
** Last update Mon Oct 20 22:22:26 2014 Cano Chloe
*/

#include <my.h>

int			my_put_nbr(int n)
{
  if (n == INT_MIN)
    {
      my_put_nbr(INT_MIN / 10);
      my_put_nbr(INT_MIN % 10 * -1);
      return (0);
    }
  if (n < 0)
    {
      my_putchar('-');
      n *= -1;
    }
  if (n >= 10)
    my_put_nbr(n / 10);
  my_putchar(n % 10 + '0');
  return (0);
}
