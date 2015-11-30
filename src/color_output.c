#include <see_stack.h>

void            addrhl(WINDOW *win, size_t var_len, size_t x, size_t y,
					   char *reg)
{
  print_addr(win, var_len, x, y, reg, 2);
}
