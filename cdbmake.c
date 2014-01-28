#include <unistd.h>
#include "error.h"
#include "open.h"
#include "strerr.h"
#include "cdb_make.h"
#include "cdb.h"

#define FATAL "cdbmake: fatal: "

char *fn;
char *fntmp;

void die_usage(void)
{
  strerr_die1x(100,"cdbmake: usage: cdbmake f ftmp");
}
void die_write(void)
{
  strerr_die4sys(111,FATAL,"unable to create ",fntmp,": ");
}
void die_read(void)
{
  strerr_die2sys(111,FATAL,"unable to read input: ");
}
void die_readformat(void)
{
  strerr_die2x(111,FATAL,"unable to read input: bad format");
}

inline void get(char *ch)
{
  switch(buffer_GETC(buffer_0,ch)) {
    case 0: die_readformat();
    case -1: die_read();
  }
}

static struct cdb_make c;

main(int argc,char **argv)
{
  off_t klen;
  off_t dlen;
  off_t i;
  ref_t h;
  int fd;
  char ch;

  if (!*argv) die_usage();
  
  if (!*++argv) die_usage();
  fn = *argv;

  if (!*++argv) die_usage();
  fntmp = *argv;

  fd = open_trunc(fntmp);
  if (fd == -1) die_write();

  if (cdb_make_start(&c,fd) == -1) die_write();

  for (;;) {
    get(&ch);
    if (ch == '\n') break;
    if (ch != '+') die_readformat();
    klen = 0;
    for (;;) {
      get(&ch);
      if (ch == ',') break;
      if ((ch < '0') || (ch > '9')) die_readformat();
      klen = klen * 10 + (ch - '0');
    }
    dlen = 0;
    for (;;) {
      get(&ch);
      if (ch == ':') break;
      if ((ch < '0') || (ch > '9')) die_readformat();
      dlen = dlen * 10 + (ch - '0');
    }

    if (cdb_make_addbegin(&c,klen,dlen) == -1) die_write();
    h = CDB_HASHSTART;
    for (i = 0;i < klen;++i) {
      get(&ch);
      if (buffer_PUTC(&c.b,ch) == -1) die_write();
      h = cdb_hashadd(h,ch);
    }
    get(&ch);
    if (ch != '-') die_readformat();
    get(&ch);
    if (ch != '>') die_readformat();
    for (i = 0;i < dlen;++i) {
      get(&ch);
      if (buffer_PUTC(&c.b,ch) == -1) die_write();
    }
    if (cdb_make_addend(&c,klen,dlen,h) == -1) die_write();

    get(&ch);
    if (ch != '\n') die_readformat();
  }

  if (cdb_make_finish(&c) == -1) die_write();
  if (fsync(fd) == -1) die_write();
  if (close(fd) == -1) die_write(); /* NFS silliness */
  if (rename(fntmp,fn) == -1)
    strerr_die6sys(111,FATAL,"unable to rename ",fntmp," to ",fn,": ");

  _exit(0);
}
