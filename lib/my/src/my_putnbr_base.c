/*
** my_putnbr_base.c for my_putnbr_base.c in /home/k6/rendu/Piscine_C_J06
** 
** Made by Cano Chloe
** Login   <k6@epitech.net>
** 
** Started on  Mon Oct  6 16:15:02 2014 Cano Chloe
** Last update Mon Oct 20 22:34:19 2014 Cano Chloe
*/

#include <my.h>

int		my_putnbr_base(int nbr, char *base)
{
  int		base_len;

  if ((base_len = get_base_len(base)) < 2)
    return (0);
  if (nbr == INT_MIN)
    {
      my_putnbr_base(INT_MIN / base_len, base);
      my_putnbr_base(INT_MIN % base_len * -1, base);
      return (0);
    }
  if (nbr < 0)
    {
      my_putchar('-');
      nbr *= -1;
    }
  if (nbr >= base_len)
    my_putnbr_base(nbr / base_len, base);
  my_putchar(base[nbr % base_len]);
  return (0);
}
