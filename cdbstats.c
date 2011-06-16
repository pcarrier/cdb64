#include <unistd.h>
#include "ref.h"
#include "fmt.h"
#include "buffer.h"
#include "strerr.h"
#include "seek.h"
#include "cdb.h"

#define FATAL "cdbstats: fatal: "

void die_read(void)
{
  strerr_die2sys(111,FATAL,"unable to read input: ");
}
void die_readformat(void)
{
  strerr_die2x(111,FATAL,"unable to read input: truncated file");
}
void die_write(void)
{
  strerr_die2sys(111,FATAL,"unable to write output: ");
}
void put(char *buf,off_t len)
{
  if (buffer_put(buffer_1small,buf,len) == -1) die_write();
}
void putflush(void)
{
  if (buffer_flush(buffer_1small) == -1) die_write();
}

ref_t pos = 0;

void get(char *buf,off_t len)
{
  int r;
  while (len > 0) {
    r = buffer_get(buffer_0,buf,len);
    if (r == -1) die_read();
    if (r == 0) die_readformat();
    pos += r;
    buf += r;
    len -= r;
  }
}

void getnum(ref_t *num)
{
  char buf[ref_size];
  get(buf,ref_size);
  ref_unpack(buf,num);
}

char strnum[FMT_ULONG];

void putnum(char *label,off_t count)
{
  off_t i;
  put(label,str_len(label));
  for (i = fmt_ulong(0,count);i < 20;++i) put(" ",1);
  put(strnum,fmt_ulong(strnum,count));
  put("\n",1);
}

char key[1024];

static struct cdb c;

static unsigned long numrecords;
static unsigned long numd[11];

main()
{
  ref_t eod;
  ref_t klen;
  ref_t dlen;
  seek_pos rest;
  int r;

  cdb_init(&c,0);

  getnum(&eod);
  while (pos < 256*entry_size) getnum(&dlen);

  while (pos < eod) {
    getnum(&klen);
    getnum(&dlen);
    if (klen > sizeof key) {
      while (klen) { get(key,1); --klen; }
    }
    else {
      get(key,klen);
      rest = seek_cur(0);
      cdb_findstart(&c);
      do {
        switch(cdb_findnext(&c,key,klen)) {
	  case -1: die_read();
	  case 0: die_readformat();
        }
      } while (cdb_datapos(&c) != pos);
      if (!c.loop) die_readformat();
      ++numrecords;
      if (c.loop > 10)
	++numd[10];
      else
	++numd[c.loop - 1];
      if (seek_set(0,rest) == -1) die_read();
    }
    while (dlen) { get(key,1); --dlen; }
  }
  
  putnum("records ",numrecords);
  putnum("d0      ",numd[0]);
  putnum("d1      ",numd[1]);
  putnum("d2      ",numd[2]);
  putnum("d3      ",numd[3]);
  putnum("d4      ",numd[4]);
  putnum("d5      ",numd[5]);
  putnum("d6      ",numd[6]);
  putnum("d7      ",numd[7]);
  putnum("d8      ",numd[8]);
  putnum("d9      ",numd[9]);
  putnum(">9      ",numd[10]);
  putflush();
  _exit(0);
}
