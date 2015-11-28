/*
1;2802;0c** env_builtins.c for minishell1 in /home/cano_c/rendu/UNIX/PSU_2014_minishell1
** 
** Made by Chloe Cano
** Login   <cano_c@epitech.net>
** 
** Started on  Mon Jan 19 16:05:04 2015
** Last update Mon Jan 19 17:07:06 2015 
*/
#include <minishell1.h>

int			my_unsetenv(t_term *s_term, char **av)
{
  if (av[1])
    del_envvar(s_term->environ, av[1]);
  return (0);
}

int			my_env(t_term *s_term, char UN **av)
{
  char			**environ;

  environ = s_term->environ;
  while (*environ)
    {
      my_putstr(*environ++);
      my_putchar('\n');
    }
  return (0);
}

char			**my_setenv(char **env, char *new, char ow)
{
  char			*var;
  char			**new_env;
  char			*var_name;

  if (new && (var_name = get_envvar_name(new)))
    {
      if ((var = my_getenv(env, var_name)))
	{
	  if (ow)
	    {
	      ow_env_var(env, var_name, new);
	      return (env);
	    }
	}
      else
	{
	  free(var_name);
	  if ((new_env = add_env_var(env, new)))
	    return (new_env);
	  else
	    return (NULL);
	}
    }
  else
    write(2, "usage: setenv ENVAR\n", 33);
  return (env);
}

int			my_setenvi(t_term *s_term, char **av)
{
  char			*var;
  char			overwrite;
  char			**new_env;
  char			*var_name;

  if (av[1] && (var_name = get_envvar_name(av[1])))
    {
      if ((var = my_getenv(s_term->environ, var_name)))
	{
	  my_putstr("environment variable already set, overwrite it (y/n) ? : ");
	  while (read(0, &overwrite, 1) < 1)
	    ;
	  if (overwrite == 'y')
	    return (ow_env_var(s_term->environ, var_name, av[1]));
	}
      else
	{
	  if ((new_env = add_env_var(s_term->environ, av[1])))
	    s_term->environ = new_env;
	  else
	    return (-1);
	}
      free(var_name);
    }
  else
    write(2, "usage: setenv ENVAR\n", 33);
  return (0);
}
