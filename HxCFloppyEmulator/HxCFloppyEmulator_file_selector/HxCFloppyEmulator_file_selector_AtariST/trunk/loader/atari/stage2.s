;exécuté dès que le loader a fini de lire X octets (ou Y secteurs)

;mapping mémoire:
; $4c6.l (_dskbuf):        pointeur vers secteur de boot
;  _dskbufp:               boot loader


;mapping mémoire fin du decrunch:
;STAGE2
;BASEPAGE
;TEXT
;DATA
;BSS
;RELOC/STACK

;à la fin du décrunch, reloger le programme, remplir la BASEPAGE, clearer la BSS+STACK, (non:positionner la STACK)

;    OPT L0              ; executable TOS
;    OPT P=68000
;    OPT D-,X-           ; no symbols debug
;    OPT Y-              ; no source debug

;    OPT O+, OW+
;    OPT O1+,OW1-        ; backward branches
;    OPT O6+,OW6-        ; forward branches


                include "const.s"

;a6:alStruct
;a5:début du bloc malloc
;a4:adresse de chargement

; FIRST, COPY STAGE2 TO THE START OF THE MALLOC'ED BLOCK. THEN JUMP IN IT.

STAGE2START     = *

                ;show the welcome screen and the loading bar
                pea     MSG1(pc)
                move.w  #9,-(a7)
                trap    #1
                addq.l  #6,a7
                
;copie le reste de stage2 vers le début du bloc:
                lea     PACKEDFILE(pc),a0
                move.l  a0,pPACKEDFILE-STAGE2START(a4)
                lea     .copyend-STAGE2START(a4),a4                     ;skip the copy rout
                move.w  #(PACKEDFILE-.copyend+3)/4-1,d0
.copystage2:    move.l  (a4)+,(a5)+
                dbra    d0,.copystage2
                jmp      -(PACKEDFILE-.copyend+3)/4*4(a5)
.copyend
                
; WE ARE NOW IN THE START OF THE MALLOC'ED BLOCk !
                                                            ;a0:Packed File source address
                lea     PACKEDFILE(pc),a1                   ;a1:Destination
                lea.l   4(a6),a4                            ;a4:offseted address
                move.l  a1,a6                               ;a6:Destination
                
                bra.s   decrunch
pPACKEDFILE:    ds.l    1                                   ;start adress of packed data
                dc.w    -1                                  ;last number of dots
wait_sync:      movem.l d0-d3/a0-a3,-(a7)
;a0: current address in the packed file
                move.l  a0,d3
                lea     pPACKEDFILE(pc),a2
                move.l  (a2)+,a1                            ;a1: adresse of packed file
                                                            ;a2: pointer to number of dots
                sub.l   a1,d3                               ;d0:current offset
                move.l  #PACKlen/38,d1
                divu    d1,d3
                bmi.s   .endplot                            ; < 0 : don't plot
                move.w  d3,d0
                sub.w   (a2),d3
                beq.s   .endplot                            ; same number of dots: do nothing
                move.w  d0,(a2)                             ;else save the number of dots
                subq.w  #1,d3
                
.plot:
                pea     (2<<16)+('.')               ; C_CONOUT "."
                trap    #1
                addq.l  #4,a7
                dbra    d3,.plot

.endplot:       movem.l (a7)+,d0-d3/a0-a3
                rts


N2B_ASYNC       =     1    ; when set, decrunch will wait if (a4)<a0
                              ; Use this to throttle real time loading. It is advised
                              ; to set (a4) as the current loaded byte - N (N=255 seems good)
                              ; when loading is finished, put -1 in (a4)

N2B_TOWER       =     1                                   ; 0,1 or 6-10  0:8  1:14  n:4n+14 bytes
N2B_WAIT_SYNC   MACRO
                bsr.s   wait_sync
                or      #16,ccr
                ENDM
decrunch:
                include "n2bd.s"


;                move.b  #$0e,$ffff8800.w
;                move.b  $ffff8800.w,d0              ; read PSG register 14
;                or.b    #6,d0                       ; deselect drivea & driveb
;                move.b  d0,$ffff8802.w
                

                ;a6:Destination
            
;************************************************** START RELOCA
                ;relocate .PRG in a6
                ;include "..\unpack\RELOCA4.S"
RELOC_PRG
            movem.l d6-d7/a4-a6,-(a7)                       ;save registers
            lea     256(a6),a6                              ; a6 ==> début du programme (TEXT)
            move.l  a6,d7                                   ; d7 ==> valeur à additionner
            move.l  a6,a4                                   ; a4 ==> Début du programme
            add.l   #TEXTlen+DATAlen,a4                     ; a4 ==> segment BSS = table de relocation
            move.l  (a4)+,d6
            beq.s   FIN2071

reloc2071:  adda.l  d6,a6
            add.l   d7,(a6)
            moveq   #0,d6
boucl2071:  move.b  (a4)+,d6
            beq.s   FIN2071
            cmpi.b  #1,d6
            bne.s   reloc2071
            lea.l   254(a6),a6
            bra.s   boucl2071

FIN2071:    movem.l (a7)+,d6-d7/a4-a6
;************************************************** END RELOCA

                                                            ;a6=basepage
            lea     256(a6),a5                              ;a5=TEXT (début du programme)
            move.l  a6,a4                                   ;a4=basepage
            move.l  a5,a3                                   ;a3=TEXT
            sub.l   a0,a0                                   ;a0=0 (important)

            lea     STAGE2START(pc),a1                      ;not used
            move.l  a6,(a4)+                                ;p_lowtpa 0x00 faked block start address (the malloc result is lower=a0)
            move.l  $436.w,(a4)+                            ;p_hitpa  0x04 top memory = end of the TIA block
            move.l  a3,(a4)+                                ;p_tbase  0x08 TEXT
            move.l  #TEXTlen,d0
            move.l  d0,(a4)+                                ;p_tlen   0x0c TEXTlen (+ stage2len)
            add.l   d0,a3
            move.l  a3,(a4)+                                ;p_dbase  0x10 DATA
            move.l  #DATAlen,d0
            move.l  d0,(a4)+                                ;p_dlen   0x14 DATAlen
            add.l   d0,a3                                   ;              a3=BSS
            move.l  a3,(a4)+                                ;p_bbase  0x18 BSS
            move.l  #BSSlen,d0
            move.l  d0,(a4)+                                ;p_blen   0x1C BSS length
            add.l   #STACKlen,d0          ;d0:length to clear (BSS + STACK)
                                          ;a3:start of BSS+Stack
            
            move.l  a0,(a4)+                                ;p_dta    0x20 DTA
            move.l  a0,(a4)+                                ;p_parent 0x24 parent BP
            move.l  a0,(a4)+                                ;p_reserv 0x28
            move.l  a4,(a4)
            addq.l  #4,(a4)+                                ;p_env    0x2c environment
                                                            ;p_undef  0x30 80 bytes unused reserved
                                                            ;p_cmdlin 0x80 128 bytes command line

;Efface la BSS et la STACK
            lsr.l   #2,d0
.clearbss:  move.l  a0,(a3)+
            dbra    d0,.clearbss

;Positionne la pile utilisateur
            move.l  a3,usp

;Passe en mode utilisateur
            and     #$dfff,sr

;PrÃ©pare le lancement
            pea     (a6)
            pea     0.w
            ;sub.l   a0,a0                                  ;a0=0
            IF MINTSTACKOFFSET!=-1
                move.l  a5,a4
                add.l   #MINTSTACKOFFSET-BASEPAGElen-4,a4
                add.l   #MINTSTACKADD,(a4)
            ENDIF

            jmp     (a5)

                
                
                ;le fichier décompacté et relogé se trouvera ici.
                ;inutile de faire un jmp (a6), donc

MSG1:       ;dc.b    27,'e'          ; show cursor
            dc.b    27,'w'          ; wrap off
            dc.b    "Loading HxC Manager...                 "
            dc.b    10              ;line down
            dc.b    27,'o'          ;clear start of line
            dc.b    ']'
            dc.b    13              ;go to start of line
            dc.b    '[',0
;            dc.b    "[......................................]"
            EVEN

PACKEDFILE:
