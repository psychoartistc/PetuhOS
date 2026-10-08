#include <stddef.h>
#include <stdint.h>
#include <utils/memory.h>

#ifdef memcpy
#undef memcpy
#endif
void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
  uint8_t *pdest = dest;
  const uint8_t *psrc = src;

  for (size_t i = 0; i < n; i++) {
    pdest[i] = psrc[i];
  }

  return dest;
}

#ifdef memset
#undef memset
#endif
void *memset(void *s, int c, size_t n) {
  uint8_t *p = s;

  for (size_t i = 0; i < n; i++) {
    p[i] = (uint8_t)c;
  }

  return s;
}

#ifdef memmove
#undef memmove
#endif
void *memmove(void *dest, const void *src, size_t n) {
  uint8_t *pdest = dest;
  const uint8_t *psrc = src;

  if ((uintptr_t)src > (uintptr_t)dest) {
    for (size_t i = 0; i < n; i++) {
      pdest[i] = psrc[i];
    }
  } else if ((uintptr_t)src < (uintptr_t)dest) {
    for (size_t i = n; i > 0; i--) {
      pdest[i - 1] = psrc[i - 1];
    }
  }

  return dest;
}

#ifdef memcmp
#undef memcmp
#endif
int memcmp(const void *s1, const void *s2, size_t n) {
  const uint8_t *p1 = s1;
  const uint8_t *p2 = s2;

  for (size_t i = 0; i < n; i++) {
    if (p1[i] != p2[i]) {
      return p1[i] < p2[i] ? -1 : 1;
    }
  }

  return 0;
}

char *strstr(const char *haystack, const char *needle) {

  if (*needle == '\0') {
    return (char *)haystack;
  }

  while (*haystack != '\0') {

    if (*haystack == *needle) {
      const char *h = haystack;
      const char *n = needle;

      while (*h != '\0' && *n != '\0' && *h == *n) {
        h++;
        n++;
      }

      if (*n == '\0') {
        return (char *)haystack;
      }
    }
    haystack++;
  }

  return NULL;
}
int strcmp(const char *s1, const char *s2) {

  const unsigned char *p1 = (const unsigned char *)s1;
  const unsigned char *p2 = (const unsigned char *)s2;

  while (*p1 && (*p1 == *p2)) {
    p1++;
    p2++;
  }

  return *p1 - *p2;
}

size_t strlen(const char *s) {
  size_t length = 0;
  while (s[length] != '\0') {
    length++;
  }
  return length;
}
