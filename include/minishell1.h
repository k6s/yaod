/*
** minishell1.h for minishell1 in /home/cano_c/rendu/ELEM/CPE_2014_bsq
** 
** Made by Chloe Cano
** Login   <cano_c@epitech.net>
** 
** Started on  Tue Jan 13 14:04:15 2015
** Last update Mon Jan 26 02:27:34 2015 
*/

#ifndef MINISHELL1_H_
# define MINISHELL1_H_

# include <see_stack.h>

# ifndef _POSIX_SOURCE
#  define _POSIX_SOURCE
# endif

# include <unistd.h>
# include <my.h>
# include <get_next_line.h>
# include <termcap.h>
# include <termios.h>
# include <fcntl.h>
# include <dirent.h>
# include <sys/stat.h>

/*
** BUILTINS
*/

typedef int	(*t_builtfp)(t_term *, char **);

char		add_builtins(t_progs *s_progs);

int		my_exit(t_term *s_term, char **av);
int		my_unsetenv(t_term *term, char **av);
int		my_setenvi(t_term *term, char **av);
int		my_env(t_term *term, char **av);
int		my_cd(t_term *term, char **av);

/*
** PROGS INDEX
*/

struct			s_prog
{
  char			name[NAME_MAX];
  size_t		namlen;
  char			path[PATH_MAX];
  t_builtfp		builtfp;
};


struct			s_progs
{
  char			exist;
  t_prog		*prog;
  t_progs		*next[256];
};

t_prog		*find_prog(t_progs *progs, char *progname);
char		add_prog(char *filename, char *path, t_progs *s_progs);
t_progs		*get_path_progs(char *path);

/*
** ENVIRONMENT
*/

char		get_environ(t_term *s_term, char **environ);
char		**add_env_var(char **environ, char *new);
int			ow_env_var(char **environ, char *var_name, char *new);
char		*get_envvar_name(char *s);
void		del_envvar(char **environ, char *target);
char		**my_setenv(char **env, char *var, char ow);

/*
** UTILS
*/

pid_t		my_getpid(void);


/*
** SIGNALS
*/

t_term		*static_term(t_term *term);
void		init_signals(void);

/*
** TERMCAPS
*/

int				tput_putc(int i);
struct termios	*init_termcap(char **environ);
void			restore_termcap(struct termios *tsave);

/*
** CLEANING
*/

void		free_strtab(char **strtab);
void		free_progs(t_progs *progs);
void		free_stuff(t_term *s_term);


/*
** READ CMDLINE
*/
int			shell_readloop(t_term *s_term, WINDOW **wins);
void		clear_line(t_buff *line);

/*
** CMDLINE EDITION
*/

# define BUFF_SIZE	1

struct			s_buff
{
  char			buff[BUFF_SIZE];
  size_t		len;
  t_buff		*next;
  t_buff		*prev;
};

# define PROMPT_MAX	PATH_MAX + 30

struct			s_line
{
  char			prompt[PROMPT_MAX];
  t_buff		*line;
  char			*s;
  t_hist		*hist;
  t_hist		*c_hist;
};

struct			s_hist
{
  t_buff		*buff;
  t_hist		*next;
  t_hist		*prev;
};

# define K_NB		5
# define K_UP		65
# define K_DO		66
# define K_RI		67
# define K_LE		68
# define K_DEL		127
# define K_CTRL		4
# define K_TAB		9
# define K_ESC		27
# define K_PUP		0
# define K_PDO		0

int		special_key(t_buff **s_buff, t_line *s_line, char *key,
			    t_term *s_term);

/*
** EXECUTE CMDLINE
*/

char		exe_cmd(t_prog *prog, char **av, char **environ, WINDOW **wins);
char		exe_builtin(t_prog *prog, char **av, t_term *s_term);

/*
** DEBUG
*/
int			run_loop(t_term *s_term, char UN **av);
int			step_prog(t_term *s_term, char UN **av);
int			cont_loop(t_term *s_term, char UN **av);
int			blind_cont_prog(t_term *s_term, char UN **av);

int			sbp_delete(t_term *s_term, char **av);
int			new_sbp(t_term *s_term, char UN **av);
int			sbp_lst(t_term *s_term, char UN **av);

int			info_segment(t_term *s_term, char UN **av);
int			info_dynsym(t_term *s_term, char UN **av);
int			info_symtab(t_term *s_term, char UN **av);
int			info_dyntab(t_term *s_term, char UN **av);
int			info_linkmap(t_term *s_term, char UN **av);
int			info_sections(t_term *s_term, char UN **av);

int			help(t_term *s_term, char UN **av);

int			print_xascii(t_term *s_term, char UN **av);
int			print_xnoascii(t_term *s_term, char UN **av);
int			print_val(t_term *s_term, char UN **av);
int			print_str(t_term *s_term, char UN **av);

int			new_hbp(t_term *s_term, char **av);
int			hbp_lst(t_term *s_term, char UN **av);
int			hbp_delete(t_term *s_term, char UN **av);

#endif /* !MINISHELL1_H_ */
