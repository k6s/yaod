/*
** lst_insert.c for lib in /home/cano_c/rendu/UNIX/PSU_2014_my_ls
** 
** Made by Chloe Cano
** Login   <cano_c@epitech.net>
** 
** Started on  Thu Nov 27 01:33:40 2014
** Last update Sun Nov 30 18:05:45 2014 Cano Chloe
*/

#include <my.h>

static void	insert_before(t_lst **lst, t_lst *p_lst, t_lst *new)
{
  new->next = p_lst;
  new->prev = p_lst->prev;
  if (new->prev)
    new->prev->next = new;
  p_lst->prev = new;
  if (p_lst == *lst)
    *lst = new;
}

static void	insert_after(t_lst *p_lst, t_lst *new)
{
  new->next = p_lst->next;
  new->prev = p_lst;
  if (new->next)
    new->next->prev = p_lst;
  p_lst->next = new;
}

int		lst_insert(t_lst **lst, t_blk *blk, int (*cmp)())
{
  t_lst		*p_lst;
  t_lst		*new;
  int		rcmp;

  if ((p_lst = *lst))
    {
      while ((rcmp = (*cmp)(blk, &p_lst->blk, blk->len)) > 0
	     && p_lst->next)
	p_lst = p_lst->next;
      if ((new = lst_new(blk)))
	{
	  if (rcmp < 0)
	    insert_before(lst, p_lst, new);
	  else
	    insert_after(p_lst, new);
	  return (1);
	}
    }
  else if ((new = lst_new(blk)))
    {
      *lst = new;
      return (1);
    }
  return (-1);
}
