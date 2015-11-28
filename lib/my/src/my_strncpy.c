/*
** my_strncpy.c for my_strncy.c in /home/k6/rendu/Piscine_C_J06
** 
** Made by Cano Chloe
** Login   <k6@epitech.net>
** 
** Started on  Mon Oct  6 10:20:09 2014 Cano Chloe
** Last update Tue Oct  7 10:36:30 2014 Cano Chloe
*/

char		*my_strncpy(char *dest, char *src, int n)
{
  char		*dp;

  dp = dest;
  while (n > 0 && *src)
    {
      *dp++ = *src++;
      --n;
    }
  while (n-- > 0)
      *dp++ = '\0';
  return (dest);
}
