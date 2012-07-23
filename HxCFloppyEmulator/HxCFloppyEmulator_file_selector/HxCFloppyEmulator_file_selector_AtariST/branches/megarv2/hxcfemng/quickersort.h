#ifndef __QUICKERSORT_H__
#define __QUICKERSORT_H__


#define quickersort_call(func, a, b, c )              \
__extension__                           \
({                                      \
short _func = (long )(func);                  \
short _a = (short)(a);                  \
short _b = (short)(b);                  \
long  _c = (long)(c);                   \
                                        \
__asm__ volatile                        \
(                                       \
"move.l   %3,-(sp)\n\t"                    \
"move.w   %2,-(sp)\n\t"                    \
"move.w   %1,-(sp)\n\t"                    \
"jsr      (%0)\n\t"                         \
"addq.l    #8,sp"                          \
: /* outputs "=r"(retvalue) */           \
: /* inputs   */ "a"(_func), "g"(_a), "g"(_b), "g"(_c) \
: /* clobbers */ "d0", "d1", "d2", "a0", "a1", "a2" \
  AND_MEMORY                            \
);                                      \
})


#endif
