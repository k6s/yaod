/*
** my_strcmp.c for my_strcmp.c in /home/k6/rendu/Piscine_C_J06
** 
** Made by Cano Chloe
** Login   <k6@epitech.net>
** 
** Started on  Mon Oct  6 13:10:34 2014 Cano Chloe
** Last update Tue Oct  7 09:43:04 2014 Cano Chloe
*/

int		my_strcmp(char *s1, char *s2)
{
  int		ret;

  if (*s1 == *s2 && *s1 && *s2)
    return (my_strcmp(++s1, ++s2));
  ret = *s1 - *s2 > 0 ? 1 : *s1 - *s2;
  ret = ret < 0 ? -1 : ret;
  return (ret);
}
