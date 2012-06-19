
                include "equates.s"

BASEPAGElen     =     $100
STACKlen        =     $1000
MINTSTACKADD    =     $3000
MALLOCsize      =     STAGE2len+BASEPAGElen+TEXTlen+DATAlen+BSSlen+STACKlen   ;stage1
LOADoffset      =     (MALLOCsize-STAGE2len-PACKlen+1)/2*2                    ;stage1
;LOADsize        equ     PACKlen+STAGE2len


; SAMPLE EQUATES.S:
;MINTSTACKOFFSET equ     168836                     ;stage2
;TEXTlen         equ     $26048                     ;stage2
;DATAlen         equ     $3708                      ;stage2
;BSSlen          equ     $88d0                      ;stage2
;STAGE2len       equ     322                        ;stage1
;PACKlen         equ     82896                      ;stage1

