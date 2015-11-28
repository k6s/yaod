/*
** my_putchar.c for my_putchar.c in /home/k6/rendu/Piscine_C_J07
** 
** Made by Cano Chloe
** Login   <k6@epitech.net>
** 
** Started on  Wed Oct  8 15:14:22 2014 Cano Chloe
** Last update Sun Nov 16 20:22:06 2014 Cano Chloe
*/

#include <unistd.h>

int			my_putchar(char c)
{
  return (write(1, &c, 1));
}
