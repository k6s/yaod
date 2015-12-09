#include <elf_parse.h>
#include <stddef.h>

int						read_data(int fd, void *buff, off_t off, ssize_t len)
{
	if (lseek(fd, off, SEEK_SET) < 0)
		return (-1);
	if (read(fd, (char *)buff, len) != len)
		return (-1);
	return (0);
}

char					*read_str(int fd, off_t addr)
{
	char				*s;
	size_t				i;
	ssize_t				r;

	s = NULL;
	i = 0;
	r = 0;
	if (lseek(fd, addr, SEEK_SET) < 0)
		return (NULL);
	while (!s || !memchr(s + i - r, 0, r))
	{
		if (!(s = realloc(s, i + sizeof(long))))
			return (NULL);
		if ((r = read(fd, s + i, sizeof(long))) <= 0)
			return (NULL);
		i += r;
	}
	return (s);

}

Elf64_Ehdr						*elf_sha_ehdr(int fd)
{
	Elf64_Ehdr					*e_hdr;

	if (!(e_hdr = malloc(sizeof(*e_hdr))))
		return (NULL);
	lseek(fd, 0, SEEK_SET);
	if (read(fd, e_hdr, sizeof(*e_hdr)) != sizeof(*e_hdr))
	{
		free(e_hdr);
		return (NULL);
	}
	return (e_hdr);
}

off_t					elf_sha_shstrtab(int fd, Elf64_Ehdr *e_hdr)
{
	Elf64_Shdr			sh_strtab;

	if (lseek(fd, e_hdr->e_shstrndx * sizeof(sh_strtab)
			  + e_hdr->e_shoff, SEEK_SET) < 0)
		return (-1);
	if (read(fd, &sh_strtab, sizeof(sh_strtab)) != sizeof(sh_strtab))
		return (-1);
	return (sh_strtab.sh_offset);
}

void					free_tables_addr(t_tables_addr *tables)
{
	free(tables->nchains);
	free(tables);
}

static int			sha_tables_new_sym(int fd, Elf64_Ehdr *e_hdr, off_t shstrtab,
									   size_t i, t_tables_addr *tables)
{
	Elf64_Shdr		cur;
	char			*name;

	if (lseek(fd, e_hdr->e_shoff + e_hdr->e_shentsize * i, SEEK_SET) < 0)
		return (-1);
	if (read(fd, &cur, e_hdr->e_shentsize) != e_hdr->e_shentsize)
		return (-1);
	switch (cur.sh_type)
	{
	 case (SHT_SYMTAB):
		 tables->symtab = cur.sh_offset;
		 *tables->nchains = cur.sh_size / cur.sh_entsize;
		 break ;
	 case (SHT_STRTAB):
		 if ((name = read_str(fd, cur.sh_name + shstrtab))
			 && !strncmp(name, ".strtab", 8))
		 tables->strtab = cur.sh_offset;
		 free(name);
		 break ;
	}
	return (0);
}

t_tables_addr			*elf_file_sha_tables(int fd, Elf64_Ehdr *e_hdr)
{
	t_tables_addr		*tables;
	off_t				shstrtab;
	size_t				i;

	if (!(tables = malloc(sizeof(*tables))))
		return (NULL);
	memset(tables, 0, sizeof(*tables));
	if (!(tables->nchains = malloc(sizeof(*tables->nchains))))
		return (NULL);
	if ((shstrtab = elf_sha_shstrtab(fd, e_hdr)) == -1)
		return (NULL);
	if (lseek(fd, e_hdr->e_shoff, SEEK_SET) < 0)
		return (NULL);
	i = 0;
	while (i < e_hdr->e_shnum && (!tables->symtab || !tables->strtab))
	{
		if (sha_tables_new_sym(fd, e_hdr, shstrtab, i, tables))
		{
			free_tables_addr(tables);
			return (NULL);
		}
		++i;
	}
	if (!tables->symtab)
	{
		free_tables_addr(tables);
		tables = NULL;
	}
	return (tables);
}

Elf64_Sym		*elf_sha_sym(int fd, t_tables_addr *tabs, unsigned long base,
							 unsigned long addr)
{
	size_t		i;
	Elf64_Sym	*sym;

	if (lseek(fd, tabs->symtab, SEEK_SET) < 0)
		return (NULL);
	if (!(sym = malloc(sizeof(*sym))))
		return (NULL);
	i = 0;
	while (i < *tabs->nchains)
	{
		if (read(fd, sym, sizeof(*sym)) != sizeof(*sym))
			return (NULL);
		if (sym->st_value + base == addr)
		{
			sym->st_value += base;
			return (sym);
		}
		++i;
	}
	free(sym);
	return (NULL);
}
