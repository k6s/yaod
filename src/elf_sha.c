#include <elf_parse.h>

static int				set_tables_addr(pid_t pid, Elf64_Dyn *dyn,
										t_tables_addr *tables)
{
	if (dyn->d_tag == DT_SYMTAB)
		tables->symtab = dyn->d_un.d_ptr;
	else if (dyn->d_tag == DT_STRTAB)
		tables->strtab = dyn->d_un.d_ptr;
	else if (dyn->d_tag == DT_HASH)
	{
		if ((tables->nchains
			 = get_data(pid, dyn->d_un.d_ptr + sizeof(Elf64_Word),
						sizeof(Elf64_Word))))
			*tables->nchains = (Elf64_Word)*tables->nchains;
	}
	else
		return (-1);
	return (tables->symtab && tables->strtab && tables->nchains ? 0 : -1);
}

t_tables_addr			*elf_sha_tables(pid_t pid, struct link_map *link_map)
{
	t_tables_addr		*tables;
	Elf64_Dyn			*dyn;
	unsigned long		addr;

	if (!(tables = malloc(sizeof(*tables))))
		return (NULL);
	memset(tables, 0, sizeof(*tables));
	addr = (unsigned long)link_map->l_ld;
	if ((dyn = get_data(pid, addr, sizeof(*dyn))))
	{
		while (dyn->d_tag)
		{
			if (!set_tables_addr(pid, dyn, tables))
			{
				free(dyn);
				return (tables);
			}
			addr += sizeof(*dyn);
			free(dyn);
			if (!(dyn = get_data(pid, addr, sizeof(*dyn))))
				return (NULL);
		}
		free(dyn);
	}
	return (tables);
}

Elf64_Sym			*elf_addr_dynsym_sym(pid_t pid, struct link_map *link_map,
						 t_tables_addr *s_tables, unsigned long sym_addr)
{
	size_t		i;
	Elf64_Sym	*sym;

	i = 0;
	if (s_tables->nchains && s_tables->symtab)
	{
		while (i < *s_tables->nchains)
		{
			if (!(sym = get_data(pid, s_tables->symtab + i * sizeof(*sym),
								 sizeof(*sym))))
				return (NULL);
			if (ELF64_ST_TYPE(sym->st_info) == STT_FUNC)
			{
				if (link_map->l_addr + sym->st_value == sym_addr)
					return (sym);
				else if (sym->st_size)
				{
					if (link_map->l_addr + sym->st_value <= sym_addr
						&& link_map->l_addr + sym->st_value + sym->st_size
						> sym_addr)
						return (sym);
				}
			}
			free(sym);
			++i;
		}
	}
	return (NULL);
}


u_char			*elf_addr_dynsym(pid_t pid, struct link_map *link_map,
						 t_tables_addr *s_tables, unsigned long sym_addr)
{
	size_t		i;
	Elf64_Sym	*sym;

	i = 0;
	if (s_tables->nchains)
	{
		while (i < *s_tables->nchains)
		{
			if (!(sym = get_data(pid, s_tables->symtab + i * sizeof(*sym),
								 sizeof(*sym))))
				return (NULL);
			if (ELF64_ST_TYPE(sym->st_info) == STT_FUNC
				&& link_map->l_addr + sym->st_value == sym_addr)
			{
				free(sym);
				return (get_str(pid, s_tables->strtab + sym->st_name));
			}
			++i;
		}
	}
	return (NULL);
}

long					elf_dynsym_addr(pid_t pid, struct link_map *link_map,
										t_tables_addr *s_tables, char *symbol)
{
	size_t				i;
	Elf64_Sym			*sym;
	long				ret;
	u_char				*s;

	i = 0;
	ret = 0;
	if (s_tables->nchains && !ret)
	{
		while (i < *s_tables->nchains)
		{
			if (!(sym = get_data(pid, s_tables->symtab + i * sizeof(*sym),
								 sizeof(*sym))))
				return (0);
			if ((s = get_str(pid, (long)s_tables->strtab + sym->st_name)))
			{
				if (!strcmp(symbol, (char *)s))
					ret = link_map->l_addr + sym->st_value;
			}
			free(sym);
			free(s);
			++i;
		}
	}
	return (ret);
}

t_elf_sha			*elf_sha_new(int pid, struct link_map *lm)
{
	t_elf_sha		*sha;

	if (!(sha = malloc(sizeof(*sha))))
		return (NULL);
	sha->lm = lm;
	if ((sha->fd = open(lm->l_name, O_RDONLY)) < 0)
	{
		free(sha);
		return (NULL);
	}
	sha->dyntabs = elf_sha_tables(pid, lm);
	if (!(sha->e_hdr = elf_sha_ehdr(sha->fd)))
	{
		sha->symtabs = NULL;
		close(sha->fd);
		sha->fd = -1;
	}
	else
		sha->symtabs = elf_file_sha_tables(sha->fd, sha->e_hdr);
	sha->nxt = NULL;
	return (sha);
}

void				elf_sha_append(t_elf_sha **r, t_elf_sha *sha)
{
	t_elf_sha		*p;

	if (!(p = *r))
		*r = sha;
	else
	{
		while (p->nxt)
			p = p->nxt;
		p->nxt = sha;
	}
}

void				elf_sha_free(t_elf_sha *sha)
{
	t_elf_sha		*p;

	while (sha)
	{
		p = sha;
		sha = sha->nxt;
		free(p->dyntabs->nchains);
		free(p->dyntabs);
		free(p);
	}
}

t_elf_sha			*elf_sha(int pid, struct link_map *lm)
{
	t_elf_sha		*sha_lst;
	t_elf_sha		*sha;

	sha_lst = NULL;
	while (lm)
	{
		if (lm->l_name && *lm->l_name)
		{
			if ((sha = elf_sha_new(pid, lm)))
			   	elf_sha_append(&sha_lst, sha);	
		}
		lm = lm->l_next;
	}
	return (sha_lst);
}


Elf64_Sym			*elf_addr_dynsym_sym_nosz(pid_t pid,
						struct link_map *link_map,
						t_tables_addr *s_tables, unsigned long sym_addr,
						unsigned long off)
{
	size_t			i;
	Elf64_Sym		*sym;
	Elf64_Sym		*ret_sym;

	i = 0;
	ret_sym = NULL;
	if (s_tables->nchains && s_tables->symtab)
	{
		while (i < *s_tables->nchains)
		{
			if (!(sym = get_data(pid, s_tables->symtab + i * sizeof(*sym),
								 sizeof(*sym))))
				return (NULL);
			if (ELF64_ST_TYPE(sym->st_info) == STT_FUNC)
			{
				if (!sym->st_size && sym_addr > sym->st_value + link_map->l_addr
					&& sym->st_value + link_map->l_addr - sym_addr < off)
				{
					ret_sym = sym;
					off = sym->st_value + link_map->l_addr - sym_addr;
				}
			}
			++i;
		}
	}
	return (ret_sym);
}

static u_long		get_dynsym(pid_t pid, struct link_map *l_map, char *name)
{
	unsigned long	val;
	t_tables_addr	*tables;

	val = 0;
	while (l_map && !val)
	{
		if ((tables = elf_sha_tables(pid, l_map)))
		{
			val = elf_dynsym_addr(pid, l_map, tables, name);
			free(tables->nchains);
			free(tables);
		}
		l_map = l_map->l_next;
	}
	return (val);
}

int					elf_populate_dynsym(pid_t pid, struct link_map *link_map,
										Elf64_Sym **dynsym, char *dynstr,
										unsigned strsz)
{
	char			*name;
	size_t			i;
	int				linked;

	i = 0;
	linked = 0;
	while (dynsym[i])
	{
		if (!dynsym[i]->st_value)
		{
			if ((name = elf_symstr(dynstr, dynsym[i]->st_name, strsz)) && *name)
			{
				if ((dynsym[i]->st_value = get_dynsym(pid, link_map, name)))
					linked = 1;
			}
			free(name);
		}
		++i;
	}
	return (linked);
}
