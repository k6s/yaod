#include <see_stack.h>

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

static void						output(WINDOW *win, char *s, size_t len)
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

static ssize_t					slave_input(int pid, int fdm, WINDOW *win)
{
	ssize_t						r_len;
	char						buff[256];

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
	return (r_len);
}

static ssize_t					slave_output(int fdm, WINDOW *win)
{
	int							flags;
	ssize_t						r_len;
	char						buff[256];

	flags = fcntl(fdm, F_GETFL, 0) | O_NONBLOCK;
	fcntl(fdm, F_SETFL, flags);
	while ((r_len = read(fdm, buff, sizeof(buff) - 1)) > 0)
	{
		buff[r_len] = 0;
		output(win, buff, r_len);
	}
	flags = fcntl(fdm, F_GETFL, 0) ^ O_NONBLOCK;
	fcntl(fdm, F_SETFL, flags);
	return (r_len);
}

int								handle_pty(int fdm, WINDOW *win, int pid)
{
	fd_set						fd_in;
	struct timeval				tv;
	int							status;
	int							stopped;

	stopped = 0;
	status = 0;
	while (!stopped)
	{
		memset(&tv, 0, sizeof(tv));
		tv.tv_usec = 10;
		FD_ZERO(&fd_in);
		FD_SET(fdm, &fd_in);
		FD_SET(0, &fd_in);
		if (select(fdm + 1, &fd_in, NULL, NULL, &tv) < 0)
		{
			perror("Err on select()");
			waitpid(pid, &status, 0);
			return (status);
		}
		if (FD_ISSET(0, &fd_in) && slave_input(pid, fdm, win) < 0)
		{
			waitpid(pid, &status, 0);
			return (status);
		}  
		else if (FD_ISSET(fdm, &fd_in) && slave_output(fdm, win) < 0)
		{
			waitpid(pid, &status, 0);
			return (status);
		}
		if (waitpid(pid, &status, WNOHANG) != pid)
			stopped = 0;
		else
			stopped = 1;
	}
	return (status);
}
