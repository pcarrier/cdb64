#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "error.h"
#include "seek.h"
#include "byte.h"
#include "cdb.h"

void cdb_free(struct cdb *c)
{
  if (c->map) {
    munmap(c->map,c->size);
    c->map = 0;
  }
}

void cdb_findstart(struct cdb *c)
{
  c->loop = 0;
}

void cdb_init(struct cdb *c,int fd)
{
  struct stat st;
  char *x;

  cdb_free(c);
  cdb_findstart(c);
  c->fd = fd;

  if (fstat(fd,&st) == 0) {
    x = mmap(0,st.st_size,PROT_READ,MAP_SHARED,fd,0);
    if (x + 1) {
      c->size = st.st_size;
      c->map = x;
    }
  }
}

int cdb_read(struct cdb *c,char *buf,off_t len,ref_t pos)
{
  if (c->map) {
    if ((pos > c->size) || (c->size - pos < len)) goto FORMAT;
    byte_copy(buf,len,c->map + pos);
  }
  else {
    if (seek_set(c->fd,pos) == -1) return -1;
    while (len > 0) {
      int r;
      do
        r = read(c->fd,buf,len);
      while ((r == -1) && (errno == error_intr));
      if (r == -1) return -1;
      if (r == 0) goto FORMAT;
      buf += r;
      len -= r;
    }
  }
  return 0;

  FORMAT:
  errno = error_proto;
  return -1;
}

static int match(struct cdb *c,char *key,off_t len,ref_t pos)
{
  char buf[64];
  int n;

  while (len > 0) {
    n = sizeof buf;
    if (n > len) n = len;
    if (cdb_read(c,buf,n,pos) == -1) return -1;
    if (byte_diff(buf,n,key)) return 0;
    pos += n;
    key += n;
    len -= n;
  }
  return 1;
}

int cdb_findnext(struct cdb *c,char *key,off_t len)
{
  char buf[entry_size];
  ref_t pos;
  ref_t u;

  if (!c->loop) {
    u = cdb_hash(key,len);
    if (cdb_read(c,buf,entry_size,(u * entry_size) & (256*entry_size - 1)) == -1) return -1;
    ref_unpack(buf + ref_size,&c->hslots);
    if (!c->hslots) return 0;
    ref_unpack(buf,&c->hpos);
    c->khash = u;
    u /= ref_size;
    u %= c->hslots;
    u *= entry_size;
    c->kpos = c->hpos + u;
  }

  while (c->loop < c->hslots) {
    if (cdb_read(c,buf,entry_size,c->kpos) == -1) return -1;
    ref_unpack(buf + ref_size,&pos);
    if (!pos) return 0;
    c->loop += 1;
    c->kpos += 8;
    if (c->kpos == c->hpos + (c->hslots * entry_size)) c->kpos = c->hpos;
    ref_unpack(buf,&u);
    if (u == c->khash) {
      if (cdb_read(c,buf,entry_size,pos) == -1) return -1;
      ref_unpack(buf,&u);
      if (u == len)
	switch(match(c,key,len,pos + entry_size)) {
	  case -1:
	    return -1;
	  case 1:
	    ref_unpack(buf + ref_size,&c->dlen);
	    c->dpos = pos + entry_size + len;
	    return 1;
	}
    }
  }

  return 0;
}

int cdb_find(struct cdb *c,char *key,off_t len)
{
  cdb_findstart(c);
  return cdb_findnext(c,key,len);
}
