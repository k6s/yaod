#include <see_stack.h>

static void			general_help(WINDOW *win)
{
	wprintw(win, "Basic process control:\n");
	wprintw(win, "  r     - run / continue\n");
	wprintw(win, "  s     - step\n");
	wprintw(win, "  rb    - run / continue blind mode\n");
	wprintw(win, "\"h i\" to list slave's info commands\n");
	wprintw(win, "\"h b\" to list breakpoints related commands\n");
	wprintw(win, "\"h p\" to list commands printing memory content\n");
}

static void			info_help(WINDOW *win)
{
	wprintw(win, "Slave's Info:\n");
	wprintw(win, "  ip    - info program headers\n");
	wprintw(win, "  id    - info dyntab\n");
	wprintw(win, "  ids   - info dynamic symbols\n");
	wprintw(win, "  is    - info symtab\n");
	wprintw(win, "  ish   - info section headers\n");
	wprintw(win, "  il    - info link map\n");
	wprintw(win, "  ir    - info relocations\n");
}

static void			breaks_help(WINDOW *win)
{
	wprintw(win, "Breakpoint:\n");
	wprintw(win, "  b 0xdeadbeef    - break at address 0xdeadbeef\n");
	wprintw(win, "  bl              - list breakpoints\n");
	wprintw(win, "  bd 1            - delete breakpoint 1\n");
	wprintw(win, "Watchpoint:\n");
	wprintw(win, "  hb 0xdeadbeef type [len] [scope]  - break at \
address 0xdeadbeef. Type can be access ('a'), exec ('x'), write ('w').\n");
}

static void			print_help(WINDOW *win)
{
	wprintw(win, "Print memory content:\n");
	wprintw(win, "  p 0xdeadbeff        - print value at 0xdeadbeef\n");
	wprintw(win, "  ps 0xdeadbeef 123   - print 123 strings starting \
from 0xdeadbeef\n");
	wprintw(win, "  px 0xdeadbeff 123   - print hexdump for content at\
0xdeadbeef, 123 bytes length\n");
	wprintw(win, "  pxa 0xdeadbeff 123  - print hexdump and ascii for \
content at 0xdeadbeef, 123 bytes length\n");
}

int					help(t_term *s_term, char **av)
{
	WINDOW			*win;

	win = s_term->slave.wins[WIN_SH];
	if (!av[1])
		general_help(win);
	else if (strlen(av[1]) == 1)
	{
		switch (*av[1])
		{
		 case 'i':
			info_help(win);
			break ;
		 case 'b':
			breaks_help(win);
			break ;
		 case 'p':
			print_help(win);
			break ;
		 default :
			general_help(win);
		}
	}
	else
		general_help(win);
	return (0);
}
