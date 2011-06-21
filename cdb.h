#ifndef CDB_H
#define CDB_H

#include <sys/types.h>
#include "ref.h"

#define CDB_HASHSTART 5381
extern ref_t cdb_hashadd(ref_t,unsigned char);
extern ref_t cdb_hash(char *,off_t);

struct cdb {
  char *map; /* 0 if no map is available */
  int fd;
  ref_t size; /* initialized if map is nonzero */
  ref_t loop; /* number of hash slots searched under this key */
  ref_t khash; /* initialized if loop is nonzero */
  ref_t kpos; /* initialized if loop is nonzero */
  ref_t hpos; /* initialized if loop is nonzero */
  ref_t hslots; /* initialized if loop is nonzero */
  ref_t dpos; /* initialized if cdb_findnext() returns 1 */
  ref_t dlen; /* initialized if cdb_findnext() returns 1 */
} ;

extern void cdb_free(struct cdb *);
extern void cdb_init(struct cdb *,int fd);

extern int cdb_read(struct cdb *,char *,off_t,ref_t);

extern void cdb_findstart(struct cdb *);
extern int cdb_findnext(struct cdb *,char *,off_t);
extern int cdb_find(struct cdb *,char *,off_t);

#define cdb_datapos(c) ((c)->dpos)
#define cdb_datalen(c) ((c)->dlen)

#endif
