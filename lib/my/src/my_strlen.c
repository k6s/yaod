/*
** my_strlen.c for my_strlen.c in /home/k6/rendu/Piscine_C_J04
** 
** Made by Cano Chloe
** Login   <k6@epitech.net>
** 
** Started on  Fri Oct  3 09:23:46 2014 Cano Chloe
** Last update Mon Oct 20 22:41:40 2014 Cano Chloe
*/

int			my_strlen(char *str)
{
  char			*s;

  s = str;
  if (s)
    {
      while (*s)
	++s;
    }
  return (s - str);
}
