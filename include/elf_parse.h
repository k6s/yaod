#ifndef ELF_PARSE_H_
# define ELF_PARSE_H_

# include <elf.h>
# include <link.h>
# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>
# include <strings.h>
# include <ptrace_get.h>

# include <fcntl.h>

typedef struct s_elf		t_elf;		/*! @brief typedef to @ref s_elf */
typedef struct s_elf_sha	t_elf_sha;

/*!
 * @brief Info about elf executable (from process memory,
 * not elf binary file).
 */
struct						s_elf
{
	Elf64_Ehdr				*e_hdr; /*!< @brief Elf header */
	Elf64_Shdr				**s_hdr;
	Elf64_Phdr				**p_hdr; /*!< @brief Elf program headers */
	Elf64_Dyn				**dyn;
	Elf64_Sym				**dynsym;
	Elf64_Sym				**symtab;
	Elf64_Rela				**rela_plt;
	char					*dynstr;
	Elf64_Xword				strsz;
	char					*strtab;
	Elf64_Xword				sstrsz;
	char					*shstrtab;
	Elf64_Xword				shstrsz;
	Elf64_Word				nchains;
	struct link_map			*link_map;
	char					linked;
	char					stripped;
	t_elf_sha				*sha;
};

/*! \brief Typedef to @ref s_tables_addr */
typedef struct s_tables_addr		t_tables_addr;

struct							s_elf_sha
{
	struct link_map				*lm;
	t_tables_addr				*dyntabs;
	int							fd;
	Elf64_Ehdr					e_hdr;
	Elf64_Sym					*strtab;
	Elf64_Sym					*symtab;
	t_elf_sha					*nxt;
};

/*!
 * \brief Symtab and strtab addresses and their associated nchains.
 */
struct				s_tables_addr
{
	unsigned long	symtab;
	unsigned long	strtab;
	unsigned long	*nchains;
	unsigned long	*symndx;
};

t_elf					*elf_get(pid_t pid, char *filename);
void					elf_free_phdr(Elf64_Phdr **p_hdr);
void					elf_free_sym(Elf64_Sym **sym);
void					elf_free_dyn(Elf64_Dyn **dyn);
int						elf_free(t_elf *elf);

/*
 * From a running process PID
 * ============================================================================
 */
ssize_t					elf_from_file(char *filename, t_elf *elf);
t_elf					*elf_from_process(pid_t pid, t_elf *elf);

Elf64_Ehdr				*elf_ehdr(pid_t pid);
Elf64_Phdr				*elf_phdr_entry(pid_t pid, Elf64_Ehdr *e_hdr,
										unsigned type);
Elf64_Phdr				**elf_phdr(pid_t pid, Elf64_Ehdr *e_hdr);
Elf64_Dyn				**elf_dyn(pid_t pid, Elf64_Phdr *p_hdr);
char					*elf_symstr(char *strtab, size_t off, Elf64_Xword size);
Elf64_Sym				**elf_dynsym(pid_t pid, Elf64_Dyn **dynsym,
									 Elf64_Word nchains, char *linked);
char					*elf_strtab(pid_t pid, Elf64_Dyn **dyn,
								   	Elf64_Xword *strsz);
Elf64_Rela				**elf_rela(pid_t pid, Elf64_Shdr **shdr, char *strtab,
								  long sstrsz);
Elf64_Shdr				*elf_shdr_type(Elf64_Shdr **shdr, Elf64_Word type);
Elf64_Shdr				*elf_shdr_name(Elf64_Shdr **shdr, Elf64_Word type,
									   char *name, char *strtab, long sstrsz);

/*
 * Dynamic symbols from shared library resolution
 */

long				get_got_addr(pid_t pid, Elf64_Dyn **dyn);
struct link_map		*elf_linkmap(pid_t pid, Elf64_Dyn *got);
t_tables_addr		*elf_tables(pid_t pid, struct link_map *link_map);
char				*elf_addr_symbol(pid_t pid, struct link_map *link_map,
							t_tables_addr *tables, unsigned long sym_addr);
long				elf_symbol_addr(pid_t pid, struct link_map *link_map,
									t_tables_addr *s_tables, char *symbol);
Elf64_Sym			*elf_addr_dynsym_sym(pid_t pid, struct link_map *link_map,
										 t_tables_addr *s_tables,
										 unsigned long sym_addr); 
int					elf_populate_dynsym(pid_t pid, struct link_map *link_map,
										Elf64_Sym **dynsym, char *dynstr,
										unsigned strsz);


/*
 * From an ELF File file descriptor (not present in process memory)
 * ============================================================================
 */

Elf64_Shdr			**elf_file_shdr(int fd, Elf64_Ehdr *e_hdr);
Elf64_Shdr			*elf_shdr_name(Elf64_Shdr **shdr, Elf64_Word type,
								   char *name, char *strtab, long sstrsz);
Elf64_Shdr			*elf_shdr_type(Elf64_Shdr **shdr, Elf64_Word type);
Elf64_Sym			**elf_file_symtab(int fd, Elf64_Shdr *s_hdr);
char				*elf_file_shstrtab(int fd, Elf64_Shdr **s_hdr, char *strtab,
									   Elf64_Xword sstrsz, Elf64_Xword *size);
char				*elf_file_strtab(int fd, Elf64_Shdr *s_hdr);
Elf64_Sym			**elf_file_symtab(int fd, Elf64_Shdr *s_hdr);

#endif
