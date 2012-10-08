        xdef    _memsetword

_memsetword:
        move.l  4+4(sp),d0                     ;d0:word value
        move.w  4+4(sp),d0                     ;d0:long value
        move.l  4+6(sp),d1                     ;d1:bytes to fill
        move.l  4+0(sp),a0                     ;a0:start of memory
        add.l   d1,a0
        add.l   d1,a0                          ;a0:end of memory
        movem.l d2-d7/a2-a5,-(sp)

        cmp.l   #50,d1
        ble.s   .enterfill2

                    moveq   #20,d2                         ; constant
                    move.l  d0,d3
                    move.l  d0,d4
                    move.l  d0,d5
                    move.l  d0,d6
                    move.l  d0,d7
                    move.l  d7,a1
                    move.l  d7,a2
                    move.l  d7,a3
                    move.l  d7,a4
                    move.l  d7,a5

.fill40:            sub.l   d2,d1
                    blt.s   .endfill40
                    movem.l d3-d7/a1-a5,-(a0)
                    bra.s   .fill40
.endfill40:         add.w   d2,d1
                    bra.s   .enterfill2
.fill2:             move.w  d0,-(a0)
.enterfill2:        dbra    d1,.fill2

                movem.l (a7)+,d2-d7/a2-a5
                rts
