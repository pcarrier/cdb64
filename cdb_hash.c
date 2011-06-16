/* Public domain. */

#include "cdb.h"

ref_t cdb_hashadd(ref_t h,unsigned char c)
{
  h += (h << 5);
  return h ^ c;
}

ref_t cdb_hash(char *buf,off_t len)
{
  ref_t h;

  h = CDB_HASHSTART;
  while (len) {
    h = cdb_hashadd(h,*buf++);
    --len;
  }
  return h;
}
