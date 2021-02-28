#include "utils.h"

void tokenise(char *buffer, char sep, char *dest)
{
  char *tmp = malloc(strlen(buffer));
  int i = 0;
  for (; buffer[i] != '\0' && buffer[i] != sep; i++)
  {
    dest[i] = buffer[i];
  }
  dest[i] = '\0';
  if (buffer[i] == sep)
    strcpy(tmp, buffer + i + 1);
  else
    tmp[0] = '\0';
  strcpy(buffer, tmp);
  free(tmp);
}

void mytoupper(char *str)
{
  for (int i = 0; str[i] != '\0'; i++)
  {
    if (str[i] >= 'a' && str[i] <= 'z')
    {
      str[i] += 'A' - 'a';
    }
  }
}