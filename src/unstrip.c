#include <elf_parse.h>
#include <capstone/capstone.h>

t_fsym				*fsym_new(t_fsym **fsym, unsigned long addr)
{
	t_fsym			*p;
	t_fsym			*f;

	if ((p = *fsym) && p->addr <= addr)
	{
		while (p->nxt && p->nxt->addr < addr)
			p = p->nxt;
		if (p->addr == addr || (p->nxt && p->nxt->addr == addr))
			return (p);
		if (!(f = calloc(sizeof(*f), 1)))
			return (NULL);
		f->prv = p;
		f->nxt = p->nxt;
		p->nxt = f;
		return (f);
	}
	else
	{
		if (!(f = calloc(sizeof(*f), 1)))
			return (NULL);
		f->nxt = *fsym;
		if (*fsym)
			(*fsym)->prv = f;
		*fsym = f;
	}
	f->addr = addr;
	return (f);
}

void				fsym_free(t_fsym *fsym)
{
	t_fsym			*p;

	while (fsym)
	{
		p = fsym;
		fsym = fsym->nxt;
		free(p);
	}
}

static int32_t		fsym_call_off(unsigned char *bytes)
{
	uint32_t		off;

	off = 0;
	for (int i = 0; i < 4; i++)
		off |= *(unsigned char *)(&bytes[i]) << ((unsigned)i * 8);
	return (off);
}

t_fsym				*fsym_from_call(int pid, Elf64_Shdr *sh_text,
									Elf64_Shdr *sh_plt)
{
	unsigned long	i;
	unsigned long	end_text;
	unsigned long	call_addr;
	struct cs_insn	*ins;
	t_fsym			*fsym;

	fsym = NULL;
	i = sh_text->sh_addr;
	end_text = i + sh_text->sh_size;
	while (i < end_text)
	{
		if ((get_code(pid, i, 1, &ins, NULL)) != 1)
			return (fsym);
		if (ins->bytes[0] == 0xe8)
		{
			/* Call target address is relative to next instruction address */
			call_addr = fsym_call_off(ins->bytes + 1) + i + ins->size;
		   	if (call_addr < sh_plt->sh_addr || call_addr > sh_plt->sh_addr
				+ sh_plt->sh_size)
			{
				if (!(fsym_new(&fsym, call_addr)))
					return (NULL);
			}
		}
		i += ins->size;
		free(ins);
	}
	return (fsym);
}

t_fsym				*fnt_fsym_strpd(int pid, t_elf *elf)
{
	Elf64_Shdr		*sh_text;
	Elf64_Shdr		*sh_plt;

	if (!(sh_text = elf_shdr_name(elf->s_hdr, SHT_PROGBITS, ".text", elf->strtab,
								 elf->strsz)))
		return (NULL);
	if (!(sh_plt = elf_shdr_name(elf->s_hdr, SHT_PROGBITS, ".plt", elf->strtab,
								 elf->strsz)))
		return (NULL);
	return (fsym_from_call(pid, sh_text, sh_plt));
}
