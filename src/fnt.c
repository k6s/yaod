#include <see_stack.h>

void					fnt_free(t_fnt *fnt)
{
	if (fnt->type & FNT_PLT || fnt->type & FNT_SHA || fnt->type & FNT_UNKNOWN)
		free(fnt->sym);
	free(fnt->name);
	free(fnt);
}

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
	if (fnt->type & FNT_SHA || fnt->type & FNT_UNKNOWN)
		free(fnt->sym);
	free(fnt->name);
	free(fnt);
}

void					fnt_set(t_fnt *fnt, Elf64_Sym *sym, int type,
								char *strtab, Elf64_Xword strsz)
{
	fnt->name = elf_symstr(strtab, sym->st_name, strsz);
	fnt->type = type;
	fnt->sym = sym;
}

Elf64_Sym				*fnt_sym(Elf64_Sym **sym, unsigned long addr)
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

int					fnt_shared_sym(pid_t pid, t_elf_sha *sha, u_long addr,
								   t_fnt *fnt)
{
	while (sha)
	{
		if (sha->dyntabs && addr > sha->lm->l_addr)
		{
			if ((fnt->sym = elf_addr_dynsym_sym(pid, sha->lm, sha->dyntabs,
												addr)))
			{
				fnt->type = FNT_SHA | FNT_DYN;
				fnt->name = (char *)get_str(pid, sha->dyntabs->strtab
											+ fnt->sym->st_name);
				fnt->sym->st_value += sha->lm->l_addr;
				return (0);
			}
		}
		if (sha->symtabs && (fnt->sym = elf_sha_sym(sha->fd, sha->symtabs,
												   sha->lm->l_addr, addr)))
		{
			fnt->name = read_str(sha->fd,
								 sha->symtabs->strtab + fnt->sym->st_name);
			fnt->type = FNT_SHA | FNT_STA;
			return (0);
		}
		sha = sha->nxt;
	}
	return (-1);
}

int					fnt_shared_sym_nosz(pid_t pid, t_elf_sha *sha, u_long addr,
										t_fnt *fnt, off_t off)
{
	char			found;

	found = 1;
	while (sha)
	{
		if (sha->dyntabs && addr > sha->lm->l_addr)
		{
			if ((fnt->sym = elf_addr_dynsym_sym_nosz(pid, sha->lm, sha->dyntabs,
													 addr, off)))
			{
				fnt->type = FNT_SHA | FNT_DYN;
				fnt->name = get_str(pid, sha->dyntabs->strtab
									+ fnt->sym->st_name);
				found = 0;
			}
		}
		sha = sha->nxt;
	}
	return (found);
}

Elf64_Sym			*fnt_sym_nosz(Elf64_Sym **symtab, u_long *off, u_long addr)
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
			if (!symtab[i]->st_size && addr > symtab[i]->st_value)
			{
				if (addr - symtab[i]->st_value < *off)
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

t_fnt				*fnt_unknown(t_fnt *fnt, u_long rip)
{
	if (!(fnt->sym = malloc(sizeof(*fnt->sym))))
		return (NULL);
	fnt->sym->st_value = rip;
	fnt->sym->st_size = 0;
	fnt->end = rip;
	fnt->type = FNT_UNKNOWN;
	return (fnt);
}

t_fnt				*fnt_set_plt(t_elf *elf, t_fnt *fnt)
{
	if ((fnt->name = elf_symstr(elf->dynstr, fnt->sym->st_name, elf->strsz)))
	{
		if (!(fnt->name = realloc(fnt->name, strlen(fnt->name) + 5)))
			return (NULL);
		fnt->name[strlen(fnt->name) + 4] = 0;
		memcpy(fnt->name + strlen(fnt->name), "@plt", 4);
		fnt->type = FNT_PLT | FNT_LOC;
		fnt->end = fnt->sym->st_value + 16;
	}
	return (fnt);
}

Elf64_Sym			*fnt_plt_sym(t_elf *elf, u_long addr, u_long *end)
{
	Elf64_Shdr		*sh_dyn;
	Elf64_Shdr		*sh_plt;
	size_t			sym_idx;
	Elf64_Sym		*sym;

	if ((sh_plt = elf_shdr_name(elf->s_hdr, SHT_PROGBITS, ".plt", elf->strtab,
								elf->sstrsz)))
	{
		if (addr >= sh_plt->sh_addr && addr < sh_plt->sh_addr + sh_plt->sh_size)
		{
			sym_idx = (addr - sh_plt->sh_addr) / sh_plt->sh_entsize;
			if ((sh_dyn = elf_shdr_type(elf->s_hdr, SHT_DYNSYM))
				&& sym_idx < sh_dyn->sh_size / sh_dyn->sh_entsize)
			{
				*end = elf->dynsym[sym_idx]->st_value + sh_dyn->sh_entsize;
				if (!(sym = malloc(sizeof(*sym))))
					return (NULL);
				memcpy(sym, elf->dynsym[sym_idx], sizeof(Elf64_Sym));
				sym->st_value = sh_plt->sh_addr + sym_idx * sh_plt->sh_entsize;
				sym->st_size = 16;
				return (sym);
			}
		}
	}
	return (NULL);
}


int					is_fnt_local(t_elf *elf, u_long addr)
{
	Elf64_Shdr			*txt_shdr;

	if ((txt_shdr = elf_shdr_name(elf->s_hdr, SHT_PROGBITS, ".text",
								  elf->strtab, elf->strsz)))
	{
		if (addr < txt_shdr->sh_addr + txt_shdr->sh_size
			&& addr >= txt_shdr->sh_addr)
			return (0);
	}
	return (1);
}

Elf64_Sym			*fnt_stripped(t_fsym *fsym, u_long addr)
{
	Elf64_Sym		*sym;

	sym = NULL;
	while (fsym && fsym->addr < addr)
		fsym = fsym->nxt;
	if (fsym && fsym->prv)
	{
		if (!(sym = malloc(sizeof(*sym))))
			return (NULL);
		memset(sym, 0, sizeof(*sym));
		sym->st_size = fsym->addr - fsym->prv->addr;
		sym->st_value = fsym->prv->addr;
	}
	return (sym);
}

void				fnt_local(t_elf *elf, u_long addr, t_fnt *fnt)
{
	u_long			off;

	off = LONG_MAX - 1;
	if (!fnt->sym && elf->symtab && (fnt->sym = fnt_sym(elf->symtab, addr)))
	{
		fnt->name = elf_symstr(elf->shstrtab, fnt->sym->st_name, elf->shstrsz);
		fnt->type = FNT_STA | FNT_LOC;
	}
	else if (!fnt->sym)
		fnt->sym = fnt_stripped(elf->fsym, addr);
	if (!fnt->sym && elf->symtab)
	{
		if ((fnt->sym = fnt_sym_nosz(elf->symtab, &off, addr)))
			fnt_set(fnt, fnt->sym, FNT_STA | FNT_LOC, elf->shstrtab,
					elf->shstrsz);
	}
}

Elf64_Sym			*fnt_nosz(pid_t pid, t_elf *elf, u_long addr, t_fnt *fnt)
{
	Elf64_Sym		*sym;
	u_long			off;

	sym = NULL;
	off = LONG_MAX - 1;
	if (elf->dynsym)
		sym = fnt_sym_nosz(elf->dynsym, &off, addr);
	if (elf->link_map)
	{
		if (!fnt_shared_sym_nosz(pid, elf->sha, addr, fnt, off))
			return (fnt->sym);
	}
	if (sym)
		fnt_set(fnt, sym, FNT_DYN | FNT_LOC, elf->dynstr, elf->strsz);
	return (sym);
}

void				fnt_extern(pid_t pid, t_elf *elf, u_long addr,
							   t_fnt *fnt)
{
	if (!fnt->sym && elf->dynsym)
	{
		if ((fnt->sym = fnt_sym(elf->dynsym, addr)))
			fnt_set(fnt, fnt->sym, FNT_DYN | FNT_LOC, elf->dynstr, elf->strsz);
	}
	if (!fnt->sym && elf->link_map)
		fnt_shared_sym(pid, elf->sha, addr, fnt);
	if (!fnt->sym)
		fnt->sym = fnt_nosz(pid, elf, addr, fnt);
}

t_fnt					*fnt_new(pid_t pid, t_elf *elf, u_long addr)
{
	t_fnt				*fnt;

	fnt = NULL;
	if (!(fnt = malloc(sizeof(*fnt))))
		return (NULL);
	memset(fnt, 0, sizeof(*fnt));
	if (elf->s_hdr && elf->dynsym
		&& (fnt->sym = fnt_plt_sym(elf, addr, &fnt->end)))
		return (fnt_set_plt(elf, fnt));
	if (!is_fnt_local(elf, addr))
		fnt_local(elf, addr, fnt);
	else
		fnt_extern(pid, elf, addr, fnt);
	if (fnt->sym)
		fnt->end = fnt->sym->st_value + fnt->sym->st_size;
	else
		fnt = fnt_unknown(fnt, addr);
	return (fnt);
}

static int				is_fnt_ret(pid_t pid, unsigned long rip)
{
	struct cs_insn		*ins;
	int					ret;

	ret = 1;
	if (get_code(pid, rip, 1, &ins, NULL) == 1)
	{
		/* retq */
		if (ins->size == 1 && ins->bytes[0] == 0xc3)
			ret = 0;
		/* repz retq */
		else if (ins->size == 2 && ins->bytes[0] == 0xf3
				 && ins->bytes[1] == 0xc3)
			ret = 0;
		free(ins);
	}
	return (ret);
}

static int		fnt_ret_sym(pid_t pid, t_elf *elf,
							struct user_regs_struct *regs, t_fnt **fnt_lst)
{
	t_fnt		*fnt;

	if (!(fnt = fnt_new(pid, elf, regs->rip)))
		return (-1);
	if (!*fnt_lst)
		*fnt_lst = fnt;
	else if ((*fnt_lst)->sym->st_value != fnt->sym->st_value)
		fnt_push(fnt_lst, fnt);
	else
		fnt_free(fnt);
	return (0);
}

int				fnt_ret(pid_t pid, t_elf *elf, struct user_regs_struct *regs,
						t_fnt **fnt_lst)
{
	if ((*fnt_lst) && (*fnt_lst)->prv && (*fnt_lst)->prv->sym)
	{
		if (!is_fnt_ret(pid, (*fnt_lst)->rip))
		{
			while (*fnt_lst && ((*fnt_lst)->type & FNT_JMP))
				fnt_prev(fnt_lst);
			fnt_prev(fnt_lst);
			while (*fnt_lst && ((*fnt_lst)->type & FNT_JMP))
				fnt_prev(fnt_lst);
/*			fnt_ret_sym(pid, elf, regs, fnt_lst); */
			return (0);
		}
	}
	return (1);
}

int					fnt_same(t_fnt **fnt_lst, t_fnt *fnt, unsigned long rip,
							  unsigned long rbp)
{
	if (fnt->sym && (*fnt_lst) && (*fnt_lst)->sym)
	{
		if ((fnt->sym->st_value == (*fnt_lst)->sym->st_value)
			|| (fnt->type & FNT_UNKNOWN && (*fnt_lst)->type & FNT_UNKNOWN))
		{
			fnt_free(fnt);
			if (!(*fnt_lst)->sym->st_size)
				(*fnt_lst)->end = rip;
			(*fnt_lst)->rbp = rbp;
			(*fnt_lst)->rip = rip;
			return (0);
		}
	}
	return (1);
}

static int			fnt_call(pid_t pid, unsigned long rip)
{
	struct cs_insn	*ins;
	int				ret;

	ret = 1;
	if (get_code(pid, rip, 1, &ins, NULL) == 1)
	{
		if (!memcmp(ins->mnemonic, "call", 4))
			ret = 0;
		free(ins);
	}
	return (ret);
}

int					fnt_call_jmp(pid_t pid, t_fnt **fnt_lst, t_fnt *fnt,
								 unsigned long rip, unsigned long rbp)
{
	if (!fnt || (!fnt->name && !(fnt->name = strdup("???"))))
		return (-1);
	if (fnt->sym)
		fnt->end = fnt->sym->st_value + fnt->sym->st_size;
	fnt->rbp = rbp;
	fnt->rip = rip;
	if (*fnt_lst && fnt_call(pid, (*fnt_lst)->rip))
		fnt->type |= FNT_JMP;
	fnt_push(fnt_lst, fnt);
	return (0);
}
