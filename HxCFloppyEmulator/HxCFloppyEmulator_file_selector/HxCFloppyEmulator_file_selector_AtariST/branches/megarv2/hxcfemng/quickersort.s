; quickersort

            xdef    _quickersort

_quickersort:
;parameters:
;4(sp).w : number of strings to sort
;6(sp).w : offset from the data to sort. 0 to sort from the first byte
;8(sp).l : pointer to array of pointers to the strings to be sorted.
;          This array will be sorted

            movem.l     a0-a5/d0-d3,-(sp)

            moveq   #0,d0
            move.w  $28+4(sp),d0                ;d0:number of strings
            move.l  d0,d1                   ;d1:number of strings:initial gap

            moveq   #0,d3
            move.w  $28+6(sp),d3                ;d3:offset to sort from

            ;compute end of pointers:
            move.l  $28+8(sp),a2
            lsl.l   #2,d0
            add.l   d0,a2                   ;a2:end of pointers

reducegap:  move.l  d1,d0                   ;divide d1 by 1.28
            add.l   d1,d1                   ;compute d1 = d1 * 25 /32
            add.l   d1,d1
            add.l   d0,d1
            move.l  d1,d0
            add.l   d1,d1
            add.l   d1,d1
            add.l   d0,d1
            lsr.l   #5,d1

            cmp.w   #8,d1
            bls.s   endpart                 ;gap<=8  : 'endpart'
            lsl.l   #2,d1                   ;d1:gap*4

            ;initpass:
            move.l  $28+8(sp),a0                ;a0:string LEFT
            lea     0(a0,d1.l),a1           ;a1:string RIGHT

.comp:      ;compare:
            move.l  (a0)+,a3
            move.l  (a1)+,a4
            bsr.s   compare
            bls.s   .noswap
            move.l  a3,-4(a1)
            move.l  a4,-4(a0)

.noswap:    cmp.l   a2,a1
            blo.s   .comp

            ;reduce gap:
            lsr.l   #2,d1
            bra.s   reducegap

endpart:    ;insertion sort
            ;d1.l<=8
            ;a2: end of pointers


            move.l  $28+8(sp),a5    ;a5:first string pointer
            addq.l  #4,a5       ;a5:second string pointer
            moveq   #0,d2       ;d2: number-1 of strings to test

.pass:                          ;a5:string RIGHT (constant in the pass)
            move.l  a5,a0       ;a0:string LEFT (will be predecremented)
                                ;at the start of each pass, a0=a5
            move.w  d2,d1       ;d1:dbxx counter: nb-1 strings to test
            move.l  (a5),a4     ;RIGHT value

.comp:      ;compare:
            move.l  -(a0),a3    ;LEFT value
            bsr.s   compare
            dbls    d1,.comp

            cmp.w   d1,d2
            beq.s   .next

            ;insertion
            move.l  a5,a3       ;a3:left (will be predecremented)
            lea     4(a5),a4    ;a4:right (will be predecremented)
            move.l  (a5),a0     ;a0:value to insert
            sub.w   d2,d1
            neg.w   d1          ;d1
            subq.w  #1,d1
.mov:       move.l  -(a3),-(a4)
            dbra    d1,.mov
            move.l  a0,(a3)

.next:      addq.l  #4,a5       ;next
            addq.w  #1,d2       ;one more string to test
            cmp.l   a2,a5
            blo.s   .pass

            movem.l     (sp)+,a0-a5/d0-d3
            rts


compare:    ;left:a3
            ;right:a4
            ;offset:d3
            movem.l a3-a4/d0-d1,-(sp)
            adda.w   d3,a3
            adda.w   d3,a4
.next:      move.b  (a3)+,d0
            beq.s   .leftz
            move.b  (a4)+,d1
            beq.s   .cmpend
            cmp.b   d1,d0
            beq.s   .next
            movem.l (sp)+,a3-a4/d0-d1
            rts
.leftz:     move.b  (a4)+,d1
.cmpend:    cmp.b   d1,d0
            movem.l (sp)+,a3-a4/d0-d1
            rts
