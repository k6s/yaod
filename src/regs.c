/*
** regs.c for dump_stack in /home/k6/null/dump_stack
** 
** Made by Chloe Cano
** Login   <k6@epitech.net>
** 
** Started on  Fri Jan 30 04:29:59 2015
** Last update Fri Jan 30 04:30:20 2015 
*/
#include <see_stack.h>

void							dump_regs_name(WINDOW *win)
{
  size_t						idx;
  struct user_regs_struct		regs;
  const char					regs_name[][3] = {"r15", "r14", "r13", "r12", 
    "rbp", "rbx", "r11",
    "r10", "r9 ", "r8 ", "rax", "rcx", "rdx", "rsi",
    "rdi", "oax", "rip", "cs ", "efg", "rsp", "ss ",
    "fsb", "gsb", "ds ", "es ", "fs ", "gs "};

  idx = 0;
  wattron(win, COLOR_PAIR(3));
  while ((size_t)idx < sizeof(struct user_regs_struct) / sizeof(regs.r15))
  {
    wmove(win, idx, 0);
    waddch(win, regs_name[idx][0]);
    waddch(win, regs_name[idx][1]);
    waddch(win, regs_name[idx][2]);
    ++idx;
  }
  wattron(win, COLOR_PAIR(1));
}

static void		restore_regs_color(WINDOW *win, size_t *regs_hl)
{
  size_t		idx;

  idx = 0;
  while (regs_hl[idx])
  {
    wmove(win, 7 + regs_hl[idx], WIN_BORDER_LEN + WIN_STACK_CO + 2 + 4);
    wchgat(win, 16, COLOR_PAIR(1), 1, 0);
    wrefresh(win);
    ++idx;
  }
}

void							dump_regs(struct user_regs_struct *old_regs,
										  struct user_regs_struct *regs,
										  WINDOW **wins, char refresh)
{
  size_t						idx;
  size_t						regs_hl[30];
  size_t						regs_idx;
  WINDOW						*win;

  win = wins[WIN_REGS];
  idx = 0;
  regs_idx = 0;
  memset(regs_hl, 0, 30 * sizeof(*regs_hl));
  while (idx < sizeof(*regs) && idx < 30 * sizeof(regs->r15))
  {
    if ((long)(*((char *)regs + idx)) != (long)(*((char *)old_regs + idx)))
    {
      addrhl(win, 8, 6, idx / sizeof(regs->r15), (char *)regs + idx);
      regs_hl[regs_idx++] = idx / sizeof(regs->r15);
    }
    else if (refresh)
      print_addr(win, 8, 6, idx / sizeof(regs->r15), (char *)regs + idx, 0);
    idx += (sizeof(regs->r15));
  }
  wrefresh(win);
  usleep(10000);
  restore_regs_color(win, regs_hl);
}
