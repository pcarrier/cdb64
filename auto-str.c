#include <unistd.h>
#include "buffer.h"

char buf1[256];
buffer ss1 = BUFFER_INIT(write,1,buf1,sizeof(buf1));

void cdb_puts(s)
char *s;
{
  if (buffer_puts(&ss1,s) == -1) _exit(111);
}

main(argc,argv)
int argc;
char **argv;
{
  char *name;
  char *value;
  unsigned char ch;
  char octal[4];

  name = argv[1];
  if (!name) _exit(100);
  value = argv[2];
  if (!value) _exit(100);

  cdb_puts("char ");
  cdb_puts(name);
  cdb_puts("[] = \"\\\n");

  while (ch = *value++) {
    cdb_puts("\\");
    octal[3] = 0;
    octal[2] = '0' + (ch & 7); ch >>= 3;
    octal[1] = '0' + (ch & 7); ch >>= 3;
    octal[0] = '0' + (ch & 7);
    cdb_puts(octal);
  }

  cdb_puts("\\\n\";\n");
  if (buffer_flush(&ss1) == -1) _exit(111);
  _exit(0);
}
