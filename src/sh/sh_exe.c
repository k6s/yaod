/*
 ** sh_exe.c for minishell1 in /home/cano_c/rendu/UNIX/PSU_2014_minishell1
 ** 
 ** Made by Chloe Cano
 ** Login   <cano_c@epitech.net>
 ** 
 ** Started on  Sun Jan 25 03:24:19 2015
 ** Last update Sun Jan 25 03:24:42 2015 
 */
#include <minishell1.h>
#include <see_stack.h>

char    		exe_builtin(t_prog *prog, char **av, t_term *s_term)
{
	char  ret;

	ret = (*prog->builtfp)(s_term, av);
	free_strtab(av);
	return ((char)ret);
}

t_term			*static_term(t_term *term)
{
	static t_term		*s_term = NULL;

	if (term)
		s_term = term;
	return (s_term);
}

char    		exe_cmd(t_prog *s_prog, char **av, char **environ,
					   	WINDOW **wins)
{
	size_t		pathlen;
	char			pathname[NAME_MAX + PATH_MAX];
	pid_t			pid;
	int			status;
	int           fds[2];

	if (pipe(fds))
		return (-1);
	pathlen = my_strlen(s_prog->path);
	my_strncpy(pathname, s_prog->path, PATH_MAX);
	if (pathname[pathlen] != '/')
		pathname[(pathlen++)] = '/';
	my_strncpy(pathname + pathlen, s_prog->name, NAME_MAX);
	pathname[NAME_MAX + PATH_MAX - 1] = 0;
	if ((pid = fork()) > -1)
	{
		if (!pid)
		{
			/* restore_termcap(tsave); */
			close(fds[0]);
			if ((dup2(fds[1], 1)) < 0)
				exit(EXIT_FAILURE);
			execve(pathname, av, environ);
			exit(EXIT_FAILURE);
		}
		else
		{
			close(fds[1]);
			my_bzero(pathname, sizeof(pathname));
			while (read(fds[0], pathname, NAME_MAX + PATH_MAX - 1) > 0)
			{
				pathname[NAME_MAX + PATH_MAX - 1] = 0;
				waddstr(wins[WIN_SH], pathname);
				wrefresh(wins[WIN_SH]);
			}
			waitpid(pid, &status, 0);
			free_strtab(av);
			return (WEXITSTATUS(status));
		}
	}
	free_strtab(av);
	waddstr(wins[WIN_SH], "err: cannot fork process\n");
	return (-2);
}
