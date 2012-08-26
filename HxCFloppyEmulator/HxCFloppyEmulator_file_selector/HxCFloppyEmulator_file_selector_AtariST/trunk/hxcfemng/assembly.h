#ifndef __QUICKERSORT_H__
#define __QUICKERSORT_H__

extern void quickersort();
#define quickersort(nbEntries, offset, base)              \
__extension__                           \
({                                      \
__asm__ volatile                        \
(                                       \
"move.l   %3,-(sp)\n\t"                    \
"move.w   %2,-(sp)\n\t"                    \
"move.w   %1,-(sp)\n\t"                    \
"jsr      (%0)\n\t"                         \
"addq.l    #8,sp"                          \
: /* outputs "=r"(retvalue) */           \
: /* inputs   */ "a"(quickersort), "g"((short) nbEntries), "g"((short)offset), "g"((unsigned char *)base) \
: /* clobbers */ "d0", "d1", "d2", "a0", "a1", "a2" \
  AND_MEMORY                            \
);                                      \
})

extern void memsetword();
#define memsetword(adr, wordvalue, length )              \
__extension__                           \
({                                      \
__asm__ volatile                        \
(                                       \
"move.l   %3,-(sp)\n\t"                    \
"move.w   %2,-(sp)\n\t"                    \
"move.l   %1,-(sp)\n\t"                    \
"jsr      (%0)\n\t"                         \
"lea      10(sp),sp"                          \
: /* outputs "=r"(retvalue) */           \
: /* inputs   */ "a"(memsetword), "g"((unsigned char *) adr), "g"((short) wordvalue), "g"((long) length) \
: /* clobbers */ "d0", "d1", "a0", "a1" \
  AND_MEMORY                            \
);                                      \
})

#endif
