#include <see_stack.h>
#include <stdio.h>
#include <wchar.h>
#include <unistd.h>

void          print_byte(WINDOW *win, unsigned char c)
{
  const char  *hex = "0123456789ABCDEF";

  waddch(win, hex[(int)(c / 16)]);
  waddch(win, hex[(int)(c % 16)]);
}

void			print_addr(WINDOW *win, size_t var_len, size_t x, size_t y,
							   char *reg, int colors)
{
  size_t		idx;

  wmove(win, y, x);
  idx = 0;
  while (idx < var_len)
    print_byte(win, *(reg + idx++));
  wmove(win, y, x);
  wchgat(win, var_len * 2, COLOR_PAIR(colors), colors, 0);
}


void							addr_prefix(long base, long idx, long var_len,
									WINDOW *win)
{
	if (idx % 16)
		idx -= idx % 16;
	while (idx < var_len)
	{
		wmove(win, idx / 16, 0);
		wattron(win, COLOR_PAIR(3));
		wprintw(win, "%lx: ", (void *)((long)(base - idx)));
		wattron(win, COLOR_PAIR(1));
		idx += 16;
	}
}

void							new_stack_msg(WINDOW **wins)
{
	wmove(wins[WIN_MAIN], 1, 3);
	wprintw(wins[WIN_MAIN], "%s", NEW_FRAME_MSG);
	wrefresh(wins[WIN_MAIN]);
}

char							is_sane_stack_addr(long sp, long bp,
												   WINDOW **wins)
{
	if (bp == sp)
		new_stack_msg(wins);
	if (bp - sp >= 0 && bp - sp < MAX_STACK_FRAME && (bp > 0xfffffff || !bp))
		return (1);
	return (0);
}

void							adump_dword(WINDOW *win, char *var, ssize_t i,
											size_t li, size_t co, int color)
{
	size_t						c_co;

	c_co = (31 % 16) * 2 + ((31 + i) % 16) / 2;
	c_co += c_co / 19 ? 1 : 0;
	c_co += co + 6;
	if (c_co == 44)
	{
		wattron(win, COLOR_PAIR(1));
		wmove(win, li, 43 + ADDR_PLEN);
		waddch(win, '|');
		wattron(win, color);
	}
	wmove(win, li, c_co + 1 + ADDR_PLEN);
	if (*(unsigned char *)(var + i) >= ' ' && *(unsigned char *)(var + i)
		<= '~')
		waddch(win, *(unsigned char *)(var + i));
	else
		waddch(win, '.');
}

void							axdump_dword(WINDOW *win, char *var, size_t li,
											 size_t co, ssize_t len, int color)
{
	int							i;
	unsigned char				c_co;
	char						*x = "0123456789abcdef";

	i = 0;
	while (i < len && (!(i + co) || (i + co) % 16))
	{
		c_co = ((co + i) % 16) * 2 + ((co + i) % 16) / 2;
		c_co += c_co / 19 ? 1 : 0;
		if (c_co + 1 > 0 && c_co + 1 < WIN_STACK_CO)
		{
			wmove(win, li, c_co + 1 + ADDR_PLEN);
			wattron(win, color);
			waddch(win, x[*(unsigned char *)(var + i) / 16 % 16]);
			waddch(win, x[*(unsigned char *)(var + i) % 16]);
			wrefresh(win);
			adump_dword(win, var, i, li, co, color);
		}
		++i;
	}
}

void						add_hl(unsigned *hl, size_t idx, size_t len)
{
	size_t				i;

	i = 0;
	while (i < MAX_STACK_FRAME && *(hl + i * 2 + 1))
		++i;
	*(hl + i * 2) = idx;
	*(hl + i * 2 + 1) = len;
}

void						update_var(WINDOW *win, char *var, size_t off,
									   size_t var_len, unsigned *hl, int color)
{
	size_t					li;
	size_t					co;
	size_t					start;
	size_t					l_line;

	li = off / 16;
	co = off % 16;
	start = off;
	if (hl)
		add_hl(hl, off, var_len);
	off = 0;
	while (off < var_len)
	{
		/*l_line = (var_len - off + start) / 16 ? 16 : (var_len - off + start % 16); */
		l_line = 1;
		co = (off + start) % 16;
		li = (off + start) / 16;
		axdump_dword(win, var + off, li, co, l_line, color);
		off += l_line;
	}
	prefresh(win, 0, 0, WIN_BORDER_LEN, WIN_BORDER_LEN,
			 WIN_STACK_LI, WIN_STACK_CO);
}
