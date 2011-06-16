#include <stdint.h>
#include "buffer.h"

void buffer_init(buffer *s,ssize_t (*op)(),int fd,char *buf,off_t len)
{
  s->x = buf;
  s->fd = fd;
  s->op = op;
  s->p = 0;
  s->n = len;
}
