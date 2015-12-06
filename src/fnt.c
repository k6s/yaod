#include <see_stack.h>

void					fnt_push(t_fnt **r, t_fnt *fnt)
{
	if (!*r)
		*r = fnt;
	else
	{
		fnt->prv = *r;
		*r = fnt;
	}
}

void					fnt_prev(t_fnt **r)
{
	t_fnt				*fnt;

	fnt = *r;
	*r = (*r)->prv;
	free(fnt->sym);
	free(fnt);
}

Elf64_Sym				*fnt_sym(Elf64_Sym **sym, long addr)
{
	size_t				i;

	i = 0;
	while (sym[i])
	{
		if (ELF64_ST_TYPE(sym[i]->st_info) == STT_FUNC)
		{
			if (sym[i]->st_value == addr)
				return (sym[i]);
			if (sym[i]->st_size)
			{
				if (addr >= sym[i]->st_value
					&& addr < sym[i]->st_value + sym[i]->st_size)
				return (sym[i]);
			}
		}
		++i;
	}
	return (NULL);
}

int					fnt_shared_sym(pid_t pid, struct link_map *lm, long addr,
								   t_fnt *fnt)
{
	t_tables_addr	*tables;

	while (lm)
	{
		if ((tables = elf_tables(pid, lm)))
		{
			if ((fnt->sym = elf_addr_dynsym_sym(pid, lm, tables, addr)))
			{
				fnt->type = FNT_SHA;
				fnt->name = get_str(pid, tables->strtab + fnt->sym->st_name);
				fnt->sym->st_value += lm->l_addr;
				free(tables->nchains);
				free(tables);
				return (0);
			}
			free(tables);
		}
		lm = lm->l_next;
	}
	return (-1);
}
/*
int					fnt_shared_sym_nosz(pid_t pid, struct link_map *lm,
										long addr, t_fnt *fnt)
{
	t_tables_addr	*tables;

	while (lm)
	{
		if ((tables = elf_tables(pid, lm)))
		{
			if ((fnt->sym = elf_addr_dynsym_sym(pid, lm, tables, addr)))
			{
				fnt->type = FNT_SHA;
				fnt->name = get_str(pid, tables->strtab + fnt->sym->st_name);
				free(tables->nchains);
				free(tables);
				return (0);
			}
			free(tables);
		}
		lm = lm->l_next;
	}
	return (-1);
}
*/
Elf64_Sym			*fnt_sym_nosz(Elf64_Sym **symtab, long *off, long addr)
{
	Elf64_Sym		*sym;
	size_t			i;

	i = 0;
	sym = NULL;
	while (symtab[i])
	{
		if (ELF64_ST_TYPE(symtab[i]->st_info) == STT_FUNC
			&& symtab[i]->st_value)
		{
			if (!(long)symtab[i]->st_size && addr > (long)symtab[i]->st_value)
			{
				if (addr - (long)symtab[i]->st_value < *off)
				{
					sym = symtab[i];
					*off = addr - symtab[i]->st_value;
				}
			}
		}
		++i;
	}
	return (sym);
}

Elf64_Sym			*fnt_nosz(pid_t pid, t_elf *elf, long addr, t_fnt *fnt)
{
	Elf64_Sym		*sym;
	Elf64_Sym		*new_sym;
	long			off;

	sym = NULL;
	off = LONG_MAX - 1;
	if (elf->symtab)
		sym = fnt_sym_nosz(elf->symtab, &off, addr);
	if (elf->dynsym)
		new_sym = fnt_sym_nosz(elf->dynsym, &off, addr);
	if (new_sym)
	{
		fnt->name = elf_symstr(elf->dynstr, new_sym->st_name, elf->strsz);
		fnt->type = FNT_DYN;
		fnt->sym = sym;
	}
	else if (sym)
	{
		fnt->name = elf_symstr(elf->shstrtab, sym->st_name, elf->shstrsz);
		fnt->type = FNT_STA_NOSZ;
		fnt->sym = sym;
	}
	return (new_sym ? new_sym : sym);
}

t_fnt				*fnt_new(pid_t pid, t_elf *elf, long addr)
{
	t_fnt			*fnt;

	fnt = NULL;
	if (!(fnt = malloc(sizeof(*fnt))))
		return (NULL);
	memset(fnt, 0, sizeof(*fnt));
	if (elf->dynsym && (fnt->sym = fnt_sym(elf->dynsym, addr)))
	{
		fnt->name = elf_symstr(elf->dynstr, fnt->sym->st_name, elf->strsz);
		fnt->type = FNT_DYN;
	}
	if (!fnt->sym && elf->symtab && (fnt->sym = fnt_sym(elf->symtab, addr)))
	{
		fnt->name = elf_symstr(elf->shstrtab, fnt->sym->st_name, elf->shstrsz);
		fnt->type = FNT_STA;
	}
	if (!fnt->sym && elf->link_map)
		fnt_shared_sym(pid, elf->link_map, addr, fnt);
	if (!fnt->sym)
		fnt->sym = fnt_nosz(pid, elf, addr, fnt);
	if (fnt->sym)
		fnt->end = fnt->sym->st_value + fnt->sym->st_size;
	return (fnt);
}

/*
t_fnt					*fnt_new(pid_t pid, struct link_map *lm, long addr)
{
	Elf64_Sym			*sym;
	t_tables			*tables;
	t_fnt				*fnt;

	sym = NULL;
	tables = NULL;
	fnt = NULL;
	while (!sym && lm)
	{
		free(tables);
		if ((tables = elf_tables(pid, lm)))
		{
			if (!(sym = elf_addr_dynsym_sym(pid, lm, tables, addr)))
				lm = lm->nxt;
		}
	}
	fnt = NULL;
	if (sym)
	{
		if (!(fnt = malloc(sizeof(*fnt))))
			return (NULL);
		fnt->sym = sym;
		fnt->name = get_str(pid, tables->strtab + sym->st_name);
	}
	free(tables);
	return (fnt);
}
*/