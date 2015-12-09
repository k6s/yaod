#include <elf_parse.h>
#include <stddef.h>

static char		*get_pathname(char *name)
{
	char		*path[20] = {"/lib/", "/lib64/", "/usr/lib/",
		"/usr/lib64/", NULL };
	char		*pathname;
	size_t		i;
	size_t		namlen;

	i = 0;
	namlen = strlen(name);
	while (path[i])
	{
		if (!(pathname = malloc(sizeof(*pathname)
								* (namlen + strlen(path[i] + 1)))))
			return (NULL);
		memcpy(pathname, path[i], strlen(path[i]));
		memcpy(pathname, name, namlen);
		if (!access(pathname, F_OK))
			return (pathname);
		free(pathname);
		++i;
	}
	return (NULL);
}

int						read_data(int fd, void *buff, off_t off, ssize_t len)
{
	if (lseek(fd, off, SEEK_SET) < 0)
		return (-1);
	if (read(fd, (char *)buff, len) != len)
		return (-1);
	return (0);
}

Elf64_Ehdr						*elf_sha_ehdr(int fd)
{
	Elf64_Ehdr					*e_hdr;

	if (!(e_hdr = malloc(sizeof(*e_hdr))))
		return (NULL);
	if (read_data(fd, &e_hdr->e_shoff, offsetof(Elf64_Ehdr, e_shoff),
				 sizeof(e_hdr->e_shoff)))
	{
		free(e_hdr);
		return (NULL);
	}
	if (read_data(fd, &e_hdr->e_shnum, offsetof(Elf64_Ehdr, e_shnum),
				 sizeof(e_hdr->e_shnum)))
	{
		free(e_hdr);
		return (NULL);
	}
	if (read_data(fd, &e_hdr->e_shstrndx, offsetof(Elf64_Ehdr, e_shstrndx),
				 sizeof(e_hdr->e_shstrndx)))
	{
		free(e_hdr);
		return (NULL);
	}
	return (e_hdr);
}

char							*read_str(int fd, off_t addr)
{
	char						*s;
	size_t						i;

	s = NULL;
	i = 0;
	if (lseek(fd, addr, SEEK_SET) < 0)
		return (NULL);
	while (!s || !memchr(s + i * sizeof(long), 0, sizeof(long)))
	{
		if (!(s = realloc(s, i + sizeof(long))))
			return (NULL);
		if (read(fd, s + i * sizeof(long), sizeof(long)) != sizeof(long))
			return (NULL);
		i += sizeof(long);
	}
	return (s);

}

off_t			elf_sha_shstrtab(int fd)
{
	Elf64_Shdr	sh_strtab;

	if (lseek(fd, e_hdr->e_shstrndx * sizeof(sh_strtab)
			  + e_hdr->e_shoff, SEEK_SET) < 0)
		return (-1);
	if (read(fd, &sh_strtab, sizeof(sh_strtab)) != sizeof(sh_strtab))
		return (-1);
	return (sh_strtab.sh_offset);
}

Elf64_Shdr		**elf_sha_tables(int fd, Elf64_Ehdr *e_hdr)
{
	Elf64_Shdr	**shdr;
	Elf64_Shdr	*cur;
	off_t		shstrtab;
	size_t		i;
	char		*name;

	if ((shstrtab = elf_sha_shstrtab(fd, e_hdr)) == -1)
		return (NULL);
	if (lseek(fd, e_hdr->e_shoff, SEEK_SET) < 0)
		return (NULL);
	if (!(shdr = malloc(sizeof(*shdr) * 3)))
		return (NULL);
	memset(shdr, 0, sizeof(*shdr) * 3);
	i = 0;
	cur = NULL;
	while (i < e_hdr->e_shnum && (shdr[0] || shdr[1] || shdr[2]))
	{
		if (!cur && !(cur = malloc(sizeof(*cur))))
			return (NULL);
		if (read(fd, cur, sizeof(*cur)) != sizeof(*cur))
			return (NULL);
		switch (cur->sh_type)
		{
		 case (SHT_SYMTAB):
			 shdr[2] = cur;
			 cur = NULL;
			 break ;
		 case (SHT_STRTAB):
			 if ((name = read_str(fd, cur->sh_name + shstrtab))
				 && !strncpy(name, ".strtab", 8))
			 {
				 shdr[1] = cur;
				 cur = NULL;
			 }
			 free(name);
			 break ;
		}
		++i;
	}
	if (!shdr[0] || !shdr[1])
	{
		free(shdr[0]);
		free(shdr[1]);
		free(shdr);
		shdr = NULL;
	}
	return (shdr);
}

Elf64_Sym		*elf_sha_sym(int fd, Elf64_Shdr *shdr, long base, long addr)
{
	size_t		i;
	Elf64_Sym	*sym;

	if (lseek(fd, shdr->sh_offset, SEEK_SET) < 0)
		return (NULL);
	if (!(sym = malloc(sizeof(*sym))))
		return (NULL);
	while (i < shdr->sh_size / shdr->sh_entsize)
	{
		if (read(fd, sym, sizeof(*sym)) != sizeof(*sym))
			return (NULL);
		if (sym->st_value + base == addr)
			return (sym);
		++i;
	}
	free(sym);
	return (NULL);
}

char			*elf_sha_static(struct link_map *lm, long addr)
{
	char		*pathname;
	int			fd;
	Elf64_Ehdr	*e_hdr;
	Elf64_Shdr	**s_hdr;
	Elf64_Sym	*sym;
	char		*name = NULL;

	if ((pathname = get_pathname(lm->l_name)))
	{
		if ((fd = open(pathname, O_RDONLY)) < 0)
			return (NULL);
		if (!(e_hdr = elf_sha_ehdr(fd)))
		{
			close(fd);
			return (NULL);
		}
		if (!(s_hdr = elf_sha_tables(fd, e_hdr)))
		{
			close(fd);
			free(e_hdr);
			return (NULL);
		}
		free(pathname);
		if ((sym = elf_sha_sym(fd, s_hdr[0], lm->l_addr, addr)))
			name = read_str(fd, s_hdr[1]->sh_offset + sym->st_name);
	}
	return (name);
}
