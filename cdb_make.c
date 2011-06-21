#include <sys/types.h>
#include <unistd.h>
#include "seek.h"
#include "error.h"
#include "alloc.h"
#include "cdb.h"
#include "cdb_make.h"

int cdb_make_start(struct cdb_make *c,int fd)
{
  c->head = 0;
  c->split = 0;
  c->hash = 0;
  c->numentries = 0;
  c->fd = fd;
  c->pos = sizeof c->final;
  buffer_init(&c->b,write,fd,c->bspace,sizeof c->bspace);
  return seek_set(fd,c->pos);
}

static int posplus(struct cdb_make *c,ref_t len)
{
  ref_t newpos = c->pos + len;
  if (newpos < len) { errno = error_nomem; return -1; }
  c->pos = newpos;
  return 0;
}

int cdb_make_addend(struct cdb_make *c,off_t keylen,off_t datalen,ref_t h)
{
  struct cdb_hplist *head;

  head = c->head;
  if (!head || (head->num >= CDB_HPLIST)) {
    head = (struct cdb_hplist *) alloc(sizeof(struct cdb_hplist));
    if (!head) return -1;
    head->num = 0;
    head->next = c->head;
    c->head = head;
  }
  head->hp[head->num].h = h;
  head->hp[head->num].p = c->pos;
  ++head->num;
  ++c->numentries;
  if (posplus(c,entry_size) == -1) return -1;
  if (posplus(c,keylen) == -1) return -1;
  if (posplus(c,datalen) == -1) return -1;
  return 0;
}

int cdb_make_addbegin(struct cdb_make *c,off_t keylen,off_t datalen)
{
  char buf[entry_size];

  ref_pack(buf,keylen);
  ref_pack(buf + ref_size,datalen);
  if (buffer_putalign(&c->b,buf,entry_size) == -1) return -1;
  return 0;
}

int cdb_make_add(struct cdb_make *c,char *key,off_t keylen,char *data,off_t datalen)
{
  if (cdb_make_addbegin(c,keylen,datalen) == -1) return -1;
  if (buffer_putalign(&c->b,key,keylen) == -1) return -1;
  if (buffer_putalign(&c->b,data,datalen) == -1) return -1;
  return cdb_make_addend(c,keylen,datalen,cdb_hash(key,keylen));
}

int cdb_make_finish(struct cdb_make *c)
{
  char buf[entry_size];
  int i;
  ref_t len;
  ref_t u;
  ref_t memsize;
  ref_t count;
  ref_t where;
  struct cdb_hplist *x;
  struct cdb_hp *hp;

  for (i = 0;i < 256;++i)
    c->count[i] = 0;

  for (x = c->head;x;x = x->next) {
    i = x->num;
    while (i--)
      ++c->count[255 & x->hp[i].h];
  }

  memsize = 1;
  for (i = 0;i < 256;++i) {
    u = c->count[i] * 2;
    if (u > memsize)
      memsize = u;
  }

  memsize += c->numentries; /* no overflow possible up to now */
  u = (ref_t) 0 - (ref_t) 1;
  u /= sizeof(struct cdb_hp);
  if (memsize > u) { errno = error_nomem; return -1; }

  c->split = (struct cdb_hp *) alloc(memsize * sizeof(struct cdb_hp));
  if (!c->split) return -1;

  c->hash = c->split + c->numentries;

  u = 0;
  for (i = 0;i < 256;++i) {
    u += c->count[i]; /* bounded by numentries, so no overflow */
    c->start[i] = u;
  }

  for (x = c->head;x;x = x->next) {
    i = x->num;
    while (i--)
      c->split[--c->start[255 & x->hp[i].h]] = x->hp[i];
  }

  for (i = 0;i < 256;++i) {
    count = c->count[i];

    len = count + count; /* no overflow possible */
    ref_pack(c->final + entry_size * i,c->pos);
    ref_pack(c->final + entry_size * i + ref_size,len);

    for (u = 0;u < len;++u)
      c->hash[u].h = c->hash[u].p = 0;

    hp = c->split + c->start[i];
    for (u = 0;u < count;++u) {
      where = (hp->h >> 8) % len;
      while (c->hash[where].p)
	if (++where == len)
	  where = 0;
      c->hash[where] = *hp++;
    }

    for (u = 0;u < len;++u) {
      ref_pack(buf,c->hash[u].h);
      ref_pack(buf + ref_size,c->hash[u].p);
      if (buffer_putalign(&c->b,buf,entry_size) == -1) return -1;
      if (posplus(c,entry_size) == -1) return -1;
    }
  }

  if (buffer_flush(&c->b) == -1) return -1;
  if (seek_begin(c->fd) == -1) return -1;
  return buffer_putflush(&c->b,c->final,sizeof c->final);
}
