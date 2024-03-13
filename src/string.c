#include "string.h"

void ah_strcpyString(String *s1, String *s2) {
  s1->len = s2->len;
  s1->buffer = (u8 *)malloc(sizeof(u8 ) * s1->len);
  memcpy(s1->buffer, s2->buffer, s2->len);
}

char *ah_strcpy(char *buffer, struct String *s2) {
  memcpy(buffer, s2->buffer, s2->len);
  return buffer;
}
