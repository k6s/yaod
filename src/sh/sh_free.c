#include <minishell1.h>

void			free_strtab(char **strtab)
{
	char			**stab;

	stab = strtab;
	if (stab)
	{
		while (*stab)
		{
			free(*stab);
			++stab;
		}
		free(strtab);
	}
}

void			free_progs(t_progs *progs)
{
	int			c;

	c = -1;
	if (progs)
	{
		while (c <= '~')
			if (progs->next[++c])
				free_progs(progs->next[(int)(c)]);
		free(progs->prog);
		free(progs);
	}
}

void			free_sbp(t_sbp *sbp)
{
	t_sbp		*t;

	while (sbp)
	{
		t = sbp;
		sbp = sbp->nxt;
		free(t);
	}
}

void			free_hbp(t_hbp *hbp)
{
	t_hbp		*t;

	while (hbp)
	{
		t = hbp;
		hbp = hbp->nxt;
		free(t);
	}
}

void			free_fnt(t_fnt *fnt)
{
	t_fnt		*t;

	while (fnt)
	{
		t = fnt;
		free(fnt->name);
		if (fnt->type == FNT_SHA)
			free(fnt->sym);
		fnt = fnt->prv;
		free(t);
	}
}

void			free_stuff(t_term *s_term)
{
	if (s_term->progs)
		free_progs(s_term->progs);
	free_sbp(s_term->slave.e_sbp);
	free_sbp(s_term->slave.d_sbp);
	free_strtab(s_term->environ);
	free_fnt(s_term->slave.fnt);
	elf_free(s_term->slave.elf);
	curses_close(s_term->slave.wins);
	free(s_term->line);
	close(s_term->slave.fdm);
	free(s_term->slave.wins);
}
