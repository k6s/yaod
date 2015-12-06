/**
 * @file curses.h
 * @brief Initialize curses windows.
 * @author k6
 */
#include <see_stack.h>

void					code_refresh(WINDOW *win, int py, int px)
{
	touchwin(win);
	prefresh(win, py, px,
			 WIN_CODE_OY, WIN_CODE_OX, WIN_CODE_OY + WIN_CODE_LI,
			 WIN_CODE_OX + WIN_CODE_CO);
}

void					sh_refresh(WINDOW *win, int py, int px)
{
	touchwin(win);
	prefresh(win, py, px,
			 WIN_SH_OY, WIN_SH_OX, WIN_SH_OY + WIN_SH_LI,
			 WIN_SH_OX + WIN_SH_CO);
}

static WINDOW			**get_wins(void)
{
	WINDOW				**wins;
	struct winsize		size;
	struct termios		term;

	if (ioctl(0, TIOCGWINSZ, &size) < 0)
	{
		fprintf(stderr, "Cannot get winsize, abort...\n");
		return (NULL);
	}
	if (size.ws_row < 100 && size.ws_col < 165)
	{
		fprintf(stderr, "More place pls. (%dx%d vs 100x165)\n", size.ws_row,
				size.ws_col);
		return (NULL);
	}
	if (!(wins = malloc(sizeof(*wins) * NB_WINS)))
		return (NULL);
	if (!(wins[WIN_SCR] = initscr()))
		return (NULL);
	if (!(wins[WIN_MAIN] = newwin(400, 400, 0, 0)))
		return (NULL);
	if (!(wins[WIN_STACK] = newpad(WIN_STACK_LI + 1000, WIN_STACK_CO + 1000)))
		return (NULL);
	if (!(wins[WIN_OSTACK] = subpad(wins[WIN_STACK], MAX_STACK_FRAME / 16 + 1,
							  WIN_STACK_CO, 0, 0)))
		return (NULL);
	if (!(wins[WIN_REGS] = subwin(wins[WIN_MAIN], WIN_REGS_LI, WIN_REGS_CO,
							WIN_BORDER_LEN, WIN_BORDER_LEN + WIN_STACK_CO)))
		return (NULL);
	if (!(wins[WIN_SH] = newpad(WIN_SH_LI, WIN_SH_CO)))
		return (NULL);
/*	wins[WIN_OSH] = subpad(wins[WIN_SH], WIN_SH_LI, WIN_SH_CO, WIN_SH_OY,
						   WIN_SH_OX); */
	if (!(wins[WIN_CODE] = newpad(WIN_CODE_LI, WIN_CODE_CO)))
		return (NULL);
/*	wins[WIN_OCODE] = subpad(wins[WIN_CODE], WIN_CODE_LI, WIN_CODE_CO,
							 WIN_CODE_OY, WIN_CODE_OX); */
	tcgetattr(0, &term);
	term.c_cc[VINTR] = 0;
	term.c_cc[VEOF] = 0;
	tcsetattr(0, TCSADRAIN | TCSANOW, &term);
	return (wins);
}

static void				set_wins_attr(WINDOW **wins)
{
	idlok(wins[WIN_SH], TRUE);
	scrollok(wins[WIN_SH], TRUE);
}

static void				set_wins_colors(WINDOW **wins)
{
	start_color();
	use_default_colors();
	init_pair(1, COLOR_WHITE, -1);
	init_pair(2, COLOR_RED, -1);
	init_pair(3, COLOR_CYAN, -1);
	init_pair(4, COLOR_YELLOW, -1);
	wattron(wins[WIN_STACK], COLOR_PAIR(1));
	wattron(wins[WIN_OSTACK], COLOR_PAIR(1));
	wattron(wins[WIN_REGS], COLOR_PAIR(1));
	wattron(wins[WIN_SH], COLOR_PAIR(1));
	wattron(wins[WIN_CODE], COLOR_PAIR(1));
}

void					curses_close(WINDOW **wins)
{
	delwin(wins[WIN_MAIN]);
	delwin(wins[WIN_STACK]);
	delwin(wins[WIN_SH]);
	delwin(wins[WIN_CODE]);
	delwin(wins[WIN_REGS]);
	delscreen((SCREEN *)wins[WIN_SCR]);
}

WINDOW					**curses_init(void)
{
	WINDOW				**wins;

	noecho();
	nl();
	if (!(wins = get_wins()))
		return (NULL);
	set_wins_attr(wins);
	set_wins_colors(wins);
	return (wins);
}
