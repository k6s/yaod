/*
** my_memalloc.c for libmy in /home/cano_c/my_select
** 
** Made by Chloe Cano
** Login   <cano_c@epitech.net>
** 
** Started on  Wed Jan  7 16:42:32 2015
** Last update Wed Jan  7 16:44:18 2015 
*/

#include <my.h>

void		*my_memalloc(size_t len)
{
  void		*new;

  if ((new = malloc(len)))
    my_bzero(new, len);
  return (new);
}
