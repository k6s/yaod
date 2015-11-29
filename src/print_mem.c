#include <see_stack.h>

int				print_xascii(t_term *s_term, char UN **av)
{
	long		addr;
	long		len;
	char		*data;
	WINDOW		*win;

	win = s_term->slave.wins[WIN_SH];
	if (av[1])
	{
		addr = strtol(av[1], NULL, 16);
		if (av[2])
			len = strtol(av[2], NULL, 0);
		else
			len = sizeof(Elf64_Addr);
		if (!(data = get_data(s_term->slave.pid, addr, len)))
			wprintw(win, "Failed to read %d bytes @ %p\n", len, addr);
		else
			showmem(win, data, len, addr, 1);
		free(data);
	}
	return (0);
}

int				print_xnoascii(t_term *s_term, char UN **av)
{
	long		addr;
	long		len;
	char		*data;
	WINDOW		*win;

	win = s_term->slave.wins[WIN_SH];
	if (av[1])
	{
		addr = strtol(av[1], NULL, 16);
		if (av[2])
			len = strtol(av[2], NULL, 0);
		else
			len = sizeof(Elf64_Addr);
		if (!(data = get_data(s_term->slave.pid, addr, len)))
			wprintw(win, "Failed to read %d bytes @ %p\n", len, addr);
		else
			showmem(win, data, len, addr, 0);
		free(data);
	}
	return (0);
}

int				print_val(t_term *s_term, char UN **av)
{
	long		addr;
	long		len;
	char		*data;
	WINDOW		*win;

	win = s_term->slave.wins[WIN_SH];
	if (av[1])
	{
		addr = strtol(av[1], NULL, 16);
		len = sizeof(Elf64_Addr);
		if (!(data = get_data(s_term->slave.pid, addr, len)))
			wprintw(win, "Failed to read %d bytes @ %p\n", len, addr);
		else
			wprintw(win, "    %p\n", *(Elf64_Addr *)data);
		free(data);
	}
	return (0);
}

int				print_str(t_term *s_term, char UN **av)
{
	long		addr;
	long		len;
	char		*data;
	WINDOW		*win;
	size_t		i;

	win = s_term->slave.wins[WIN_SH];
	if (av[1])
	{
		addr = strtol(av[1], NULL, 16);
		if (av[2])
			len = strtol(av[2], NULL, 10);
		else
			len = 1;
		i = 0;
		while (i < len)
		{
			if (!(data = get_str(s_term->slave.pid, addr)))
			{
				wprintw(win, "Failed to string @ %p\n", len, addr);
				break ;
			}
			else
				wprintw(win, "    %012lx: \"%s\"\n", addr, data);
			addr += strlen(data) + 1;
			++i;
			free(data);
		}
	}
	return (0);
}
