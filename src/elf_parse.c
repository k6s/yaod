#include <elf_parse.h>

void					elf_free_dyn(Elf64_Dyn **dyn)
{
	size_t				i;

	i = 0;
	if (dyn)
	{
		i = 0;
		while (dyn[i])
			free(dyn[i++]);
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
		free(sym);
	}
}

void					elf_free_shdr(Elf64_Shdr **shdr)
{
	size_t				i;

	i = 0;
	if (shdr)
	{
		while (shdr[i])
			free(shdr[i++]);
		free(shdr);
	}
}

void					elf_free_rela(Elf64_Rela **rel)
{
	size_t				i;

	i = 0;
	if (rel)
	{
		while (rel[i])
			free(rel[i++]);
	}
	free(rel);
}

void					elf_free_linkmap(struct link_map *lm)
{
	struct link_map		*p;

	while (lm)
	{
		p = lm;
		lm = lm->l_next;
		free(p->l_name);
		free(p);
	}
}

void					elf_free_sha(t_elf_sha *sha)
{
	t_elf_sha			*p;

	while (sha)
	{
		free(sha->e_hdr);
		if (sha->dyntabs)
			free(sha->dyntabs->nchains);
		free(sha->dyntabs);
		if (sha->symtabs)
			free(sha->symtabs->nchains);
		free(sha->symtabs);
		p = sha;
		sha = sha->nxt;
		free(p);
	}
}

int						elf_free(t_elf *elf)
{
	if (elf)
	{
		elf_free_linkmap(elf->link_map);
		elf_free_dyn(elf->dyn);
		elf_free_sym(elf->dynsym);
		elf_free_sym(elf->symtab);
		elf_free_phdr(elf->p_hdr);
		elf_free_shdr(elf->s_hdr);
		elf_free_rela(elf->rela_plt);
		elf_free_sha(elf->sha);
		fsym_free(elf->fsym);
		free(elf->strtab);
		free(elf->shstrtab);
		free(elf->dynstr);
		free(elf->e_hdr);
		free(elf);
	}
	return (0);
}

int						elf_valid_magic(u_char *magic)
{
	if (magic[EI_MAG0] != ELFMAG0)
		return (-1);
	if (magic[EI_MAG1] != ELFMAG1)
		return (-1);
	if (magic[EI_MAG2] != ELFMAG2)
		return (-1);
	if (magic[EI_MAG3] != ELFMAG3)
		return (-1);
	return (0);
}

int						elf_x64_valid(Elf64_Ehdr *e_hdr)
{
	if (elf_valid_magic(e_hdr->e_ident))
		return (-1);
	if (e_hdr->e_ident[EI_CLASS] != ELFCLASS64)
		return (-1);
	if (e_hdr->e_ident[EI_DATA] != ELFDATA2LSB)
		return (-1);
	if (e_hdr->e_type != ET_EXEC)
		return (-1);
	return (0);
}

t_elf					*elf_get(pid_t pid, char *filename)
{
	t_elf				*elf;

	if (!(elf = malloc(sizeof(*elf))))
		return (NULL);
	memset(elf, 0, sizeof(*elf));
	if ((elf->e_hdr = elf_ehdr(pid)))
	{
		if (elf_x64_valid(elf->e_hdr))
		{
			free(elf->e_hdr);
			free(elf);
			return (NULL);
		}
		if (elf_from_file(filename, elf))
		{
			elf->stripped = 1;
			elf->fsym = fnt_fsym_strpd(pid, elf);
			t_fsym	*p = elf->fsym;
			while (p)
				p = p->nxt;
		}
		return (elf_from_process(pid, elf));
	}
	elf_free(elf);
	return (NULL);
}
