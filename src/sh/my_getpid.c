/*
** my_getpid.c for minishell1 in /home/cano_c/rendu/UNIX/PSU_2014_minishell1
** 
** Made by Chloe Cano
** Login   <cano_c@epitech.net>
** 
** Started on  Tue Jan 20 18:58:34 2015
** Last update Tue Jan 20 19:09:30 2015 
*/
#include <minishell1.h>

pid_t		my_getpid(void)
{
  int		fd;
  pid_t		pid;
  char		*line;
  char		*pline;

  pid = -1;
  if ((fd = open("/proc/self/status", O_RDONLY)))
  {
    while ((line = get_next_line(fd)))
      {
	if (!my_strncmp("Pid:", line, 4))
	  {
	    pline = line + 4;
	    while (*pline == ' ' || *pline == '\t')
	      ++pline;
	    pid = my_getnbr(pline);
	  }
	free(line);
      }
    close(fd);
  }
  return (pid);
}
