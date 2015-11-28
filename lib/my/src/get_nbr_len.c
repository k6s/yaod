/*
** get_nbr_len.c for lib in /home/cano_c/rendu/UNIX/PSU_2014_my_ls
** 
** Made by Chloe Cano
** Login   <cano_c@epitech.net>
** 
** Started on  Sat Nov 29 21:04:08 2014
** Last update Sat Nov 29 21:06:20 2014 
*/

#include <my.h>

size_t			get_nbr_len(int n)
{
  int			nlen;

  nlen = 0;
  if (n > 9)
    nlen += get_nbr_len(n / 10);
  return (nlen + 1);
}
