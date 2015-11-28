/*
** my_exit.c for minishell1 in /home/cano_c/rendu/UNIX/PSU_2014_minishell1
** 
** Made by Chloe Cano
** Login   <cano_c@epitech.net>
** 
** Started on  Mon Jan 19 16:02:35 2015
** Last update Mon Jan 19 16:21:39 2015 
*/
#include <minishell1.h>

int			my_exit(t_term *s_term, char UN **av)
{
  kill(s_term->pid, SIGTERM);
  return (0);
}
