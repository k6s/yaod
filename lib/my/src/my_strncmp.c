/*
** my_strncmp.c for my_strncmp.c in /home/k6/rendu/Piscine_C_J06
** 
** Made by Cano Chloe
** Login   <k6@epitech.net>
** 
** Started on  Mon Oct  6 13:10:34 2014 Cano Chloe
** Last update Mon Oct  6 13:28:58 2014 Cano Chloe
*/

int		my_strncmp(char *s1, char *s2, int n)
{
  int		ret;

  if (*s1 == *s2 && *s1 && *s2 && n > 1)
    return (my_strncmp(++s1, ++s2, --n));
  ret = *s1 - *s2 > 0 ? 1 : *s1 - *s2;
  ret = ret < 0 ? -1 : ret;
  return (ret);
}
