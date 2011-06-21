#ifndef CDB_MAKE_H
#define CDB_MAKE_H

#include "buffer.h"
#include "ref.h"

#define CDB_HPLIST 1000

struct cdb_hp { ref_t h; ref_t p; } ;

struct cdb_hplist {
  struct cdb_hp hp[CDB_HPLIST];
  struct cdb_hplist *next;
  int num;
} ;

struct cdb_make {
  char bspace[8192];
  char final[4096];
  ref_t count[256];
  ref_t start[256];
  struct cdb_hplist *head;
  struct cdb_hp *split; /* includes space for hash */
  struct cdb_hp *hash;
  ref_t numentries;
  buffer b;
  ref_t pos;
  int fd;
} ;

extern int cdb_make_start(struct cdb_make *,int);
extern int cdb_make_addbegin(struct cdb_make *,off_t,off_t);
extern int cdb_make_addend(struct cdb_make *,off_t,off_t,ref_t);
extern int cdb_make_add(struct cdb_make *,char *,off_t,char *,off_t);
extern int cdb_make_finish(struct cdb_make *);

#endif
