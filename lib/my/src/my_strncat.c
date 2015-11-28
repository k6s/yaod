/*
** my_strncat.c for my_strncat.c in /home/k6/rendu/Piscine_C_J07
** 
** Made by Cano Chloe
** Login   <k6@epitech.net>
** 
** Started on  Wed Oct  8 10:18:07 2014 Cano Chloe
** Last update Wed Oct  8 10:30:04 2014 Cano Chloe
*/

char		*my_strncat(char *dest, char *src, int nb)
{
  int		n;
  char		*dp;

  n = 0;
  dp = dest;
  while (*dest)
    ++dest;
  while (n < nb && *src)
      dest[n++] = *src++;
  dest[n] = '\0';
  return (dp);
}
