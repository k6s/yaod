#include <see_stack.h>
#include <elf_parse.h>

int					info_linkmap(t_term *s_term, char UN **av)
{
	struct link_map	*link_map;

	link_map = s_term->slave.elf->link_map;
	wattrset(s_term->slave.wins[WIN_SH], A_BOLD);
	wattron(s_term->slave.wins[WIN_SH], COLOR_PAIR(3));
	wprintw(s_term->slave.wins[WIN_SH], "\t\t---= Dynamic libraries =---\n");
	wattron(s_term->slave.wins[WIN_SH], COLOR_PAIR(1));
	wattroff(s_term->slave.wins[WIN_SH], A_BOLD);
	if (link_map && (!link_map->l_name || !*link_map->l_name)
		&& !link_map->l_addr)
		link_map = link_map->l_next;
	while (link_map)
	{
		wprintw(s_term->slave.wins[WIN_SH], "    %p", link_map->l_addr);
		wprintw(s_term->slave.wins[WIN_SH], "    ::     ");
		wprintw(s_term->slave.wins[WIN_SH], "    %s\n", link_map->l_name);
		link_map = link_map->l_next;
	}
	return (0);
}

int					info_sym(Elf64_Sym **sym, WINDOW *win, char *strtab,
							 Elf64_Word strsz)
{
	size_t			i;
	char			*name;
	unsigned char	info;

	i = 0;
	while (sym[i])
	{
		wattrset(win, A_BOLD);
		if ((name = elf_symstr(strtab, sym[i]->st_name, strsz)) && *name)
			wprintw(win, "%-30s\n", name);
		else
			wprintw(win, "Unknown symbol:\n");
		wattroff(win, A_BOLD);
		free(name);
		wprintw(win, "    0x%012lx", sym[i]->st_value);
		wprintw(win, "    0x%012lx", sym[i]->st_size);
		info = ELF64_ST_TYPE(sym[i]->st_info);
		switch (info)
		{
		 case (STT_GNU_IFUNC):
			 wprintw(win, "  %-9s", "IFUNC");
			 break ;
		 case (STT_NOTYPE):
			 wprintw(win, "  %-9s", "NOTYPE");
			 break ;
		 case (STT_OBJECT):
			 wprintw(win, "  %-9s", "OBJECT");
			 break ;
		 case (STT_FUNC):
			 wprintw(win, "  %-9s", "FUNC");
			 break ;
		 case (STT_SECTION):
			 wprintw(win, "  %-9s", "SECTION");
			 break ;
		 case (STT_FILE):
			 wprintw(win, "  %-9s", "FILE");
			 break ;
		 case (STT_COMMON):
			 wprintw(win, "  %-9s", "COMMON");
			 break ;
		 case (STT_TLS):
			 wprintw(win, "  %-9s", "TLS");
			 break ;
		 case (STT_NUM):
			 wprintw(win, "  %-9s", "NUM");
			 break ;
		 case (STT_HIOS):
			 wprintw(win, "  %-9s", "HIOS");
			 break ;
		 case (STT_LOPROC):
			 wprintw(win, "  %-9s", "LOPROC");
			 break ;
		 case (STT_HIPROC):
			 wprintw(win, "  %-9s", "HIPROC");
			 break ;
		 default:
			 wprintw(win, "  %-9s", "UNKNOWN");
			 break ;
		}
		switch (ELF64_ST_BIND(sym[i]->st_info))
		{
		 case (STB_LOCAL):
			 wprintw(win, "%-12s", "LOCAL");
			 break ;
		 case (STB_GLOBAL):
			 wprintw(win, "%-12s", "GLOBAL");
			 break ;
		 case (STB_WEAK):
			 wprintw(win, "%-12s", "WEAK");
			 break ;
		 case (STB_NUM):
			 wprintw(win, "%-12s", "NUM");
			 break ;
		 case (STB_GNU_UNIQUE):
			 wprintw(win, "%-12s", "GNU_UNIQUE");
			 break ;
		 case (STB_HIOS):
			 wprintw(win, "%-12s", "HIOS");
			 break ;
		 case (STB_LOPROC):
			 wprintw(win, "%-12s", "LOPROC");
			 break ;
		 case (STB_HIPROC):
			 wprintw(win, "%-12s", "HIPROC");
			 break ;
		 default:
			 wprintw(win, "%-12s", "UNKNOWN");
			 break ;
		}
		switch (sym[i]->st_other)
		{
		 case (STV_DEFAULT):
			 wprintw(win, "%-12s", "DEFAULT");
			 break ;
		 case (STV_INTERNAL):
			 wprintw(win, "%-12s", "INTERNAL");
			 break ;
		 case (STV_HIDDEN):
			 wprintw(win, "%-12s", "HIDDEN");
			 break ;
		 case (STV_PROTECTED):
			 wprintw(win, "%-11s", "PROTECTED");
			 break ;
		 default:
			 wprintw(win, "%-12s", "UNKNOWN");
			 break ;
		}
		wprintw(win, "%-08x\n", sym[i]->st_shndx);
		++i;
	}
	sh_refresh(win, 0, 0);
	return (0);
}

int				info_dynsym(t_term *s_term, char UN **av)
{
	wattrset(s_term->slave.wins[WIN_SH], A_BOLD);
	wattron(s_term->slave.wins[WIN_SH], COLOR_PAIR(3));
	wprintw(s_term->slave.wins[WIN_SH], "\t\t\t---= Dynamic symbols =---\n");
	wattron(s_term->slave.wins[WIN_SH], COLOR_PAIR(1));
	wattroff(s_term->slave.wins[WIN_SH], A_BOLD);
	info_sym(s_term->slave.elf->dynsym, s_term->slave.wins[WIN_SH],
			 s_term->slave.elf->dynstr, s_term->slave.elf->strsz);
	return (0);
}

int				info_dyntab(t_term *s_term, char UN **av)
{
	size_t		i;
	Elf64_Dyn	**dyn;
	char		known;

	waddstr(s_term->slave.wins[WIN_SH], ".dynamic:\n");
	if (s_term->slave.elf)
	{
		dyn = s_term->slave.elf->dyn;
		i = 0;
		while (dyn[i])
		{
			known = 1;
			switch (dyn[i]->d_tag)
			{
			 case DT_PLTGOT:
				 wprintw(s_term->slave.wins[WIN_SH], "\t%-15s", ".got.plt");
				 break;
			 case DT_HASH:
				 wprintw(s_term->slave.wins[WIN_SH], "\t%-15s", ".hash");
				 break;
			 case DT_STRTAB:
				 wprintw(s_term->slave.wins[WIN_SH], "\t%-15s", ".dynstr");
				 break;
			 case DT_SYMTAB:
				 wprintw(s_term->slave.wins[WIN_SH], "\t%-15s", ".dynsym");
				 break;
			 case DT_RELA:
				 wprintw(s_term->slave.wins[WIN_SH], "\t%-15s", ".rela");
				 break;
			 case DT_INIT:
				 wprintw(s_term->slave.wins[WIN_SH], "\t%-15s", ".init");
				 break;
			 case DT_FINI:
				 wprintw(s_term->slave.wins[WIN_SH], "\t%-15s", ".fini");
				 break;
			 case DT_REL: 
				 wprintw(s_term->slave.wins[WIN_SH], "\t%-15s", ".rel");
				 break;
			 case DT_JMPREL:
				 wprintw(s_term->slave.wins[WIN_SH], "\t%-15s", ".plt");
				 break;
			 case DT_GNU_HASH:
				 wprintw(s_term->slave.wins[WIN_SH], "\t%-15s", ".gnu.hash");
				 break;
			 case DT_SYMINFO:
				 wprintw(s_term->slave.wins[WIN_SH], "\t%-15s", ".syminfo");
				 break;
			 case DT_DEBUG:
				 wprintw(s_term->slave.wins[WIN_SH], "\t%-15s", ".debug");
				 break;
			 default:
				 known = 0;
				 break;
			}
			if (known)
				wprintw(s_term->slave.wins[WIN_SH], " @ %p\n",
						dyn[i]->d_un.d_ptr);
			else
				wprintw(s_term->slave.wins[WIN_SH], "%x @ %p\n",
						dyn[i]->d_tag, dyn[i]->d_un.d_ptr);
			++i;
		}
	}
	sh_refresh(s_term->slave.wins[WIN_SH], 0, 0);
	return (0);
}

int			info_segment(t_term *s_term, char UN **av)
{
	t_slave	*slave;
	size_t	i;
	t_elf	*elf;

	slave = &s_term->slave;
	i = 0;
	waddstr(slave->wins[WIN_SH], "segments: \n");
	if ((elf = slave->elf))
	{
		while (i < elf->e_hdr->e_phnum)
		{
			switch (elf->p_hdr[i]->p_type)
			{
			 case PT_NULL:
				 wprintw(slave->wins[WIN_SH], "%-15s", "UNUSED");
				 break;
			 case PT_LOAD:
				 wprintw(slave->wins[WIN_SH], "\t%-15s", "LOAD");
				 break;
			 case PT_DYNAMIC:
				 wprintw(slave->wins[WIN_SH], "\t%-15s", "DYNAMIC");
				 break;
			 case PT_INTERP:
				 wprintw(slave->wins[WIN_SH], "\t%-15s", "INTERP");
				 break;
			 case PT_SHLIB:
				 wprintw(slave->wins[WIN_SH], "\t%-15s", "RESERVED");
				 break;
			 case PT_TLS:
				 wprintw(slave->wins[WIN_SH], "\t%-15s", "TLS");
				 break;
			 case PT_PHDR:
				 wprintw(slave->wins[WIN_SH], "\t%-15s", "PHDR");
				 break;
			 case PT_NUM:
				 wprintw(slave->wins[WIN_SH], "\t%-15s", "NUM");
				 break;
			 case PT_GNU_EH_FRAME:
				 wprintw(slave->wins[WIN_SH], "\t%-15s", "GNU_EH_FRAME");
				 break;
			 case PT_GNU_STACK:
				 wprintw(slave->wins[WIN_SH], "\t%-15s", "GNU_EH_STACK");
				 break;
			 case PT_GNU_RELRO:
				 wprintw(slave->wins[WIN_SH], "\t%-15s", "GNU_EH_RELRO");
				 break;
			 case PT_SUNWBSS:
				 wprintw(slave->wins[WIN_SH], "\t%-15s", "WUNWBSS");
				 break;
			 case PT_SUNWSTACK:
				 wprintw(slave->wins[WIN_SH], "\t%-15s", "SUNWSTACK");
				 break;
			 case PT_HISUNW:
				 wprintw(slave->wins[WIN_SH], "\t%-15s", "HSUNW");
				 break;
			 default:
				 wprintw(slave->wins[WIN_SH], "\t%-15s", "UNKNOWN");
				 break;
			}
			wprintw(slave->wins[WIN_SH], "@ %p -> %p :: ",
					elf->p_hdr[i]->p_vaddr,
					elf->p_hdr[i]->p_vaddr + elf->p_hdr[i]->p_memsz);
			if (elf->p_hdr[i]->p_flags & 1)
				wprintw(slave->wins[WIN_SH], "X");
			if (elf->p_hdr[i]->p_flags & 2)
				wprintw(slave->wins[WIN_SH], "W");
			if (elf->p_hdr[i]->p_flags & 4)
				wprintw(slave->wins[WIN_SH], "R");
			wprintw(slave->wins[WIN_SH], "\n");
			++i;
		}
	}
	sh_refresh(s_term->slave.wins[WIN_SH], 0, 0);
	return (0);
}

int					info_sections(t_term *s_term, char UN **av)
{
	Elf64_Shdr		**shdr;
	size_t			i;
	t_slave			*slave;
	int				j;
	char			*name;

	i = 0;
	slave = &s_term->slave;
	wattrset(s_term->slave.wins[WIN_SH], A_BOLD);
	wattron(s_term->slave.wins[WIN_SH], COLOR_PAIR(3));
	wprintw(slave->wins[WIN_SH], "\t\t---= Section Headers =---\n");
	wattroff(s_term->slave.wins[WIN_SH], A_BOLD);
	wattron(s_term->slave.wins[WIN_SH], COLOR_PAIR(1));
	if (!(shdr = s_term->slave.elf->s_hdr))
	{
		wprintw(slave->wins[WIN_SH], "\t\tStripped...\n");
		return (0);	
	}
	while (shdr[i])
	{
		wattrset(s_term->slave.wins[WIN_SH], A_BOLD);
		if ((name = elf_symstr(slave->elf->strtab,
							   shdr[i]->sh_name, slave->elf->sstrsz)))
			wprintw(slave->wins[WIN_SH], "%s:\n", name);
		else
			wprintw(slave->wins[WIN_SH], "Unknown:\n", shdr[i]->sh_name);
		wattroff(s_term->slave.wins[WIN_SH], A_BOLD);
		free(name);
		wprintw(slave->wins[WIN_SH], "  %012lx  ", shdr[i]->sh_addr);
		switch (shdr[i]->sh_type)
		{
		 case (SHT_NULL):
			wprintw(slave->wins[WIN_SH], "%-16s", "NULL");
			break ;
		 case (SHT_PROGBITS):
			wprintw(slave->wins[WIN_SH], "%-16s", "PROGBITS");
			break ;
		 case (SHT_SYMTAB):
			wprintw(slave->wins[WIN_SH], "%-16s", "SYMTAB");
			break ;
		 case (SHT_STRTAB):
			wprintw(slave->wins[WIN_SH], "%-16s", "STRTAB");
			break ;
		 case (SHT_RELA):
			wprintw(slave->wins[WIN_SH], "%-16s", "RELA");
			break ;
		 case (SHT_HASH):
			wprintw(slave->wins[WIN_SH], "%-16s", "HASH");
			break ;
		 case (SHT_NOTE):
			wprintw(slave->wins[WIN_SH], "%-16s", "NOTE");
			break ;
		 case (SHT_DYNAMIC):
			wprintw(slave->wins[WIN_SH], "%-16s", "DYNAMIC");
			break ;
		 case (SHT_NOBITS):
			wprintw(slave->wins[WIN_SH], "%-16s", "NOBITS");
			break ;
		 case (SHT_REL):
			wprintw(slave->wins[WIN_SH], "%-16s", "REL");
			break ;
		 case (SHT_SHLIB):
			wprintw(slave->wins[WIN_SH], "%-16s", "SHLIB");
			break ;
		 case (SHT_DYNSYM):
			wprintw(slave->wins[WIN_SH], "%-16s", "DYNSYM");
			break ;
		 case (SHT_INIT_ARRAY):
			wprintw(slave->wins[WIN_SH], "%-16s", "INIT ARRAY");
			break ;
		 case (SHT_FINI_ARRAY):
			wprintw(slave->wins[WIN_SH], "%-16s", "FINI_ARRAY");
			break ;
		 case (SHT_PREINIT_ARRAY):
			wprintw(slave->wins[WIN_SH], "%-16s", "PREINIT_ARRAY");
			break ;
		 case (SHT_GROUP):
			wprintw(slave->wins[WIN_SH], "%-16s", "GROUP");
			break ;
		 case (SHT_SYMTAB_SHNDX):
			wprintw(slave->wins[WIN_SH], "%-16s", "SYMTAB_SHNDX");
			break ;
		 case (SHT_NUM):
			wprintw(slave->wins[WIN_SH], "%-16s", "NUM");
			break ;
		 case (SHT_GNU_ATTRIBUTES):
			wprintw(slave->wins[WIN_SH], "%-16s", "GNU_ATTRIBUTES");
			break ;
		 case (SHT_GNU_HASH):
			wprintw(slave->wins[WIN_SH], "%-16s", "GNU_HASH");
			break ;
		 case (SHT_GNU_LIBLIST):
			wprintw(slave->wins[WIN_SH], "%-16s", "GNU_LIBLIST");
			break ;
		 case (SHT_CHECKSUM):
			wprintw(slave->wins[WIN_SH], "%-16s", "CHECKSUM");
			break ;
		 case (SHT_GNU_verdef):
			wprintw(slave->wins[WIN_SH], "%-16s", "VERDEF");
			break ;
		 case (SHT_GNU_verneed):
			wprintw(slave->wins[WIN_SH], "%-16s", "VERNEED");
			break ;
		 case (SHT_GNU_versym):
			wprintw(slave->wins[WIN_SH], "%-16s", "VERSYM");
			break ;
		 default:
			wprintw(slave->wins[WIN_SH], "%-16s", "UNKNOWN");
			break ;
		}
		j = 0;
		if ((SHF_WRITE & shdr[i]->sh_flags) && j++ < 8)
			wprintw(slave->wins[WIN_SH], "W");
		if ((SHF_ALLOC & shdr[i]->sh_flags) && j++ < 8)
			wprintw(slave->wins[WIN_SH], "A");
		if ((SHF_EXECINSTR & shdr[i]->sh_flags) && j++ < 8)
			wprintw(slave->wins[WIN_SH], "X");
		if ((SHF_MERGE & shdr[i]->sh_flags) && j++ < 8)
			wprintw(slave->wins[WIN_SH], "M");
		if ((SHF_STRINGS & shdr[i]->sh_flags) && j++ < 8)
			wprintw(slave->wins[WIN_SH], "S");
		if ((SHF_INFO_LINK & shdr[i]->sh_flags) && j++ < 8)
			wprintw(slave->wins[WIN_SH], "I");
		if ((SHF_LINK_ORDER & shdr[i]->sh_flags) && j++ < 8)
			wprintw(slave->wins[WIN_SH], "L");
		if ((SHF_GROUP & shdr[i]->sh_flags) && j++ < 8)
			wprintw(slave->wins[WIN_SH], "G");
		if ((SHF_TLS & shdr[i]->sh_flags) && j++ < 8)
			wprintw(slave->wins[WIN_SH], "T");
		if ((SHF_COMPRESSED & shdr[i]->sh_flags) && j++ < 8)
			wprintw(slave->wins[WIN_SH], "C");
		if ((SHF_ORDERED & shdr[i]->sh_flags) && j++ < 8)
			wprintw(slave->wins[WIN_SH], "O");
		if ((SHF_EXCLUDE & shdr[i]->sh_flags) && j++ < 8)
			wprintw(slave->wins[WIN_SH], "X");
		while (j++ < 8)
			wprintw(slave->wins[WIN_SH], " ");
		wprintw(slave->wins[WIN_SH], "%010lx", shdr[i]->sh_size);
		wprintw(slave->wins[WIN_SH], " %08lx", shdr[i]->sh_link);
		wprintw(slave->wins[WIN_SH], " %010lx", shdr[i]->sh_addralign);
		wprintw(slave->wins[WIN_SH], " %010lx\n", shdr[i]->sh_entsize);
		++i;
	}
	return (0);
}
