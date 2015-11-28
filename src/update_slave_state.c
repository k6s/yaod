/*
** update_slave_state.c for dump_stack in /home/k6/null/dump_stack
** 
** Made by Chleo Cano
** Login   <k6@epitech.net>
** 
** Started on  Fri Jan 30 04:13:40 2015
** Last update Fri Jan 30 04:14:02 2015 
*/
#include <see_stack.h>

void							dump_new_frame(struct user_regs_struct *regs,
                                 struct user_regs_struct *old_regs,
                                 void *stack_frame, WINDOW **wins)
{
  wclear(wins[WIN_REGS]);
  dump_regs_name(wins[WIN_REGS]);
  if (old_regs)
    dump_regs(old_regs, regs, wins, 1);
  werase(wins[WIN_STACK]);
  addr_prefix(regs->rbp, 0, regs->rbp - regs->rsp + 8, wins[WIN_STACK]);
/*  dump_data((char *)stack_frame, (char *)((long)stack_frame + regs->rbp
										  - regs->rsp + 8),
            wins[WIN_STACK], 0, 0); */
  update_var(wins[WIN_STACK], stack_frame, 0, regs->rbp - regs->rsp + 8, NULL, 1);
  touchwin(wins[WIN_STACK]);
  prefresh(wins[WIN_STACK], 0, 0, WIN_BORDER_LEN, WIN_BORDER_LEN, WIN_STACK_LI, 
		   WIN_STACK_CO);
}
