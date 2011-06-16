#include "ref.h"

void ref_pack(char s[ref_size],ref_t u)
{
  s[0] = u & 255; u >>= 8;
  s[1] = u & 255; u >>= 8;
  s[2] = u & 255; u >>= 8;
  s[3] = u & 255; u >>= 8;
  s[4] = u & 255; u >>= 8;
  s[5] = u & 255; u >>= 8;
  s[6] = u & 255;
  s[7] = u >> 8;
}
