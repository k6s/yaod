/*
** lst_rm.c for lib in /home/k6/rendu/PSU_2014_my_ls
** 
** Made by Cano Chloe
** Login   <k6@epitech.net>
** 
** Started on  Sun Nov 30 17:40:36 2014 Cano Chloe
** Last update Sun Nov 30 17:40:47 2014 Cano Chloe
*/

#include <my.h>

void			lst_rm(t_lst **lst, t_lst *p)
{
  if (p->prev)
    p->prev->next = p->next;
  else
    *lst = p->next;
  if (p->next)
    p->next->prev = p->prev;
  free(p);
}
