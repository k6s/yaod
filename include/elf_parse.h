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

typedef struct s_elf	t_elf;		/*! @brief typedef to @ref s_elf */

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
	char					*dynstr;
	Elf64_Xword				strsz;
	char					*strtab;
	Elf64_Xword				sstrsz;
	/* 
	 * Not the original hash table value since hash table 'disapeared'.
	 * DT_PLTRELSZ / sizeof(Elf64_Sym) + 1 (for 1st NULL dynsym entry)
	 */
	Elf64_Word				nchains;
	struct link_map			*link_map;
	char					linked;
	char					stripped;
};

/*! \brief Typedef to @ref s_tables_addr */
typedef struct s_tables_addr		t_tables_addr;

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

/*!
 * \brief Get process Elf64 Elf Header.
 * \param pid Process ID.
 * \return An alloced and filled Elf64_Ehdr structure or NULL in case of
 * error.
 */
Elf64_Ehdr				*elf_ehdr(pid_t pid);

/*!
 * \brief Get a process Elf64 Program Header.
 * \param pid Process ID.
 * \param e_hdr Process's Elf Header.
 * \param type Program Header type (PT_*).
 * \return A alloced and filled Elf64_Phdr structure or NULL in case of error.
 */
Elf64_Phdr				*elf_phdr_entry(pid_t pid, Elf64_Ehdr *e_hdr,
										unsigned type);
Elf64_Phdr				**elf_phdr(pid_t pid, Elf64_Ehdr *e_hdr);

Elf64_Dyn				**elf_dyn(pid_t pid, Elf64_Phdr *p_hdr);

char					*elf_symstr(char *strtab, size_t off, Elf64_Xword size);

Elf64_Sym				**elf_dynsym(pid_t pid, Elf64_Dyn **dynsym,
									 Elf64_Word nchains, char *linked);

char					*elf_strtab(pid_t pid, Elf64_Dyn **dyn,
								   	Elf64_Xword *strsz);

/*!
 * \brief Get process's GOT address.
 * \param pid Process ID.
 * \param e_hdr Process's ID.
 * \return GOT address or -1 in case of error. 
 */
/* long					get_got_addr(pid_t pid, Elf64_Ehdr *e_hdr); */
long						get_got_addr(pid_t pid, Elf64_Dyn **dyn);

/*!
 * \brief Get process's link_map.
 * \param pid Process ID.
 * \param e_hdr Process's Elf Header.
 * \return An alloced and filled link_map chained list or NULL in case of
 * error.
 */
/* struct link_map			*get_linkmap(pid_t pid, Elf64_Ehdr *e_hdr); */
struct link_map				*elf_linkmap(pid_t pid, Elf64_Dyn **dyn);

/*!
 * \brief Get symtab, strtab and nchains from a dynamically loaded library.
 * \param pid Process's ID.
 * \param link_map Process's link_map.
 * \return An alloced and filled @ref s_tables_addr with symtab, strtab address
 * and nchains or NULL in case of error.
 */
t_tables_addr			*elf_tables(pid_t pid, struct link_map *link_map);

/*
 * \brief Get a symbol name from a dynamically loaded library.
 * \param pid Process ID.
 * \param link_map Process link_map element (does not process the list).
 * \param tables @ref s_tables_addr structure filled with proper informations.
 * \param sym_addr Symbol address to lookup.
 * \return Symbol name or NULL in case of error.
 */
char					*elf_addr_symbol(pid_t pid, struct link_map *link_map,
								t_tables_addr *tables, unsigned long sym_addr);
/*
 * \brief Get a symbol addr from a dynamically loaded library.
 * \param pid Process ID.
 * \param link_map Process link_map element (does not process the list).
 * \param tables @ref s_tables_addr structure filled with proper informations.
 * \param symbol Symbol name to look for.
 * \return Symbol address or NULL in case of error.
 */
long					elf_symbol_addr(pid_t pid, struct link_map *link_map,
										t_tables_addr *s_tables, char *symbol);

int						elf_populate_dynsym(pid_t pid,
											struct link_map *link_map,
											Elf64_Sym **dynsym, char *dynstr,
											unsigned strsz);
#endif
