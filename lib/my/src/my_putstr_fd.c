/*
** my_putstr_fd.c for my_putstr.c in /home/k6/rendu/Piscine_C_J04
** 
** Made by Cano Chloe
** Login   <k6@epitech.net>
** 
** Started on  Fri Oct  3 09:22:47 2014 Cano Chloe
** Last update Sat Nov 29 22:56:52 2014 
*/

#include <my.h>

int		my_putstr_fd(char *str, int fd)
{
  char		*s;

  s = str;
  if (str)
    {
      while (*s)
	{
	  write(fd, s, 1);
	  ++s;
	}
    }
  return ((int)(s - str));
}
