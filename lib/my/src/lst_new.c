/*
** lst_new.c for lib in /home/cano_c/rendu/UNIX/PSU_2014_my_ls
** 
** Made by Chloe Cano
** Login   <cano_c@epitech.net>
** 
** Started on  Thu Nov 27 01:33:19 2014
** Last update Sun Nov 30 18:08:02 2014 Cano Chloe
*/

#include <my.h>

t_lst		*lst_new(t_blk *blk)
{
  t_lst		*new;

  if ((new = malloc(sizeof(*new))))
    {
      if (blk)
	{
	  new->blk.data = blk->data;
	  new->blk.len = blk->len;
	}
      else
	{
	  new->blk.data = NULL;
	  new->blk.len = 0;
	}
      new->next = NULL;
      new->prev = NULL;
    }
  return (new);
}
