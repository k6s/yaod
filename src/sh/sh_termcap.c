/*
** sh_termcap.c for minishell1 in /home/cano_c/rendu/UNIX/PSU_2014_minishell1
** 
** Made by Chloe Cano
** Login   <cano_c@epitech.net>
** 
** Started on  Mon Jan 19 16:22:21 2015
** Last update Tue Jan 20 18:19:47 2015 
*/
#include <minishell1.h>

static void		tcfatal(int ret)
{
  write(2, "./mysh:  cannot initalize termcap\n", 35);
  exit(ret);
}

int			tput_putc(int i)
{
  return (write(1, &i, sizeof(i)));
}

void			restore_termcap(struct termios *tsave)
{
	if (tsave)
		tcsetattr(0, TCSANOW, tsave);
}

struct termios		*init_termcap(char **environ)
{
  char			t_buff[2048];
  struct termios	*tsave;
  struct termios	term;

  if (environ)
    {
      if (tgetent(t_buff, my_getenv(environ, "TERM=")) == -1)
	tcfatal(-1);
    }
  else
    if (tgetent(t_buff, "xterm") == -1)
      tcfatal(-1);
  if (tcgetattr(0, &term) == -1)
    tcfatal(-2);
  if (!(tsave = malloc(sizeof(*tsave))))
    return (NULL);
  my_memcpy(tsave, &term, sizeof(*tsave));
  term.c_lflag &= ~(ICANON);
  term.c_lflag &= ~(ECHO);
  term.c_cc[VMIN] = 1;
  term.c_cc[VTIME] = 0;
  if (tcsetattr(0, TCSANOW, &term) < 0)
    tcfatal(-3);
  return (tsave);
}
