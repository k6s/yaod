/*
** my_cd.c for minishell1 in /home/cano_c/rendu/UNIX/PSU_2014_minishell1
** 
** Made by Chloe Cano
** Login   <cano_c@epitech.net>
** 
** Started on  Mon Jan 19 16:31:10 2015
** Last update Sun Jan 25 01:09:10 2015 
*/
#include <minishell1.h>

void		cat_envvar(char *var, char *varname, char *value)
{
  my_strncpy(var, varname, my_strlen(varname) + 1);
  my_strcat(var, "=");
  if (value)
    my_strcat(var, value);
}

char		my_getcwd(char *buff)
{
  return (readlink("/proc/self/cwd", buff, PATH_MAX));
}

int		my_cd(t_term *s_term, char **av)
{
  char		pwd[PATH_MAX + 10];
  char		cwd[PATH_MAX];

  if (!av[1] || access(av[1], F_OK))
    {
      write(2, "cannot cd to ", 13);
      write(2, av[1], my_strlen(av[1]));
      write(2, ": no such file or directory\n", 27);
      return (1);
    }
  if (!chdir(av[1]))
    {
      cat_envvar(pwd, "OLDPWD", my_getenv(s_term->environ, "PWD="));
      s_term->environ = my_setenv(s_term->environ, pwd, 1);
      my_bzero(cwd, PATH_MAX);
      my_getcwd(cwd);
      cat_envvar(pwd, "PWD", cwd);
      s_term->environ = my_setenv(s_term->environ, pwd, 1);
    }
  else
    {
      write(2, "cannot cd to ", 13);
      write(2, av[1], my_strlen(av[1]));
      write(2, "\n", 1);
      return (1);
    }
  return (0);
}
