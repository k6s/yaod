/*
** sh_env.c for minishell1 in /home/cano_c/rendu/UNIX/PSU_2014_minishell1
** 
** Made by Chloe Cano
** Login   <cano_c@epitech.net>
** 
** Started on  Mon Jan 19 16:14:43 2015 
** Last update Mon Jan 19 16:14:57 2015 
*/
#include <minishell1.h>

void			del_envvar(char **environ, char *target)
{
  char			**next;

  while (*environ && my_strncmp(*environ, target, my_strlen(target)))
    ++environ;
  next = environ + 1;
  while (*environ)
    {
      *environ = *next;
      environ = next++;
    }
}

char			*get_envvar_name(char *s)
{
  char			*sp;
  char			*varname;
  char			save;

  varname = NULL;
  sp = s;
  while (*sp && *sp != '=')
    ++sp;
  if (*sp)
    {
      save = *(++sp);
      *sp = 0;
      varname = my_strdup(s);
      *sp = save;
    }
  return (varname);
}

int			ow_env_var(char **environ, char *var_name, char *new)
{
  while (*environ && my_strncmp(*environ, var_name, my_strlen(var_name)))
    ++environ;
  if (*environ)
    {
      free(*environ);
      if (!(*environ = my_strdup(new)))
	return (-1);
    }
  return (0);
}

char			**add_env_var(char **environ, char *new)
{
  char			**p_env;
  char			**new_env;

  p_env = environ;
  while (*p_env)
    ++p_env;
  if (!(new_env = malloc(sizeof(*new_env) * (p_env - environ + 2))))
    return (NULL);
  new_env[p_env - environ + 1] = 0;
  if (!(new_env[p_env - environ] = my_strdup(new)))
    {
      free(new_env);
      return (NULL);
    }
  while (--p_env >= environ)
    new_env[p_env - environ] = environ[p_env - environ];
  free(environ);
  return (new_env);
}


char			get_environ(t_term *s_term, char **environ)
{
  char			**p_env;
  char			**c_env;

  p_env = environ;
  while (*p_env)
    ++p_env;
  if (p_env - environ < 2)
    return (-1);
  if (!(c_env = malloc(sizeof(*c_env) * (p_env - environ + 1))))
    return (-1);
  c_env[(p_env) - environ] = 0;
  while (--p_env >= environ)
    if (!(c_env[p_env - environ] = my_strdup(environ[p_env - environ]))
	&& my_strlen(environ[p_env - environ]))
      return (-1);
  s_term->environ = c_env;
  return (0);
}
