#include <elf_parse.h>


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

Elf64_Sym			**elf_file_symtab(int fd, Elf64_Shdr **s_hdr)
{
	size_t			i;
	size_t			j;
	size_t			entnum;
	Elf64_Sym		**sym;

	i = 0;
	sym = NULL;
	while (s_hdr[i] && s_hdr[i]->sh_type != SHT_SYMTAB)
		++i;
	if (s_hdr[i])
	{
		if (lseek(fd, s_hdr[i]->sh_offset, SEEK_SET) == -1)
			return (NULL);
		entnum = s_hdr[i]->sh_size / s_hdr[i]->sh_entsize;
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
	}
	return (sym);
}

char				*elf_file_shstrtab(int fd, Elf64_Shdr **s_hdr, char *strtab,
									   Elf64_Xword sstrsz, Elf64_Xword *size)
{
	char			*shstrtab;
	size_t			i;
	char			*symname;

	i = 0 ;
	while (s_hdr[i])
	{
		symname = NULL;
		if (s_hdr[i]->sh_type == SHT_STRTAB
			&& (symname = elf_symstr(strtab, s_hdr[i]->sh_name, sstrsz))
			&& !strncmp(symname, ".strtab", 10))
		{
			free(symname);
			break;
		}
		else
		{
			free(symname);
			++i;
		}
	}
	if (s_hdr[i])
	{
		*size = s_hdr[i]->sh_size;
		if (!(shstrtab = malloc(sizeof(*shstrtab) * *size)))
			return (NULL);
		if (lseek(fd, s_hdr[i]->sh_offset, SEEK_SET) == -1)
			return (NULL);
		if (read(fd, shstrtab, *size) != (ssize_t)*size)
		{
			free(shstrtab);
			return (NULL);
		}
	}
	return (shstrtab);
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

	if ((fd = open(filename, O_RDONLY)) > 0)
	{
		if (!(elf->s_hdr = elf_file_shdr(fd, elf->e_hdr)))
			return (-1);
		if (!(elf->strtab = elf_file_strtab(fd, elf->e_hdr, elf->s_hdr)))
			return (-1);
		elf->sstrsz = elf->s_hdr[elf->e_hdr->e_shstrndx]->sh_size;
		if (!(elf->symtab = elf_file_symtab(fd, elf->s_hdr)))
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
