#ifndef __REF_H
#define __REF_H

#include <stdint.h>

typedef uint64_t ref_t;

#define ref_size (sizeof(ref_t))
#define entry_size (2*ref_size)

#define REF_MAX UINT64_MAX

extern void ref_pack(char *, ref_t);
extern void ref_pack_big(char *, ref_t);
extern void ref_unpack(char *, ref_t *);
extern void ref_unpack_big(char *, ref_t *);

#endif
