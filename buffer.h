#ifndef BUFFER_H
#define BUFFER_H

#include <sys/types.h>

typedef struct buffer {
  char *x;
  off_t p;
  off_t n;
  int fd;
  ssize_t (*op)();
} buffer;

#define BUFFER_INIT(op,fd,buf,len) { (buf), 0, (len), (fd), (op) }
#define BUFFER_INSIZE 8192
#define BUFFER_OUTSIZE 8192

extern void buffer_init(buffer *,ssize_t (*)(),int,char *,off_t);

extern int buffer_flush(buffer *);
extern int buffer_put(buffer *,char *,off_t);
extern int buffer_putalign(buffer *,char *,off_t);
extern int buffer_putflush(buffer *,char *,off_t);
extern int buffer_puts(buffer *,char *);
extern int buffer_putsalign(buffer *,char *);
extern int buffer_putsflush(buffer *,char *);

#define buffer_PUTC(s,c) \
  ( ((s)->n != (s)->p) \
    ? ( (s)->x[(s)->p++] = (c), 0 ) \
    : buffer_put((s),&(c),1) \
  )

extern int buffer_get(buffer *,char *,off_t);
extern int buffer_bget(buffer *,char *,off_t);
extern int buffer_feed(buffer *);

extern char *buffer_peek(buffer *);
extern void buffer_seek(buffer *,off_t);

#define buffer_PEEK(s) ( (s)->x + (s)->n )
#define buffer_SEEK(s,len) ( ( (s)->p -= (len) ) , ( (s)->n += (len) ) )

#define buffer_GETC(s,c) \
  ( ((s)->p > 0) \
    ? ( *(c) = (s)->x[(s)->n], buffer_SEEK((s),1), 1 ) \
    : buffer_get((s),(c),1) \
  )

extern int buffer_copy(buffer *,buffer *);

extern buffer *buffer_0;
extern buffer *buffer_0small;
extern buffer *buffer_1;
extern buffer *buffer_1small;
extern buffer *buffer_2;

#endif
