/*
** sh_hist.c for minishell1 in /home/cano_c/rendu/UNIX/PSU_2014_minishell1
** 
** Made by Chloe Cano
** Login   <cano_c@epitech.net>
** 
** Started on  Sun Jan 25 01:17:36 2015
** Last update Sun Jan 25 01:45:33 2015 
*/
#include <minishell1.h>

int     init_hist(void)
{
  int		fd;

  if ((fd = open("~/.config/mysh/.mysh_hist", O_RDWR)) > -1)
    return (fd);
  if ((fd = open("~/.config/.mysh_hist", O_RDWR)) > -1)
    return (fd);
  if ((fd = open("~/.mysh/mysh_hist", O_RDWR)) > -1)
    return (fd);
  if ((fd = open("~/.mysh_hist", O_RDWR)) > -1)
    return (fd);
  if ((fd = open("~/.config/mysh/mysh_hist", O_CREAT)) > -1)
    return (fd);
  if ((fd = open("~/.config/mysh_hist", O_CREAT)) > -1)
    return (fd);
  if ((fd = open("~/.mysh/.mysh_hist", O_CREAT)) > -1)
    return (fd);
  return (open("~/.mysh_hist", O_CREAT) > -1);
}
