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
	else if (WIFSTOPPED(status))
		return (SLAVE_BREAK);
	else if (WIFEXITED(status))
	{
		wprintw(win, "Slave exited with status %d\n", WEXITSTATUS(status));
		return (slave_exit(win, slave));
	}
#ifdef WCOREDUMP
	else if (WCOREDUMP(status))
	{
		wprintw(win, "Slave quitted (core dumped)\n");
		return (slave_exit(win, slave));
	}
#endif
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
		exit(EXIT_FAILURE);
	}
	close(fds);
	return (pid);
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
	return (status);
}

int								step_slave(t_slave *slave)
{
	int							status;

	if (ptrace(PTRACE_SINGLESTEP, slave->pid, NULL, NULL))
	{
		waitpid(slave->pid, &status, 0);
		return (slave_status(status, slave->wins[WIN_SH], slave));
	}
	status = handle_pty(slave->fdm, slave->wins[WIN_SH], slave->pid);
	return (status);
}

void			update_elf(t_slave *s_slave)
{
	size_t		i;

	if (!s_slave->elf)
		s_slave->elf = elf_get(s_slave->pid, s_slave->filename);
	if (s_slave->elf && s_slave->elf->dyn && !(s_slave->elf->link_map))
	{
		i = 0;
		while (s_slave->elf->dyn[i] && s_slave->elf->dyn[i]->d_tag != DT_PLTGOT)
			++i;
		if (s_slave->elf->dyn[i])
			s_slave->elf->link_map
				= elf_linkmap(s_slave->pid, s_slave->elf->dyn[i]);
	}
	if (!s_slave->elf->linked)
		elf_populate_dynsym(s_slave->pid, s_slave->elf->link_map,
							s_slave->elf->dynsym, s_slave->elf->dynstr,
							s_slave->elf->strsz);
}

int				update_func(pid_t pid, struct user_regs_struct *regs,
							struct user_regs_struct *old_regs,
							t_elf *elf, t_fnt **fnt_lst)
{
	t_fnt		*fnt;

	if (!*fnt_lst || !(*fnt_lst)->sym || (*fnt_lst && (*fnt_lst)->sym
				 && regs && (regs->rip < (*fnt_lst)->sym->st_value
					 || regs->rip > (*fnt_lst)->end)))
	{
			if (!(fnt = fnt_new(pid, elf, regs->rip)))
				return (-1);
			if (!fnt || (!fnt->name && !(fnt->name = strdup("???"))))
				return (-1);
			if (fnt->sym && fnt->sym->st_size)
				fnt->end = fnt->sym->st_value + fnt->sym->st_size;
			free(*fnt_lst);
			*fnt_lst = fnt;
	}
	return (0);
}

char            refresh_exe_state(t_slave *s_slave, char sclean)
{
	if (s_slave->pid > -1)
	{
		update_elf(s_slave);
		memcpy(&s_slave->old_regs, &s_slave->regs, sizeof(s_slave->old_regs));
		if (ptrace(PTRACE_GETREGS, s_slave->pid, NULL, &s_slave->regs))
		{
			memcpy(&s_slave->regs, &s_slave->old_regs, sizeof(s_slave->regs));
		/*	return (-1); */
		}
		dump_regs(&s_slave->old_regs, &s_slave->regs, s_slave->wins, 1);
		wrefresh(s_slave->wins[WIN_REGS]);
		if (!update_func(s_slave->pid, &s_slave->regs, &s_slave->old_regs,
					s_slave->elf, &s_slave->fnt))
		{
			wmove(s_slave->wins[WIN_MAIN], 1, WIN_CODE_OX);
			wclrtoeol(s_slave->wins[WIN_MAIN]);
			wattrset(s_slave->wins[WIN_MAIN], A_BOLD);
			wattron(s_slave->wins[WIN_MAIN], COLOR_PAIR(1));
			wprintw(s_slave->wins[WIN_MAIN], ">> %s + %p\n", s_slave->fnt->name,
					s_slave->fnt->sym && s_slave->regs.rip ? s_slave->regs.rip
					- s_slave->fnt->sym->st_value : 0);
			wattroff(s_slave->wins[WIN_MAIN], A_BOLD);
			wattron(s_slave->wins[WIN_MAIN], COLOR_PAIR(1));
			wrefresh(s_slave->wins[WIN_MAIN]);
		}
		dump_stack(s_slave->pid, &s_slave->old_regs, &s_slave->regs,
				   s_slave->wins, sclean);
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
	if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP && !bp_hdl(s_term))
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
	int			status;

	if (s_term->slave.pid > -1)
	{
		if (s_term->slave.d_sbp || s_term->slave.d_hbp)
		{
			status = step_slave(&s_term->slave);
			if (slave_status(status, s_term->slave.wins[WIN_SH],
							 &s_term->slave) == SLAVE_EXIT)
				return (SLAVE_EXIT);
			sbp_restore(&s_term->slave);
		}
		return (take_step(s_term, &cont_slave));
	}
	else
		wprintw(s_term->slave.wins[WIN_SH], "No loaded executable.\n");
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
	if (!(s_slave->elf = elf_get(s_slave->pid, s_slave->filename)))
	{
		kill(s_slave->pid, SIGKILL);
		status = waitpid(s_slave->pid, &status, 0);
		s_slave->elf = NULL;
		slave_exit(s_slave->wins[WIN_SH], s_slave);
	}
	if (WIFEXITED(status))
	{
		wmove(s_slave->wins[WIN_MAIN], 1, 3);
		wprintw(s_slave->wins[WIN_MAIN],
				"%s won't be enslaved: can't execute it\n", path);
		wrefresh(s_slave->wins[WIN_MAIN]);
		return (slave_exit(s_slave->wins[WIN_SH], s_slave));
	}
	s_slave->e_sbp = NULL;
	s_slave->d_sbp = NULL;
	memset(&s_slave->regs, 0, sizeof(s_slave->regs));
	memset(&s_slave->old_regs, 0, sizeof(s_slave->old_regs));
	s_slave->fnt = NULL;
	dump_regs_name(s_slave->wins[WIN_REGS]);
	refresh_exe_state(s_slave, 1);
	wrefresh(s_slave->wins[WIN_REGS]);
	return (slave_status(status, s_slave->wins[WIN_SH], s_slave));
}

WINDOW          **dump_stack_start(int argc, char **argv, char **environ,
								   t_slave *s_slave)
{
	if (argc > 1)
	{
		if ((s_slave->wins = curses_init()))
		{
			s_slave->filename = argv[1];
			start_slave(argv[1], argv + 1, environ, s_slave);
		}
		return (s_slave->wins);
	}
	return (NULL);
}
