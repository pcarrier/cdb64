#include <unistd.h>
#include "ref.h"
#include "fmt.h"
#include "buffer.h"
#include "strerr.h"

#define FATAL "cdbdump: fatal: "

void die_write(void)
{
  strerr_die2sys(111,FATAL,"unable to write output: ");
}
void put(char *buf,off_t len)
{
  if (buffer_put(buffer_1,buf,len) == -1) die_write();
}
void putflush(void)
{
  if (buffer_flush(buffer_1) == -1) die_write();
}

ref_t pos = 0;

void get(char *buf,off_t len)
{
  int r;
  while (len > 0) {
    r = buffer_get(buffer_0,buf,len);
    if (r == -1)
      strerr_die2sys(111,FATAL,"unable to read input: ");
    if (r == 0)
      strerr_die2x(111,FATAL,"unable to read input: truncated file");
    pos += r;
    buf += r;
    len -= r;
  }
}

char buf[512];

void copy(ref_t len)
{
  unsigned int x;

  while (len) {
    x = sizeof buf;
    if (len < x) x = len;
    get(buf,x);
    put(buf,x);
    len -= x;
  }
}

void getnum(ref_t *num)
{
  get(buf,ref_size);
  ref_unpack(buf,num);
}

char strnum[FMT_ULONG];

main()
{
  ref_t eod;
  ref_t klen;
  ref_t dlen;

  getnum(&eod);
  while (pos < 4096) getnum(&dlen);

  while (pos < eod) {
    getnum(&klen);
    getnum(&dlen);
    put("+",1); put(strnum,fmt_ulong(strnum,klen));
    put(",",1); put(strnum,fmt_ulong(strnum,dlen));
    put(":",1); copy(klen);
    put("->",2); copy(dlen);
    put("\n",1);
  }

  put("\n",1);
  putflush();
  _exit(0);
}
