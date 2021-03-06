#include <elf_parse.h>

Elf64_Shdr				*elf_shdr_type(Elf64_Shdr **shdr, Elf64_Word type)
{
	size_t				i;

	i = 0;
	while (shdr[i] && shdr[i]->sh_type != type)
		++i;
	return (shdr[i]);
}

Elf64_Shdr				*elf_shdr_name(Elf64_Shdr **shdr, Elf64_Word type,
									   char *name, char *strtab, long sstrsz)
{
	size_t				i;
	size_t				namlen;
	char				*sym_name;

	i = 0;
	namlen = strlen(name);
	while (shdr[i])
	{
		if (shdr[i]->sh_type == type)
		{
			if ((sym_name = elf_symstr(strtab, shdr[i]->sh_name, sstrsz)))
			{
				if (!strncmp(sym_name, name, namlen))
				{
					free(sym_name);
					return (shdr[i]);
				}
				free(sym_name);
			}
		}
		++i;
	}
	return (NULL);
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
	memset(s_hdr, 0, sizeof(*s_hdr) * (e_hdr->e_shnum + 1));
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

Elf64_Sym			**elf_file_symtab(int fd, Elf64_Shdr *s_hdr)
{
	size_t			j;
	size_t			entnum;
	Elf64_Sym		**sym;

	sym = NULL;
	if (lseek(fd, s_hdr->sh_offset, SEEK_SET) == -1)
		return (NULL);
	entnum = s_hdr->sh_size / s_hdr->sh_entsize;
	if (!(sym = malloc(sizeof(*sym) * (entnum + 1))))
		return (NULL);
	j = 0;
	memset(sym, 0, sizeof(*sym) * (entnum + 1));
	while (j < entnum)
	{
		if (!(sym[j] = malloc(sizeof(**sym))))
				return (NULL);
		if (read(fd, sym[j], sizeof(**sym)) != sizeof(**sym))
			return (NULL);
		++j;
	}
	return (sym);
}

char			*elf_file_shstrtab(int fd, Elf64_Shdr **s_hdr, char *shstrtab,
								   Elf64_Xword shstrsz, Elf64_Xword *size)
{
	char		*strtab;
	size_t		i;
	Elf64_Shdr	*strtab_shdr;
		
	if ((strtab_shdr = elf_shdr_name(s_hdr, SHT_STRTAB, ".strtab", shstrtab,
									shstrsz)))
	{
		*size = strtab_shdr->sh_size;
		if (!(strtab = malloc(sizeof(*strtab) * *size)))
			return (NULL);
		if (lseek(fd, strtab_shdr->sh_offset, SEEK_SET) == -1)
			return (NULL);
		if (read(fd, strtab, *size) != (ssize_t)*size)
		{
			free(strtab);
			return (NULL);
		}
	}
	return (strtab);
}


char				*elf_file_strtab(int fd, Elf64_Shdr *s_hdr)
{
	ssize_t			size;
	char			*strtab;

	size = s_hdr->sh_size;
	if (!(strtab = malloc(sizeof(*strtab) * size)))
		return (NULL);
	if (lseek(fd, s_hdr->sh_offset, SEEK_SET) == -1)
		return (NULL);
	if (read(fd, strtab, size) != size)
	{
		free(strtab);
		return (NULL);
	}
	return (strtab);
}

ssize_t					elf_from_file(char *filename, t_elf *elf)
{
	int					fd;
	size_t				i;

	if ((fd = open(filename, O_RDONLY)) > 0)
	{
		if (!(elf->s_hdr = elf_file_shdr(fd, elf->e_hdr)))
			return (-1);
		if (!(elf->strtab
			  = elf_file_strtab(fd, elf->s_hdr[elf->e_hdr->e_shstrndx])))
			return (-1);
		elf->sstrsz = elf->s_hdr[elf->e_hdr->e_shstrndx]->sh_size;
		i = 0;
		while (elf->s_hdr[i] && elf->s_hdr[i]->sh_type != SHT_SYMTAB)
			++i;
		if (!elf->s_hdr[i]
			|| !(elf->symtab = elf_file_symtab(fd, elf->s_hdr[i])))
			return (-1);
		if (!(elf->shstrtab
			  = elf_file_shstrtab(fd, elf->s_hdr, elf->strtab, elf->sstrsz,
								  &elf->shstrsz)))
			return (-1);
		close(fd);
		return (0);
	}
	return (-1);
}
