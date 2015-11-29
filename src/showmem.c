/*
 ** my_showmem.c for my_showmem.c in /home/k6/rendu/Piscine_C_J06/ex_18
 ** 
 ** Made by Cano Chloe
 ** Login   <k6@epitech.net>
 ** 
 ** Started on  Tue Oct  7 14:58:54 2014 Cano Chloe
 ** Last update Mon Oct 20 22:18:40 2014 Cano Chloe
 */

#include <my.h>
#include <curses.h>

static void		print_aascii(WINDOW *win, unsigned char *str, int len)
{
	int		n;

	n = -1;
	waddstr(win, "| ");
	while (++n < len)
	{
		if (str[n] > 31 && str[n] < 127)
			waddch(win, str[n]);
		else
			waddch(win, '.');
	}
	waddch(win, '\n');
}

static void		print_hhex(WINDOW *win, unsigned char *str, int len)
{
	int		n;

	n = 0;
	while (n < len)
	{
		wprintw(win, "%02x", str[n++]);
		if (n < len)
			wprintw(win, "%02x", str[n++]);
		waddch(win, ' ');
	}
}

static void		print_offset(WINDOW *win, int offset)
{
	wprintw(win, "    0x%012x", offset);
	waddch(win, ':');
	waddch(win, ' ');
}

int			showmem(WINDOW *win, unsigned char *str, int size, long base,
					char ascii)
{
	int		off;
	int		n;

	off = 0;
	n = -1;
	while ((size - off) / 16)
	{
		print_offset(win, off + base);
		print_hhex(win, str + off, 16);
		if (ascii)
			print_aascii(win, str + off, 16);
		else
			waddch(win, '\n');
		off += 16;
	}
	if (size - off)
	{
		print_offset(win, off + base);
		print_hhex(win, str + off, size - off);
		while (++n < (4 * 10 - (size % 16 ) * 2 - (size % 16) / 2)
			   - (size % 16 % 2))
			waddch(win, ' ');
		if (ascii)
			print_aascii(win, str + off, size % 16);
		else
			waddch(win, '\n');
	}
	return (0);
}
