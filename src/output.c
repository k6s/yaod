/**
 * @file dump_data.c
 * @brief Output memory content on curses windows in hexdump form.
 * @author k6
 */
#include <see_stack.h>

void                print_byte(WINDOW *win, unsigned char c)
{
  const char        *hex = "0123456789ABCDEF";

  waddch(win, hex[(int)(c / 16)]);
  waddch(win, hex[(int)(c % 16)]);
}

void				print_hex(WINDOW *win, char *data, char *end_data)
{
  size_t            idx;

  idx = 0;
  while (idx < sizeof(unsigned long)  && data + idx <= end_data)
  {
    print_byte(win, *(data + (idx)));
    print_byte(win, *(data + (++idx)));
    if (++idx <= sizeof(unsigned long) - 2)
      waddch(win, ' ');
  }
  while (idx++ < sizeof(unsigned long))
    waddch(win, ' ');
}

void                print_ascii(WINDOW *win, char *data, char *end_data)
{
  size_t            idx;

  idx = 0;
  while (idx < sizeof(long) * 2 && (ssize_t)idx + data <= end_data)
  {
    if (*(data + idx) >= ' ' && *(data + idx)  <= '~')
      waddch(win, *(data + idx));
    else
      waddch(win, '.');
    ++idx;
  }
  while (idx++ < 16)
    waddch(win, ' ');
}

void    	   		dump_data(void *b_data, void *e_data, WINDOW *win,
							  size_t x, size_t y)
{
  size_t			idx;

  idx = 0;
  wmove(win, y++, x + 1);
  while ((long)((long)b_data + idx) <= (long)e_data
		 && (((long)b_data + idx - (long)e_data) / 8))
  {
    print_hex(win, (void *)((long)b_data + idx), e_data);
    if (idx / sizeof(long) % 2)
    {
      waddstr(win, "  | ");
      print_ascii(win, (char *)((long)b_data + idx - sizeof(long)), e_data);
      wmove(win, y++, x + 1);
    }
    else
      waddstr(win, "  ");
    idx += sizeof(long);
  }
  sleep(0.4);
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

void			my_perror(char *prog_name, char *msg)
{
  write(1, prog_name, strlen(prog_name));
  write(1, ": ", 2);
  perror(msg);
}
