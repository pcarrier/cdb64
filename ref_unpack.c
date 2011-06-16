#include "ref.h"

void ref_unpack(char s[ref_size],ref_t *u)
{
  ref_t result;

  result  = (unsigned char) s[7]; result <<= 8;
  result += (unsigned char) s[6]; result <<= 8;
  result += (unsigned char) s[5]; result <<= 8;
  result += (unsigned char) s[4]; result <<= 8;
  result += (unsigned char) s[3]; result <<= 8;
  result += (unsigned char) s[2]; result <<= 8;
  result += (unsigned char) s[1]; result <<= 8;
  result += (unsigned char) s[0];

  *u = result;
}
