/*
** my_memcmp.c for lib in /home/cano_c/rendu/ELEM/Bistromathique
** 
** Made by Chloe Cano
** Login   <cano_c@epitech.net>
** 
** Started on  Sun Nov  9 04:59:21 2014
** Last update Sun Nov  9 05:09:03 2014 
*/

int		my_memcmp(void *s1, void *s2, int len)
{
  int		idx;

  idx = 0;
  while (idx < len && *(char *)(s1 + idx) == *(char *)(s2 + idx))
    ++idx;
  if (idx == len)
    --idx;
  return (*(char *)(s1 + idx) - *(char *)(s2 + idx));
}
