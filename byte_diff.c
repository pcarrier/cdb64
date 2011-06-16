#include <sys/types.h>
#include "byte.h"

off_t byte_diff(s,n,t)
register char *s;
register off_t n;
register char *t;
{
  for (;;) {
    if (!n) return 0; if (*s != *t) break; ++s; ++t; --n;
    if (!n) return 0; if (*s != *t) break; ++s; ++t; --n;
    if (!n) return 0; if (*s != *t) break; ++s; ++t; --n;
    if (!n) return 0; if (*s != *t) break; ++s; ++t; --n;
  }
  return ((off_t) *s) - ((off_t) *t);
}
