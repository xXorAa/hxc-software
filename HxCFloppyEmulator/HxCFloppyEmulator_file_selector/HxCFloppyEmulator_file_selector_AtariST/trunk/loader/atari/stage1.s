;
;    OPT L0              ; TOS executable TOS
;    OPT P=68000
;    OPT D-,X-           ; no symbols debug
;    OPT Y-              ; no source debug
;
;                OPT O+,OW+                                  ;all optimisations warns
;O2: d16(an) -> (an) when d16=0


;pour autoload:
NOTRACKINFO     =     1                               ;don't update alTrackCurrent
NOSEEK          =     1                               ;don't seek first
FORCE2SIDES     =     1
FORCESECTORS    =     9                               ;0/9/10/11 use 0 to not force
USEMOVEP        =     0                               ;recommended to leave to 0, as the 68060 doesn't know movep
SHOWCOLORS      =     0
DONTSAVEREGS    =     1                               ;don't save/restore all registers when calling fdcAutoloader. Saves 8 bytes
DONTINITA0      =     0                               ;set to 1 if A0=alStruct before calling fdcAutoloader. Saves 4 bytes
DONTTOUCHVBL    =     1                               ;does not install the motoroff vbl. fdcVBL should still be called to handle motoroff. Saves 28 bytes
DONTINCLUDEVBL  =     1                               ;saves an extra 98 bytes. Motoroff code must be implemented
ASYNCADDR2      =     1                               ;maintain a second, offseted address (used for asynchronous loading like real time depacking). Costs 8 bytes + 4 in alStruct

;    OPT O+, OW+
;    OPT O1+,OW1-        ; backward branches
;    OPT O6+,OW6-        ; forward branches

    include "const.s"

        
            
                move.l  #(MALLOCsize+511)/512*512,-(a7)
                move.w  #$48,-(a7)                              ;MALLOC
                trap    #1
                addq.l  #6,a7
            
                ;d0: address of the malloced block            

                lea     alStruct(pc),a6                     ;a6:alStruct
                move.l  d0,a5                               ;a5:début du bloc malloc
                add.l   #LOADoffset,d0                      ;d0:adresse de chargement
                move.l  d0,a4                               ;a4:adresse de chargement
                move.l  d0,(a6)+                            ;alAddress
                add.l   d0,(a6)+                            ;alAddress2
                move.l  (a6),d7                             ;d7=nb sector to read
                subq.l  #8,a6                               ;a6:alStruct

                bsr.s   fdcAutoloader                       ;prend ses paramètres de alStruct. Aucun registre modifié

    IFNE STAGE2len/512>0
    
                subq.l  #STAGE2len/512,d7                   ;nombre de secteur supplémentaire à attendre avant
                                                            ;de lancer stage2 (0 signifie, attend un secteur)
    ENDC
.wait1sector:   cmp.l   8(a6),d7
                bls.s   .wait1sector

                jmp     (a4)

        



alStruct:       dc.l    0                   ;address to read to
                dc.l    -256               ;offset for address2
                dc.l    (STAGE2len+PACKlen+511)/512   ;number of sectors to read
                dc.b    2                   ;sector to start to read from    ; sector 2
                dc.b    0                   ;side to start to read from      ; side 0
;                dc.b    0                   ;motoron                         ; can be saved to save 2 bytes
;                EVEN

SUPER:          include "autoload.s"






