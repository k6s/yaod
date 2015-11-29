/*
 ** run_slave.c for dump_stack in /home/k6/null/dump_stack
 ** 
 ** Made by Chloe Cano
 ** Login   <k6@epitech.net>
 ** 
 ** Started on  Fri Jan 30 04:43:33 2015
 ** Last update Fri Jan 30 04:43:41 2015 
 */
#include <see_stack.h>
#include <elf_parse.h>

int					slave_exit(WINDOW *win, t_slave *slave)
{
	slave->end = 1;
	slave->pid = -1;
	sh_refresh(win, 0, 0);
	return (SLAVE_EXIT);
}

int				slave_status(int status, WINDOW *win, t_slave *slave)
{
	if (WIFEXITED(status))
	{
		wprintw(win, "Slave exited with status %d\n", WEXITSTATUS(status));
		return (slave_exit(win, slave));
	}
#ifdef WCOREDUMP
	if (WCOREDUMP(status))
	{
		wprintw(win, "Slave quitted (core dumped)\n");
		return (slave_exit(win, slave));
	}
#endif
	/*
	 * WIFSIGNALED is totally screwed up for some still obscure reasons.
	 * WIFSTOPPED behaves strangely too. Need a PTRACE_GETSIG call to
	 * handles signals properly.
	 */
	if (WIFSIGNALED(status) && WTERMSIG(status) != SIGURG)
	{
		wprintw(win, "Slave terminated by signal: %s\n",
				strsignal(WTERMSIG(status)));
		return (slave_exit(win, slave));
	}
	if (WIFSTOPPED(status) && WSTOPSIG(status) != SIGTRAP)
		wprintw(win, "Slave stopped by signal %s\n",
				strsignal(WSTOPSIG(status)));
	else
		return (SLAVE_BREAK);
	sh_refresh(win, 0, 0);
	return (0);
}

int                             ptrace_exec(char *path, char **cmd,
											char **environ, int fds, int fdm)
{
	pid_t                       pid;

	if (!(pid = fork()))
	{
		close(fdm);
		close(0);
		close(1);
		close(2);
		dup(fds);
		dup(fds);
		dup(fds);
		close(fds);
		if (ptrace(PTRACE_TRACEME, getpid(), NULL, NULL))
		{
			write(2, "Cannot attach to process\n", 25);
			exit(EXIT_FAILURE);
		}
		execve(path, cmd, environ);
		write(2, "Cannot execute command\n", 24);
		exit(EXIT_FAILURE);
	}
	close(fds);
	return (pid);
}

void							output(WINDOW *win, char *s, size_t len)
{
	size_t						i;

	i = 0;
	while (i < len)
	{
		if (s[i] == '\n' || s[i] == '\r')
		{
			wmove(win, getcury(win), getmaxx(win));
			wmove(win, getcury(win) + 1, 0);
		}
		else
			waddch(win, s[i]);
		if (i + 1 < len && s[i] == '\r' && s[i + 1] == '\n')
			++i;
		++i;
	} 
	wrefresh(win);
}



int								handle_pty(int fdm, WINDOW *win, int pid)
{
	fd_set						fd_in;
	ssize_t						r_len;
	char						buff[256];
	struct timeval				tv;
	int							i;
	int							x;
	int							y;

	x = 0;
	while (!x)
	{
		bzero(&tv, sizeof(tv));
		tv.tv_sec = 0;
		tv.tv_usec = 10;
		FD_ZERO(&fd_in);
		FD_SET(fdm, &fd_in);
		FD_SET(0, &fd_in);
		if (select(fdm + 1, &fd_in, NULL, NULL, &tv) < 0)
		{
			perror("Err on select()");
			waitpid(pid, &x, 0);
			return (x);
		}
		if (FD_ISSET(0, &fd_in))
		{
			sh_refresh(win, 0, 0);
			while ((r_len = read(0, buff, sizeof(buff))) > 0)
			{
				if (memchr(buff, '\x04', r_len))
				{
					write(fdm, "\n", 1);
					break ;
				}
				if (memchr(buff, '\x03', r_len))
				{
					kill(pid, SIGINT);
					break ;
				}
				output(win, buff, r_len);
				sh_refresh(win, 0, 0);
				write(fdm, buff, r_len);
				if (memchr(buff, '\r', r_len))
				{
					write(fdm, "\n", 1);
					break ;
				}
			}
			if (r_len < 0)
			{
				perror("Err reading input");
				waitpid(pid, &x, 0);
				return (x);
			}
		}  
		else if (FD_ISSET(fdm, &fd_in))
		{
			y = fcntl(fdm, F_GETFL, 0) | O_NONBLOCK;
			fcntl(fdm, F_SETFL, y);
			while ((r_len = read(fdm, buff, sizeof(buff) - 1)) > 0)
			{
				buff[r_len] = 0;
				output(win, buff, r_len);
			}
			y = fcntl(fdm, F_GETFL, 0) ^ O_NONBLOCK;
			fcntl(fdm, F_SETFL, y);
			if (r_len < 0)
			{
				perror("Err reading slave's output");
				waitpid(pid, &x, 0);
				return (x);
			}
		}
		if (waitpid(pid, &i, WNOHANG) != pid)
			x = 0;
		else
			x = 1;
	}
	return (i);
}

int								cont_slave(t_slave *slave)
{
	int							status;

	if (ptrace(PTRACE_CONT, slave->pid, NULL, NULL))
	{
		waitpid(slave->pid, &status, 0);
		return (slave_status(status, slave->wins[WIN_SH], slave));
	}
	status = handle_pty(slave->fdm, slave->wins[WIN_SH], slave->pid);
	return (slave_status(status, slave->wins[WIN_SH], slave));
}

int								step_slave(t_slave *slave)
{
	t_sbp						*sbp;
	int							status;

	if (!(sbp = malloc(sizeof(*sbp))))
		return (-1);
	sbp->addr = slave->regs.rip;
	sbp->current = SBP_STEP;
	sbp_append(&slave->e_sbp, sbp, 1);
	if (ptrace(PTRACE_SINGLESTEP, slave->pid, NULL, NULL))
	{
		waitpid(slave->pid, &status, 0);
		return (slave_status(status, slave->wins[WIN_SH], slave));
	}
	status = handle_pty(slave->fdm, slave->wins[WIN_SH], slave->pid);
	return (slave_status(status, slave->wins[WIN_SH], slave));
}

void					print_link_map(struct link_map *link_map)
{
	while (link_map)
	{
		fprintf(stderr, "addr: %lx name: %s\n", link_map->l_addr, link_map->l_name);
		link_map = link_map->l_next;
	}
}

char            refresh_exe_state(t_slave *s_slave, char sclean)
{
	if (s_slave->pid > -1)
	{
	if (!s_slave->elf)
		s_slave->elf = elf_get(s_slave->pid, s_slave->filename);
	if (s_slave->elf && s_slave->elf->dyn && !(s_slave->elf->link_map))
		s_slave->elf->link_map = elf_linkmap(s_slave->pid, s_slave->elf->dyn);
	if (!s_slave->elf->linked)
		elf_populate_dynsym(s_slave->pid, s_slave->elf->link_map,
							s_slave->elf->dynsym, s_slave->elf->dynstr,
							s_slave->elf->strsz);
	memcpy(&s_slave->old_regs, &s_slave->regs, sizeof(s_slave->old_regs));
	if (ptrace(PTRACE_GETREGS, s_slave->pid, NULL, &s_slave->regs))
	{
		perror("ptrace() cannot get regs\n");
		memcpy(&s_slave->regs, &s_slave->old_regs, sizeof(s_slave->regs));
		/*	return (-1); */
	}
	dump_regs(&s_slave->old_regs, &s_slave->regs, s_slave->wins, 1);
	wrefresh(s_slave->wins[WIN_REGS]);
	dump_stack(s_slave->pid, &s_slave->old_regs, &s_slave->regs, s_slave->wins,
			   sclean);
	wrefresh(s_slave->wins[WIN_REGS]);
	sh_refresh(s_slave->wins[WIN_SH], 0, 0);
	update_code(s_slave);
	}
	return (0);
}

static int		breaked_update(t_term *s_term)
{
	if (refresh_exe_state(&s_term->slave, 0) == -2)
		kill(s_term->slave.pid, SIGKILL);
	return (0);
}

int				take_step(t_term *s_term, int (*exe_nxt)(t_slave * ))
{
	int			status;

	status = exe_nxt(&s_term->slave);
	if (refresh_exe_state(&s_term->slave, 1) == -2)
	{
		wprintw(s_term->slave.wins[WIN_SH], "Slave killed: shit happened\n");
		kill(s_term->slave.pid, SIGKILL);
	}
	if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP && !sbp_hdl(s_term))
		return (breaked_update(s_term));
	if (WIFEXITED(status))
		endwin();
	sbp_restore(&s_term->slave);
	return (slave_status(status, s_term->slave.wins[WIN_SH], &s_term->slave));
}

int				step_prog(t_term *s_term, char UN **av)
{
	if (s_term->slave.pid > -1)
		return (take_step(s_term, &step_slave));
	else
		wprintw(s_term->slave.wins[WIN_SH], "No loaded executable.\n");
	return (0);
}

int				cont_loop(t_term *s_term, char UN **av)
{
	int			status;

	status = 0;
	if (s_term->slave.pid > -1)
	{
		status = 222;
		while (status == 222 || status == SLAVE_BREAK)
			status = take_step(s_term, &step_slave);
	}
	else
		wprintw(s_term->slave.wins[WIN_SH], "No loaded executable.\n");
	return (status);
}

int				run_loop(t_term *s_term, char UN **av)
{
	int			status;

	status = 0;
	if (s_term->slave.pid > -1)
	{
		status = 222;
		while (status != SLAVE_EXIT)
			status = take_step(s_term, &step_slave);
	}
	else
		wprintw(s_term->slave.wins[WIN_SH], "No loaded executable.\n");
	return (status);
}

int				blind_cont_prog(t_term *s_term, char UN **av)
{
	int			ret;

	fprintf(stderr, "blind_cont\n");
	if (s_term->slave.pid > -1)
	{
		if (s_term->slave.d_sbp)
		{
			if ((ret = step_slave(&s_term->slave)) == SLAVE_EXIT)
				return (SLAVE_EXIT);
			sbp_restore(&s_term->slave);
		}
		fprintf(stderr, "blind_cont return\n");
		return (take_step(s_term, &cont_slave));
	}
	else
		wprintw(s_term->slave.wins[WIN_SH], "No loaded executable.\n");
	return (0);
}

int						open_pty(t_slave *s_slave)
{
	struct termios		term;

	if ((s_slave->fdm = posix_openpt(O_RDWR)) < 0)
	{
		perror("Err on posix_openpt()");
		return (-1);
	}
	if (grantpt(s_slave->fdm))
	{
		perror("Err on grantpt()");
		return (-1);
	}
	if (unlockpt(s_slave->fdm))
	{
		perror("Err on unlockpt()");
		return (-1);
	}
	if ((s_slave->fds = open(ptsname(s_slave->fdm), O_RDWR)) < 0)
	{
		perror("Err on open() pty");
	}
	tcgetattr(s_slave->fds, &term);
	term.c_cc[VEOF] = '\x04';
	tcsetattr(s_slave->fds, TCSADRAIN, &term);
	return (0);
}

int				start_slave(char *path, char **cmd, char **environ,
							t_slave *s_slave)
{
	int			status;

	if (open_pty(s_slave))
		return (-1); 
	if ((s_slave->pid = ptrace_exec(path, cmd, environ, s_slave->fds,
									s_slave->fdm)) < 0)
		return (-1);
	status = handle_pty(s_slave->fdm, s_slave->wins[WIN_SH], s_slave->pid);
	wprintw(s_slave->wins[WIN_SH], "%s\n", ptsname(s_slave->fdm));
	s_slave->elf = NULL;
	s_slave->e_sbp = NULL;
	s_slave->d_sbp = NULL;
	bzero(&s_slave->regs, sizeof(s_slave->regs));
	bzero(&s_slave->old_regs, sizeof(s_slave->old_regs));
	dump_regs_name(s_slave->wins[WIN_REGS]);
	refresh_exe_state(s_slave, 1);
	wrefresh(s_slave->wins[WIN_REGS]);
	return (slave_status(status, s_slave->wins[WIN_SH], s_slave));
}
