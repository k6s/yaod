/*
** blk_new.c for lib in /home/cano_c/rendu/UNIX/PSU_2014_my_ls
** 
** Made by Chloe Cano
** Login   <cano_c@epitech.net>
** 
** Started on  Thu Nov 27 01:32:54 2014
** Last update Sun Nov 30 18:07:45 2014 Cano Chloe
*/

#include <my.h>

t_blk		*blk_new(size_t data_len)
{
  t_blk		*blk;

  if ((blk = malloc(sizeof(*blk))))
    {
      blk->len = 0;
      if (data_len)
	{
	  if (!(blk->data = malloc(data_len)))
	    {
	      free(blk);
	      blk = NULL;
	    }
	  else
	    blk->len = data_len;
	}
      else
	blk->data = NULL;
    }
  return (blk);
}
