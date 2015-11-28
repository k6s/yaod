/*
** libmy.h for libmy.h in /home/k6/rendu/Piscine_C_J08
** 
** Made by Cano Chloe
** Login   <k6@epitech.net>
** 
** Started on  Wed Oct  8 15:50:16 2014 Cano Chloe
** Last update Tue Jan 13 19:07:47 2015 
*/

#ifndef LIBMY_H_
# define LIBMY_H_

# include <unistd.h>
# include <stdlib.h>
# include <limits.h>

# define ABS(val)	((val) >= 0 ? (val) : (val) * -1)

# define UN		__attribute__((unused))

typedef struct	s_point
{
  int		x;
  int		y;
} t_point;

void		my_putchar(char c);
int		my_isneg(int nb);
int		my_put_nbr(int nb);
int		my_putnbr_base(int nb, char *base);
int		my_swap(int *a, int *b);
int		my_putstr(char *str);
int		my_putendl(char *str);

int		my_strlen(char *str);
int		my_getnbr(char *str);
int		my_getnbr_base(char *str, char *base, size_t base_len);
void		my_sort_int_tab(int *tab, int size);
int		my_power_rec(int nb, int power);
int		my_square_root(int nb);
int		my_isprime(int nombre);
int		my_find_prime_sup(int nb);
char		*my_strcpy(char *dest, char *src);
char		*my_strncpy(char *dest, char *src, int nb);
char		*my_revstr(char *str);
char		*my_strstr(char *str, char *to_find);
int		my_strcmp(char *s1, char *s2);
int		my_strncmp(char *s1, char *s2, int nb);
char		*my_strupcase(char *str);
char		*my_strlowcase(char *str);
char		*my_strcapitalize(char *str);
int		my_str_isalpha(char *str);
int		my_str_isnum(char *str);
int		my_str_islower(char *str);
int		my_str_isupper(char *str);
int		my_str_isprintable(char *str);
int		my_showstr(char *str);
int		my_showmem(char *str, int size);
char		*my_strcat(char *dest, char *src);
char		*my_strncat(char *dest, char *src, int nb);
int		my_strlcat(char *dest, char *src, int size);
char		*my_strdup(char *src);
char		*convert_base(char *nbr, char *base_from, char *base_to);
int		get_base_len(char *base);
char		**my_str_to_wordtab(char *str);
char		**str_split_whitespc(char *s);
int		my_show_wordtab(char **tab);
char		*my_strdup(char *str);

void		my_bzero(char *mem, int len);
void		my_memcpy(void *dst, const void *src, size_t len);
int		my_memcmp(void *s1, void *s2, int len);
void		*my_memmove(void *dest, void *src, size_t len);
void		*my_memalloc(size_t len);

char		*my_getenv(char **environ, char *name);

#endif /* !LIBMY_H_ */
