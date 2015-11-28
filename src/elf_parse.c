/**
 * @file elf_parse.c
 * @brief Parse slave ELF64 file information.
 * @author k6
 */
#include <elf_parse.h>

Elf64_Ehdr				*elf_ehdr(pid_t pid)
{
	Elf64_Ehdr			*e_hdr;

	if (!(e_hdr = get_data(pid, 0x400000, sizeof(Elf64_Ehdr))))
		return (NULL);
	if (!(e_hdr->e_entry & 0x400000)
		|| (e_hdr->e_entry & 0xff00000000000000))
	{
		free(e_hdr);
		return (NULL);
	}
	return (e_hdr);
}

Elf64_Shdr				**elf_file_shdr(int fd, Elf64_Ehdr *e_hdr)
{
	Elf64_Shdr			**s_hdr;
	size_t				i;

	i = 0;
	if (lseek(fd, e_hdr->e_shoff, SEEK_SET) == -1)
		return (NULL);
	if (!(s_hdr = malloc(sizeof(*s_hdr) * (e_hdr->e_shnum + 1))))
		return (NULL);
	bzero(s_hdr, sizeof(*s_hdr) * (e_hdr->e_shnum + 1));
	while (i < e_hdr->e_shnum)
	{
		if (!(s_hdr[i] = malloc(sizeof(**s_hdr))))
			return (NULL);
		if (read(fd, s_hdr[i], e_hdr->e_shentsize) != e_hdr->e_shentsize)
		{
			free(s_hdr);
			return (NULL);
		}
		++i;
	}
	return (s_hdr);
}

char				*elf_file_strtab(int fd, Elf64_Ehdr *e_hdr,
									 Elf64_Shdr **s_hdr)
{
	ssize_t			size;
	char			*strtab;

	size = s_hdr[e_hdr->e_shstrndx]->sh_size;
	if (!(strtab = malloc(sizeof(*strtab) * size)))
		return (NULL);
	if (lseek(fd, s_hdr[e_hdr->e_shstrndx]->sh_offset, SEEK_SET) == -1)
		return (NULL);
	if ((read(fd, strtab, size)) != size)
	{
		free(strtab);
		return (NULL);
	}
	return (strtab);
}

ssize_t					elf_file(char *filename, t_elf *elf)
{
	int					fd;

	if ((fd = open(filename, O_RDONLY)) > 0)
	{
		if (!(elf->s_hdr = elf_file_shdr(fd, elf->e_hdr)))
			return (-1);
		if (!(elf->strtab = elf_file_strtab(fd, elf->e_hdr, elf->s_hdr)))
			return (-1);
		elf->sstrsz = elf->s_hdr[elf->e_hdr->e_shstrndx]->sh_size;
		close(fd);
		return (0);
	}
	return (-1);
}

Elf64_Phdr				*elf_phdr_entry(pid_t pid, Elf64_Ehdr *e_hdr,
										unsigned type)
{
	Elf64_Phdr			*p_hdr;
	ssize_t				i;

	if (!(p_hdr = get_data(pid, e_hdr->e_phoff + 0x400000, sizeof(*p_hdr))))
		return (NULL);
	i = sizeof(*p_hdr);
	while (p_hdr->p_type != type && i < e_hdr->e_phentsize * e_hdr->e_phnum)
	{
		free(p_hdr);
		if (!(p_hdr = get_data(pid, e_hdr->e_phoff + 0x400000 + i,
							   sizeof(*p_hdr))))
			return (NULL);
		i += sizeof(*p_hdr);
	}
	return (p_hdr->p_type == type ? p_hdr : NULL);
}

Elf64_Phdr				**elf_phdr(pid_t pid, Elf64_Ehdr *e_hdr)
{
	Elf64_Phdr			**p_hdr;
	size_t				i;

	i = 0;
	if (!(p_hdr = malloc(sizeof(*p_hdr) * (e_hdr->e_phnum + 1))))
		return (NULL);
	p_hdr[e_hdr->e_phnum] = NULL;
	while (i < e_hdr->e_phnum)
	{
		if (!(p_hdr[i] = get_data(pid, e_hdr->e_phoff + 0x400000
								  + i * e_hdr->e_phentsize,
								  e_hdr->e_phentsize)))
			return (NULL);
		++i;
	}
	return (p_hdr);
}

Elf64_Dyn				**elf_dyn(pid_t pid, Elf64_Phdr *p_hdr)
{
	Elf64_Dyn			**dyn;
	size_t				max;
	size_t				i;
	Elf64_Sxword		dtag;

	max = p_hdr->p_memsz / sizeof(Elf64_Dyn);
	i = 0;
	if (!(dyn = malloc(sizeof(*dyn) * (max + 1))))
		return (NULL);
	dtag = 1;
	bzero(dyn, sizeof(*dyn) * (max + 1));
	while (dtag && i < max)
	{
		if (!(dyn[i] = get_data(pid, p_hdr->p_vaddr + i * sizeof(Elf64_Dyn),
								sizeof(Elf64_Dyn))))
			return (NULL);
		dtag = dyn[i]->d_tag;
		++i;
	}
	return (dyn);
}

Elf64_Sym				*elf_sym(pid_t pid, Elf64_Dyn *dyn, size_t off)
{
	return (get_data(pid, dyn->d_un.d_ptr + off * sizeof(Elf64_Sym),
					 sizeof(Elf64_Sym)));
}

Elf64_Word				elf_hash_entry(pid_t pid, Elf64_Dyn *h_dyn, size_t i)
{
	Elf64_Word			val;
	Elf64_Word			*t;

	val = -1;
	if ((t = get_data(pid, h_dyn->d_un.d_ptr + i * sizeof(val), sizeof(val))))
	{
		val = *t;
		free(t);
	}
	return (val);
}	

void					elf_free_dyn(Elf64_Dyn **dyn)
{
	size_t				i;

	i = 0;
	if (dyn)
	{
		i = 0;
		while (dyn[i])
			free(dyn[i++]);
		free(dyn[i]);
		free(dyn);
	}
}

void					elf_free_phdr(Elf64_Phdr **p_hdr)
{
	size_t				i;

	i = 0;
	if (p_hdr)
	{
		i = 0;
		while (p_hdr[i])
			free(p_hdr[i++]);
		++i;
		free(p_hdr[i]);
		free(p_hdr);
	}
}

void					elf_free_sym(Elf64_Sym **sym)
{
	size_t				i;

	i = 0;
	if (sym)
	{
		i = 0;
		while (sym[i])
			free(sym[i++]);
		free(sym[i]);
		free(sym);
	}
}

int						elf_free(t_elf *elf)
{
	elf_free_dyn(elf->dyn);
	elf_free_sym(elf->dynsym);
	free(elf->e_hdr);
	free(elf);
	return (0);
}

char					*elf_symstr(char *strtab, size_t off, Elf64_Xword size)
{
	if (memchr(strtab + off, 0, size - off))
		return (strdup(strtab + off));
	return (NULL);
}

Elf64_Sym				**elf_dynsym(pid_t pid, Elf64_Dyn **dynsym,
									 Elf64_Word nchains, char *linked)
{
	Elf64_Sym			**sym;
	size_t				i;
	size_t				j;

	i = 0;
	while (dynsym[i] && dynsym[i]->d_tag != DT_SYMTAB)
		++i;
	if (!dynsym[i])
		return (NULL);
	if (!(sym = malloc(sizeof(*sym) * (nchains + 1))))
		return (NULL);
	bzero(sym, sizeof(*sym) * (nchains + 1));
	*linked = 0;
	j = 0;
	while (j < nchains && (sym[j] = elf_sym(pid, dynsym[i], j)))
	{
		if (sym[j]->st_value)
			*linked = 1;
		++j;
	}
	return (sym);
}

char					*elf_strtab(pid_t pid, Elf64_Dyn **dyn,
									Elf64_Xword *strsz)
{
	Elf64_Addr			p_strtab = 0;
	size_t				i;

	*strsz = 0;
	i = 0;
	while (dyn[i] && (!*strsz || !p_strtab))
	{
		if (dyn[i]->d_tag == DT_STRSZ)
			*strsz = dyn[i]->d_un.d_val;
		else if (dyn[i]->d_tag == DT_STRTAB)
			p_strtab = dyn[i]->d_un.d_ptr;
		++i;
	}
	if (*strsz && p_strtab)
		return (get_data(pid, p_strtab, *strsz));
	return (NULL);
}

Elf64_Word				elf_pseudo_nchains(Elf64_Dyn **dyn)
{
	size_t				i;

	i = 0;
	while (dyn[i] && dyn[i]->d_tag != DT_PLTRELSZ)
		++i;
	if (dyn[i])
		return (dyn[i]->d_un.d_val / sizeof(Elf64_Sym) + 1);
	return (-1);
}

t_elf					*elf_mem(pid_t pid, t_elf *elf)
{
	size_t				i;

	if ((elf->p_hdr = elf_phdr(pid, elf->e_hdr)))
	{
		i = 0;
		while (i < elf->e_hdr->e_phnum
			   && elf->p_hdr[i]->p_type != PT_DYNAMIC)
			++i;
		if ((elf->dyn = elf_dyn(pid, elf->p_hdr[i])))
		{
			elf->nchains = elf_pseudo_nchains(elf->dyn);
			elf->dynstr = elf_strtab(pid, elf->dyn, &elf->strsz);
			elf->dynsym = elf_dynsym(pid, elf->dyn, elf->nchains,
									 &elf->linked);
			return (elf);
		}
	}
	elf_free(elf);
	return (NULL);
}

t_elf					*elf_get(pid_t pid, char *filename)
{
	t_elf				*elf;

	if (!(elf = malloc(sizeof(*elf))))
		return (NULL);
	bzero(elf, sizeof(*elf));
	if ((elf->e_hdr = elf_ehdr(pid)))
	{
		if (elf_file(filename, elf))
			elf->stripped = 1;
		return (elf_mem(pid, elf));
	}
	elf_free(elf);
	return (NULL);
}

long						elf_got_addr(Elf64_Dyn **dyn)
{
	size_t					i;

	i = 0;
	while (dyn[i])
	{
		if (dyn[i]->d_tag == DT_PLTGOT)
			return (dyn[i]->d_un.d_ptr);
		++i;
	}
	return (0);
}

static struct link_map		*elf_linkmap_base(pid_t pid, Elf64_Dyn **dyn)
{
	long					got_addr;
	long					*got;
	struct link_map			*link_map;

	link_map = NULL;
	if ((got_addr = elf_got_addr(dyn)))
	{
		/* link_map is second entry in GOT */
		got = get_data(pid, got_addr + sizeof(long),
					   sizeof(long));
		link_map = get_data(pid, *got, sizeof(*link_map));
		free(got);
	}
	return (link_map);
}

struct link_map				*elf_linkmap(pid_t pid, Elf64_Dyn **dyn)
{
	struct link_map			*link_map;
	struct link_map			*l;

	link_map = elf_linkmap_base(pid, dyn);
	l = link_map;
	while (link_map)
	{
		link_map->l_name = get_str(pid, (long)link_map->l_name);
		if (link_map->l_next)
			link_map->l_next = get_data(pid, (long)link_map->l_next,
										sizeof(*link_map));
		link_map = link_map->l_next;
	}
	return (l);
}

typedef struct
{
	Elf64_Sxword	d_tag;
	Elf64_Addr		d_ptr;
} MyElf64_Dyn;

t_tables_addr				*elf_tables(pid_t pid, struct link_map *link_map)
{
	t_tables_addr			*tables;
	Elf64_Dyn				*dyn;
	unsigned long			addr;

	if (!(tables = malloc(sizeof(*tables))))
		return (NULL);
	bzero(tables, sizeof(*tables));
	addr = (unsigned long)link_map->l_ld;
	if ((dyn = get_data(pid, addr, sizeof(*dyn))))
	{
		while (dyn->d_tag)
		{
			if (dyn->d_tag == DT_SYMTAB)
				tables->symtab = dyn->d_un.d_ptr;
			if (dyn->d_tag == DT_STRTAB)
				tables->strtab = dyn->d_un.d_ptr;
			if (dyn->d_tag == DT_HASH)
			{
				if ((tables->nchains
					 = get_data(pid, dyn->d_un.d_ptr + sizeof(Elf64_Word),
								sizeof(Elf64_Word))))
					*tables->nchains = (Elf64_Word)*tables->nchains;
			}
			free(dyn);
			addr += sizeof(*dyn);
			if (!(dyn = get_data(pid, addr, sizeof(*dyn))))
				return (NULL);
		}
	}
	return (tables);
}

char					*elf_addr_dynsym(pid_t pid, struct link_map *link_map,
										 t_tables_addr *s_tables, unsigned long sym_addr)
{
	size_t				i;
	Elf64_Sym			*sym;

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
	char				*s;

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
				if (!strcmp(symbol, s))
					ret = link_map->l_addr + sym->st_value;
			}
			free(sym);
			free(s);
			++i;
		}
	}
	return (ret);
}

int					elf_populate_dynsym(pid_t pid, struct link_map *link_map,
										Elf64_Sym **dynsym, char *dynstr,
										unsigned strsz)
{
	char			*name;
	size_t			i;
	struct link_map	*l_map;
	t_tables_addr	*tables;
	int				linked;

	i = 0;
	linked = 0;
	while (dynsym[i])
	{
		if (!dynsym[i]->st_value)
		{
			if ((name = elf_symstr(dynstr, dynsym[i]->st_name, strsz)) && *name)
			{
				l_map = link_map;
				while (l_map && (!dynsym[i]->st_value))
				{
					if ((tables = elf_tables(pid, l_map)))
					{
						if ((dynsym[i]->st_value
							= elf_dynsym_addr(pid, l_map, tables, name)))
							linked = 1;
						free(tables->nchains);
						free(tables);
					}
					l_map = l_map->l_next;
				}
				free(name);
			}
		}
		++i;
	}
	return (linked);
}
