; -----------------------------------------------------------------------------
PLRINIT            
; INIT THE POSITION OF THE PLAYER
; INPUT    - NONE (ACCESSES KBDVAL AND KBDEDGE)
; OUTPUT   - NONE (MODIFIES PLAYER DATA)
; MODIFIES - NONE
;----------------------------------------------------------------------------           
            MOVE.W #GRAVEDAD,(VGRAVEDAD)
            MOVE.W  #(SCRWIDTH)/2,(PLRPOSX)
            MOVE.W  #(SCRHEIGH-PLRHEIGH),(PLRPOSY)
            RTS
            
; -----------------------------------------------------------------------------            
PLRUPD
; UPDATES THE PLAYER MOTION
; INPUT    - NONE (ACCESSES KBDVAL AND KBDEDGE)
; OUTPUT   - NONE (MODIFIES PLAYER DATA)
; MODIFIES - NONE
; -----------------------------------------------------------------------------
            MOVEM.W  D1/A3/D2/D3,-(A7)
            
            
            MOVE.B  (KBDVAL),D1             ; HACER EL MOVIMIENTO Y MIRAR LOS BORDES
            LEA     VGRAVEDAD,A3
            MOVE.W   (PLRPOSY),D2
            ADD.W   (A3),D2                         ;AÑADIMOS GRAVEDAD
            MOVE.W  D2,(PLRPOSY)
           
            ROR.B   #1,D1
            BCC     .CHKUP
            SUB.W   #PLRSPEEDX,(PLRPOSX)
            TST.W   (PLRPOSX)
            BGE     .CHKUP
            CLR.W   (PLRPOSX)
.CHKUP      ROR.B   #1,D1
            BCC     .CHKRT
            
            SUB.W   #PLRSPEEDY,(PLRPOSY) 
            ADDQ.W  #1,(VGRAVEDAD)    ;INCREMENTAMOS LA GRAVEDAD PARA QUE HAYA
                                      ;UN MOMENTO DONDE SEA MAYOR A LA VELY
            TST.W   (PLRPOSY)
            BGE     .CHKRT
            CLR.W   (PLRPOSY)
.CHKRT      ROR.B   #1,D1
            BCC     .CHKDN
            ADD.W   #PLRSPEEDX,(PLRPOSX)
            CMP.W   #SCRWIDTH-PLRWIDTH,(PLRPOSX)
            BLE     .CHKDN
            MOVE.W  #SCRWIDTH-PLRWIDTH,(PLRPOSX)
.CHKDN      ROR.B   #1,D1
            BCC     .MDONE
            ADD.W   #PLRSPEEDY,(PLRPOSY)
           
            BRA     .MDONE
           
.MDONE      
            CMP.W   #SCRHEIGH-PLRHEIGH,(PLRPOSY)
            BLE     .FINAL
            MOVE.W  #SCRHEIGH-PLRHEIGH,(PLRPOSY)
            MOVE.W #GRAVEDAD,(VGRAVEDAD)            ;CUANDO TOCAMOS SUELO DEJAMOS
                                            ;LA GRAVEDAD COMO CONSTANTE ANTERIOR
            
            
.FINAL      MOVEM.W  (A7)+,D1/A3/D2/D3
            RTS
; -----------------------------------------------------------------------------            
DIBUJAR     
; DRAW THE CHARACTER IMAGE
; INPUT    - X AND Y
; OUTPUT   - NONE 
; MODIFIES - NONE
; -----------------------------------------------------------------------------
            ;Dibujaremos un numero constante de pixeles, contados por el programa
            ;que escribe los RGB en un documento en binario, usaremos FILLOAD, para
            ;tener los RGB en memoria, además de utilizar unas variables llamadas X e Y
            ;que indican las coordenadas de cada pixel. Cada pixel lo moveremos segun 
            ;PLRPOSX y PLRPOSY

            MOVEM.L D0-D6,-(A7)
            MOVEM.L A0-A6,-(A7)
            
            MOVE.W (PLRPOSX),D5
            MOVE.W (PLRPOSY),D6
            ;CARGAMOS LOS DATOS
            LEA PIXEL,A0
            LEA FILENAMEPIX,A1
            JSR FILLOAD
            
            LEA X,A3
            LEA Y,A4            
           
            MOVE.W (PLRPOSX),D5
            MOVE.W (PLRPOSY),D6
            
            MOVE.W #896,D4
            SUBQ.W #1,D4
            
            
            
LOOPDIB:
            
            MOVE.B #80,D0
            MOVE.L (A0)+,D1
            TRAP #15 ;PEN COLOR
            MOVE.B #82,D0
            CLR.L D1
            MOVE.L (A3)+,D1
            MOVE.L (A4)+,D2
            ADD.L D5,D1
            ADD.L D6,D2
            
            TRAP #15 ;Dibujar pixel
            DBRA D4,LOOPDIB
           ; JSR BALPLOT
            
            MOVEM.L (A7)+,A0-A6
            MOVEM.L (A7)+,D0-D6
            RTS
            





*~Font name~Courier New~
*~Font size~10~
*~Tab type~0~
*~Tab size~4~
