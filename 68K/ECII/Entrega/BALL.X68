; ==============================================================================
; BALL MANAGEMENT (AGENT)
; BALPOSX (W)  (A0)
; BALPOSY (W) 2(A0)
; BALVELX (W) 4(A0)
; BALVELY (W) 6(A0)
; ==============================================================================
            
; ------------------------------------------------------------------------------
BALINIT
; INITIALIZE BALL.
; INPUT    : A0 POINTER TO THE INSTANCE VARIABLES 
; OUTPUT   : NONE
; MODIFIES : NONE
; ------------------------------------------------------------------------------
            MOVEM.L D0-A2,-(A7),-(A7)
            CLR D2
            JSR     UTLRAND
            LEA     .VELVEC,A1
            LEA     .POSVEC,A2
            AND.L   #$0000000C,D0           ;MASCARA PARA TENER DE 0-12           
            MOVE.L  (A2,D0),(A0)
            
            
            MOVE.W D0,D1  
            CLR.L D0
            JSR     UTLRAND
            BTST #0, D0        ;OTRO RANDOM PARA CONSEGUIR LAS VELOCIDADES
            BNE .BitIsClear    ;SI ES 0 SE VA A LA SIGUIENTE VELOCIDADES  
            MOVE.W D1,D2
            CMP #12,D2        ;SI ES EL ULTIMO SE QUEDA CON ESE
            BEQ .BitIsClear
            ADDQ.W #4,D1      
            
.BitIsClear  
            
            MOVE.L  (A1,D1),4(A0)
            
            
            MOVEM.L (A7)+,D0-A2

            RTS

.VELVEC     DC.W    BALVEL,-BALVEL      ;0
            DC.W    BALVEL,BALVEL       ;4
            DC.W    -BALVEL,-BALVEL     ;8
            DC.W    -BALVEL,BALVEL          ;12
.POSVEC     DC.W    0,160     ;0
            DC.W    0,320       ;4
            DC.W    SCRWIDTH,160      ;8
            DC.W    SCRWIDTH,320       ;12

            
; ------------------------------------------------------------------------------
BALUPD
; UPDATES THE BALL.
; INPUT    : A0 POINTER TO THE INSTANCE VARIABLES  
; OUTPUT   : NONE
; MODIFIES : NONE
; ------------------------------------------------------------------------------

            MOVEM.W D0-D7,-(A7)
            
            ; ACTUALIZACIÓN DE COORDENADAS 
            MOVE.W  (A0),D0
            MOVE.W  2(A0),D1
            
            ADD.W   4(A0),D0
            ADD.W   6(A0),D1
            
            
          ; REVISAMOS LAS COLISIONES HORIZONTALES
            CMP.W   #SCRWIDTH-BALRAD,D0
            BGT     .COLRGT
            CMP.W   #BALRAD,D0
            BLT     .COLLFT
            
            ; REVISAMOS LAS COLISIONES VERTICALES
.CHKVERT    CMP.W   #SCRHEIGH-BALRAD,D1
            BGT     .COLBOT
            CMP.W   #BALRAD,D1
            BLT     .COLTOP
            BRA     .SHIELDCOLISION
            
            ; REVISAMOS LAS COLISIONES EN LA DERECHA
.COLRGT     NEG.W   4(A0)
            MOVE.W  #SCRWIDTH-BALRAD,D0
            BRA     .CHKVERT
            
            ;REVISAMOS LAS COLISIONES EN LA IZQUIERDA
.COLLFT     NEG.W   4(A0)
            MOVE.W #BALRAD,D0
            BRA     .CHKVERT

            ; REVISAMOS LAS COLISIONES ABAJO
.COLBOT     NEG.W   6(A0)
            MOVE.W  #SCRHEIGH-BALRAD,D1
            BRA     .SHIELDCOLISION
            
            ; REVISAMOS LAS COLISIONES ARRIBA

.COLTOP      NEG.W   6(A0)
            MOVE.W  #BALRAD,D1
            BRA .SHIELDCOLISION
            
.SHIELDCOLISION
            
            ;Miramos primero las colisiones del escudo, pues antes de 
            ;tocar al jugador, se toca el escudo(si está activo)
            MOVE.W  D0,(A0)
            MOVE.W  D1,2(A0)
            
            SUB.W   #BALRAD,D0
            SUB.W   #BALRAD,D1
            MOVE.W D0,(BALPRV1)
            MOVE.W D1,(BALPRV2) ;guardar por si despues miramos la del player
            
            ;MIRAMOS SI EL ESCUDO ESTA ACTIVO, para eso, dos comprobaciones
            ;1 (esta el espacio presionado)?
            ;2(estamos en el tiempo disponible para la activación del mismo)?
            MOVE.B  #19,D0
            MOVE.L  #KBDSHIELD<<24,D1
            TRAP #15
            BTST #24,D1
            
            BEQ .PLRCOLISION ;No estamos presionando espacio, miramos si
            ;la bola toca al jugador
            
            ;HEMOS PRESIONADO ESPACIO!
            MOVE.W (SHIELDTIMEEN),D6
            MOVE.W (VTIMER),D7
            CMP D7,D6
            BLE .PLRCOLISION                ;NO estamos en el tiempo indicado
            
            CLR.L D6
            CLR.L D7           
            
            ;Preparamos los datos para llamar al UTLCHCOL 
            MOVE.W (BALPRV1),D0
            MOVE.W (BALPRV2),D1
            MOVE.W (PLRPOSX),D2
            SUB.W #15,D2
            
            MOVE.W (PLRPOSY),D3
            SUB.W #15,D3
       
            
            MOVE.W #BALRAD*2,D4
            MOVE.W #SHIELDATA,D5
            
            MOVE.W #BALRAD*2,D6
            MOVE.W #SHIELDATA,D7
            
            JSR     UTLCHCOL
            TST.B   D0
            ;Si no hay colisión no hacemos nada
            BEQ     .DONE
            ;Si hay colisión matamos el agente, es decir
            ;quitamos la bola tocada por el escudo.
            JSR     AGLKILL
            BRA .DONE
            
            
            


.PLRCOLISION
           
            
            ;Teniamos guardadas X e Y
            ; en BALPRV1 y BALPRV2
            MOVE.W   (BALPRV1),D0
            MOVE.W   (BALPRV2),D1
            
            MOVE.W (PLRPOSX),D2
            MOVE.W (PLRPOSY),D3
            
            MOVE.W #BALRAD*2,D4
            MOVE.W #PLRWIDTH,D5
           
            MOVE.W #BALRAD*2,D6
            MOVE.W #PLRHEIGH,D7
            
            JSR     UTLCHCOL
            TST.B   D0
            BEQ     .DONE
            ;GAME OVER
            
            ;DESHABILITAMOS EL DOUBLE BUFFER
            ;Debemos deshabilitar el double buffer, porque los
            ;textos de arriba a la izquierda se quedan aunque borremos
            ;la pantalla mediante otro trap 15.
            MOVE.W #16,D1
            MOVE.W #92,D0
            TRAP #15
            
            ;Limpiar pantalla : Ponemos D1.W a $FF00
            MOVE.W #11,D0
            MOVE.W #$FF00,D1
            TRAP #15
            
            
            MOVE.L #$00000000,D1
            MOVE.W #81,D0        ; PONEMOS FILL COLOR
            TRAP #15
            
            			
	MOVE.L #$000000FF,D1
            MOVE.W #80,D0        ; PONEMOS PEN COLOR
            TRAP #15

            ;Cargamos los textos indicados
            LEA GAMEOVER1,A1
            MOVE.W #14,D0
            TRAP #15
           
            
            
            
            LEA GAMEOVER2,A1
            
            MOVE.W #14,D0
            TRAP #15
            
            LEA GAMEOVER3,A1
            MOVE.W #14,D0
            TRAP #15
            
            LEA GAMEOVER4,A1
            MOVE.W #14,D0
            TRAP #15
            
            LEA GAMEOVER5,A1
            MOVE.W #14,D0
            TRAP #15
            

            SIMHALT            
           
            
            
            


            


            
.DONE
            MOVEM.W (A7)+,D0-D7
            RTS

            
; ------------------------------------------------------------------------------
BALPLOT
; PLOT THE BALL.
; INPUT    : A0 POINTER TO THE INSTANCE VARIABLES 
; OUTPUT   : NONE
; MODIFIES : NONE
; ------------------------------------------------------------------------------
           MOVEM.L D0-D4,-(A7)
            
            
            
            ; PONEMOS PEN COLOR
            MOVE.B  #80,D0
            MOVE.L  #$00FF0000,D1
            TRAP    #15
            
            ; PONEMOS FILL COLOR
            MOVE.B  #81,D0
            MOVE.L  #$00FF0000,D1
            TRAP    #15
            
            ; DEFINIMOS COORDENADAS
            MOVE.W  (A0),D1
            SUB.W   #BALRAD,D1
            MOVE.W  2(A0),D2
            SUB.W   #BALRAD,D2
            MOVE.W  D1,D3
            ADD.W   #2*BALRAD,D3
            MOVE.W  D2,D4
            ADD.W   #2*BALRAD,D4
            
            ; DIBUJAMOS EL CIRCULO
            MOVE.B  #88,D0
            TRAP    #15
            
            MOVEM.L (A7)+,D0-D4

            RTS






