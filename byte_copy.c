#include <sys/types.h>
#include "byte.h"

void byte_copy(to,n,from)
register char *to;
register off_t n;
register char *from;
{
  for (;;) {
    if (!n) return; *to++ = *from++; --n;
    if (!n) return; *to++ = *from++; --n;
    if (!n) return; *to++ = *from++; --n;
    if (!n) return; *to++ = *from++; --n;
  }
}
