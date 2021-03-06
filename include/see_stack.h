/**
 ** @file see_stack.h
 ** @brief Get stack content and update stack window.
 ** @author K6
 ** 
 ** see_stack.h for dump_stack in /home/cano_c/null/dump_stack
 ** 
 ** Made by Chloe Cano
 ** Login   <cano_c@epitech.net>
 ** 
 ** Started on  Fri Jan 23 10:17:45 2015
 ** Last update Fri Jan 23 10:17:55 2015 
 */
#ifndef _SEE_STACK_H
# define _SEE_STACK_H

# define _XOPEN_SOURCE		600
# define _POSIX_C_SOURCE	201109L
# define _DEFAULT_SOURCE 
# define __USE_MISC

# include <sys/types.h>
# include <sys/mman.h>
# include <sys/ioctl.h>
# include <sys/select.h>
# include <fcntl.h>
# include <signal.h>
# include <unistd.h>
# include <string.h>
# include <strings.h>
# include <stdio.h>
# include <stdlib.h>
# include <stdint.h>
# include <sys/user.h>
# include <sys/wait.h>
# include <limits.h>
# include <curses.h>
# include <elf.h>
# include <link.h>
# include <elf_parse.h>
# include <ptrace_get.h>
# include <capstone/capstone.h>
# include <hbp.h>
# include <sbp.h>

typedef struct s_buff	t_buff;		/*! @brief typedef to @ref s_buff */
typedef struct s_prog	t_prog;		/*! @brief typedef to @ref s_prog */
typedef struct s_progs	t_progs;	/*! @brief typedef to @ref s_progs */
typedef struct s_term	t_term;		/*! @brief typedef to @ref s_term */
typedef struct s_slave	t_slave;	/*! @brief typedef to @ref s_slave */
typedef struct s_line	t_line;		/*! @brief typedef to @ref s_line */
typedef struct s_hist	t_hist;		/*! @brief typedef to @ref s_hist */
typedef struct s_sbp	t_sbp;	/*! @brief typedef to @ref s_sbreak */
typedef struct s_fnt	t_fnt;
typedef struct s_win	t_win;

# include <minishell1.h>

/*! @brief Max stack frame length. */
# define MAX_STACK_FRAME	      0x3e60
/*! @brief Message to display at each new stacl frame. */
# define NEW_FRAME_MSG		      "  >> New Stack Frame"


/*! @brief Windows margin between border and content. */
# define WIN_BORDER_LEN			3

enum			e_win_offset
{
	WIN_SH = 0,			/*! @brief Shell window offset in windows array */
	WIN_STACK,		/*! @brief Stack window offset in windows array */
	WIN_REGS,		/*! @brief Registry window offset in windows array */
	WIN_CODE,
	WIN_CALL,
	WIN_SCR,
	WIN_MAIN,		/*! @brief Main window offset in windows array */
	WIN_OSTACK,		/*! @brief Stack pad offset in windows array */
	WIN_OSH,		/*! @brief Shell window offset in windows array */
	WIN_OCODE,
	WIN_OCALL,
	WIN_OOUT,
	NB_WINS
};

# define ADDR_PLEN		13

# define WIN_STACK_CO   80				/*! @brief stack window colums */
# define WIN_STACK_LI	WIN_REGS_LI		/*! @brief stack window lines */
# define WIN_OSTACK_LI	999

# define WIN_REGS_OX	WIN_BORDER_LEN + WIN_STACK_CO
# define WIN_REGS_OY	WIN_BORDER_LEN
# define WIN_REGS_CO	22				/*! @brief registry window columns */
# define WIN_REGS_LI	27				/*! @brief registry window lines */

# define WIN_SH_OX		WIN_BORDER_LEN
# define WIN_SH_OY		WIN_STACK_LI + WIN_BORDER_LEN * 2
# define WIN_SH_CO		WIN_STACK_CO	/*! @brief shell window columns */
# define WIN_SH_LI		15				/*! @brief shell window lines */

# define WIN_CODE_OX	WIN_BORDER_LEN * 3 + WIN_STACK_CO + WIN_REGS_CO
# define WIN_CODE_OY	WIN_BORDER_LEN
# define WIN_CODE_LI	WIN_REGS_LI
# define WIN_CODE_CO	50

# define WIN_CALL_OX	WIN_STACK_CO + WIN_BORDER_LEN * 3
# define WIN_CALL_OY	WIN_SH_OY
# define WIN_CALL_LI	WIN_SH_LI
# define WIN_CALL_CO	WIN_CODE_CO + WIN_REGS_CO

/*!
 * @brief Register index in user_reg_struct.
 */
enum					e_regum
{
	E_R15 = 0,
	E_R14,
	E_R13,
	E_R12,
	E_RBP,
	E_RBX,
	E_R11,
	E_R10,
	E_R9,
	E_R8,
	E_RAX,
	E_RCX,
	E_RDX,
	E_RSI,
	E_RDI,
	E_OAX,
	E_RIP,
	E_CS,
	E_EFG,
	E_RSP,
	E_SS,
	E_FSB,
	E_GSB,
	E_DS,
	E_ES,
	E_FS,
	E_GS
};

typedef struct		t_bp
{
	t_hbp			*e_hbp;
	t_hbp			*d_hbp;
	t_sbp			*s_sbp;
	t_sbp			*d_sbp;
	int				dr6;
}					s_bp;

int							sbp_disable(t_slave *s_slave, t_sbp *sbp);
int							sbp_enable(t_slave *s_slave, t_sbp *sbp);
int							sbp_restore(t_slave *s_slave);

# define SLAVE_EXIT			42
# define SLAVE_BREAK		23

int							hbp_disable(t_slave *s_slave, t_hbp *hbp);
int							hbp_enable(t_slave *s_slave, t_hbp *hbp);

# define FNT_LOC				1
# define FNT_DYN				2
# define FNT_STA				4
# define FNT_NOSZ				8
# define FNT_SHA				16
# define FNT_JMP				32
# define FNT_PLT				(64 | FNT_JMP)
# define FNT_UNKNOWN			128
# define FNT_SHA_STA			(FNT_SHA | FNT_STA)

struct							s_fnt
{
	char						*name;
	Elf64_Sym					*sym;
	t_fnt						*prv;
	struct user_regs_struct		*regs;
	unsigned char				*stack;
	int							type;
	unsigned long				end;
	unsigned long				rbp;
	unsigned long				rip;
};

t_fnt			*fnt_new(pid_t pid, t_elf *elf, u_long addr);
int				fnt_ret(pid_t pid, t_elf *elf, struct user_regs_struct *regs,
						t_fnt **fnt_lst);
int				fnt_same(t_fnt **fnt_lst, t_fnt *fnt,
						 unsigned long rip, unsigned long rbp);
int				fnt_call_jmp(pid_t pid, t_fnt **fnt_lst, t_fnt *fnt,
							 unsigned long rip, unsigned long rbp);

struct							s_win
{
	size_t						x;
	size_t						y;
	size_t						orig;
	WINDOW						*win;
};

/*!
 * @brief Informations and memory content of a process.
 */
struct							s_slave
{
	char						*filename;
	t_elf						*elf;		/*!< @brief ELF process info */
	pid_t		                pid;		/*!< @brief Process ID */
	struct cs_insn				*ins;
	int							n_ins;
	struct user_regs_struct		regs;		/*!< @brief registries content */
	struct user_regs_struct		old_regs;	/*!< @brief previous registry content */
	WINDOW						**wins;		/*!< @brief Windows curses id */
	t_win						s_win[WIN_SCR];
	unsigned					c_win;
	t_sbp						*e_sbp;		/*!< @brief Enabled soft breakpoints */
	t_sbp						*d_sbp;		/*!< @brief Disabled soft breakpoints */
	t_hbp						*e_hbp;
	t_hbp						*d_hbp;
	t_fnt						*fnt;
	int							fdm;		/*!< @brief Master side of PTY */
	int							fds;		/*!< @brief Slave side of PTY */
	int							end;
};

/*
 ** GENERAL TERM STRUCT
 */

struct						s_term
{
	char					*file;
	char					**environ;
	t_progs					*progs;
	char					end;
	pid_t					pid;
	t_buff					*line;
	t_slave					slave;
	unsigned				c_win;
	char					intr;
};

/*
 * dump_stack.c
 * ============
 * start and PTRACE_TRACEME slavee
 */
	WINDOW          **dump_stack_start(int argc, char **arv, char **environ,
									   t_slave *s_slave);


WINDOW			**curses_init(t_win *wins);

/*
 * update_slave_state.c
 * ====================
 */
void			dump_new_frame(struct user_regs_struct *regs,
							   struct user_regs_struct *old_regs,
							   void *stack_frame, WINDOW **wins);

/*
 * stack.c
 * =======
 * get / update slave's current stack frame
 */
char			is_sane_stack_addr(long sp, long bp, WINDOW **wins);
void			update_vars(struct user_regs_struct *regs, long stack_frame,
							long old_stack_frame, WINDOW **wins, unsigned *hl);
int				dump_new_vars(struct user_regs_struct *regs,
							  struct user_regs_struct *old_regs,
							  void *stack_frame, WINDOW *win, unsigned *hl);
int				dump_stack(pid_t pid, struct user_regs_struct *old_regs,
						   struct user_regs_struct *regs, WINDOW **wins,
						   char clean);
void			update_var(WINDOW *win, char *var, size_t off,
						   size_t var_len, unsigned *hl, int color);


/*
 * regs.c
 * ======
 * get / update slave's registry
 */

	void			dump_regs_name(WINDOW *win);
	void			dump_regs(struct user_regs_struct *old_regs,
							  struct user_regs_struct *regs, WINDOW **wins,
							  char refresh);
	int				update_regs(pid_t pid, struct user_regs_struct *regs,
								struct user_regs_struct *old_regs);

/*
 * ouput.c
 * =======
 * curses output
 */

void			print_byte(WINDOW *wins, unsigned char c);
void			print_hex(WINDOW   *win, char *data, char *end_data);
void			print_ascii(WINDOW *win, char *data, char *end_data);
void			dump_data(void *b_data, void *e_data, WINDOW *win, size_t x,
						  size_t y);
void			print_addr(WINDOW *win, size_t var_len, size_t x, size_t y,
						   char *reg, int colors);
void			addr_prefix(long base, long idx, long var_len,
							WINDOW *win);
void			my_perror(char *prog_name, char *msg);

void			out_refresh(WINDOW *win, int py, int px);
void			code_refresh(WINDOW *win, int py, int px);
void			sh_refresh(WINDOW *win, int py, int px);
void			call_refresh(WINDOW *win, int py, int px);
int				showmem(WINDOW *win, unsigned char *str, int size, long base,
						char ascii);

/*
 * color_output.c
 * ==============
 */
void			restore_color(WINDOW *win, size_t x, size_t y, long addr,
							  long end);
void			strhl(WINDOW *win, size_t var_len, size_t x, size_t y,
					  char *str);
void			restore_stack_color(WINDOW **wins, size_t *idx_res);

/*
 * run_slave.c
 * ===========
 */

int				bp_hdl(t_term *s_term);
int				start_slave(char *path, char **cmd, char **environ,
								t_slave *s_slave);
int				ptrace_exec(char *path, char **cmd, char **environ, int fd_s,
							int fdm);
int				step_slave(t_slave *slave);
int				cont_slave(t_slave *slave);
int				update_slave_state(t_slave *s_slave, char sclean);

/*
 * code.c
 * ==========
 */
int				get_code(pid_t pid, unsigned long rip, int max_ins,
						 struct cs_insn **ins, t_sbp *sbp);
int				update_code(t_slave *s_slave);

/*
 * Slave's I/O
 * ===========
 */

int				handle_pty(int fdm, WINDOW *win, int pid);
int				open_pty(t_slave *s_slave);

#endif /* ! _SEE_STACK_H */
