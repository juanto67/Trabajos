; ==============================================================================
; PLATAFORMA MANAGEMENT
;X CENTER: (A0)
;Y CENTER: 2(A0)
;MOVX:     4(A0)
;MOVY:     6(A0)
; ==============================================================================
;CADA INIT PARA UNA DIFERENTE POSICION DE LA PLATAFORMA            
; ------------------------------------------------------------------------------
PINIT
; INITIALIZE PLATAFORMA 0.
; INPUT    : NONE 
; OUTPUT   : NONE
; MODIFIES : XCENTER, YCENTER, MOVIMIENTO
; ------------------------------------------------------------------------------
    MOVE.W  #100, (A0)          ;INICIALIZAR COORDENADAS DE PLATAFORMA
    MOVE.W  #350, 2(A0)
    MOVE.W  #1, 4(A0)
    MOVE.W  #1, 6(A0)
    RTS
; ------------------------------------------------------------------------------
PINIT1
; INITIALIZE PLATAFORMA 1.
; INPUT    : NONE 
; OUTPUT   : NONE
; MODIFIES : XCENTER, YCENTER, MOVIMIENTO
; ------------------------------------------------------------------------------
    MOVE.W  #100, (A0)          ;INICIALIZAR COORDENADAS DE PLATAFORMA
    MOVE.W  #200, 2(A0)
    MOVE.W  #5, 4(A0)
    MOVE.W  #1, 6(A0)
    RTS
    
; ------------------------------------------------------------------------------
PINIT2
; INITIALIZE PLATAFORMA 2.
; INPUT    : NONE 
; OUTPUT   : NONE
; MODIFIES : XCENTER, YCENTER, MOVIMIENTO
; ------------------------------------------------------------------------------
    MOVE.W  #500, (A0)          ;INICIALIZAR COORDENADAS DE PLATAFORMA
    MOVE.W  #0, 2(A0)
    MOVE.W  #5, 4(A0)
    MOVE.W  #3, 6(A0)
    RTS
    
; ------------------------------------------------------------------------------    
PUPDATE
; UPDATE PLATFORM COORDINATES, IF IT COLLIDES WITH A WALL, 
; DENY MOVEMENT TO UPDATE THE COORDINATE. THAT UPDATE IS USED FOR HORIZONTAL PLATFORMS
; INPUT    : NONE
; OUTPUT   : NONE
; MODIFIES : XCENTER
; ------------------------------------------------------------------------------
    
    MOVEM.W D0-D5,-(A7)

    MOVE.W (A0),D0
    MOVE.W  2(A0),D1
    
    ADD.W   4(A0),D0
    
    ; COMPARA SI CHOCA PARED
    CMP.W   #SCRWIDTH, D0
    BLO .UPAUX
    NEG.W   4(A0)
    ADD.W   4(A0),D0
    BRA .F
.UPAUX
    MOVE.W  D0, (A0) 
.F          
            ;MIRAMOS SI HAY COLISIONES EN LA PARTE SUPERIOR


            SUB.W #CALTURA/2,D1
            SUB.W #CANCHURA/2,D0
            
            MOVE.W (PLRPOSX),D2
            MOVE.W (PLRPOSY),D3
            ADD.W   #PLRHEIGH,D3
            MOVE.W #CANCHURA,D4
            MOVE.W #PLRWIDTH,D5
            ;USAMOS CONSTANTES PARA TENER UNICAMENTE LA PARTE
            ;DE ARRIBA DE LA PLATAFORMA PARA DETECTAR LA COLISIÓN
            MOVE.W #10,D6
            MOVE.W #5,D7
            
            JSR     UTLCHCOL
            
            
            CMP.B   #$FF,D0
            BNE .FIN
            ;AJUSTAMOS ENCIMA DE LA PLATAFORMA
            MOVE.W  2(A0),(PLRPOSY)
            SUB.W   #CALTURA/2,(PLRPOSY)
            SUB.W   #PLRHEIGH,(PLRPOSY)
            MOVE.W  #GRAVEDAD,(VGRAVEDAD)
            BRA     .FIN
            
    
.FIN   
    
    MOVEM.W (A7)+,D0-D5

    RTS    

; ------------------------------------------------------------------------------        
PUPDATE1
; UPDATE PLATFORM COORDINATES, IF IT COLLIDES WITH A WALL, 
; DENY MOVEMENT TO UPDATE THE COORDINATE.THAT UPDATE IS USED FOR VERTICAL PLATFORMS
; INPUT    : NONE
; OUTPUT   : NONE
; MODIFIES : XCENTER
; ------------------------------------------------------------------------------
    ;LA UNICA DIFERENCIA ES QUE AQUÍ MODIFICAMOS LA VELOCIDAD DE Y
    ; Y LA POSICIÓN EN Y DE LA PLATAFORMA
    MOVEM.W D0-D5,-(A7)

    MOVE.W (A0),D0
    MOVE.W  2(A0),D1
    
    ADD.W   6(A0),D1
    
    ; COMPARA SI CHOCA PARED
    CMP.W   #SCRHEIGH, D1
    BLO .UPAUX
    NEG.W   6(A0)
    ADD.W   6(A0),D1
    BRA .F
.UPAUX
    MOVE.W  D1, 2(A0) 
.F          
            ;MIRAMOS SI HAY COLISIONES EN LA PARTE SUPERIOR
                        
         

            SUB.W #CALTURA/2,D1
            SUB.W #CANCHURA/2,D0
            
            MOVE.W (PLRPOSX),D2
            MOVE.W (PLRPOSY),D3
            ADD.W   #PLRHEIGH,D3
            MOVE.W #CANCHURA,D4
            MOVE.W #PLRWIDTH,D5
             ;USAMOS CONSTANTES PARA TENER UNICAMENTE LA PARTE
            ;DE ARRIBA DE LA PLATAFORMA PARA DETECTAR LA COLISIÓN
            MOVE.W #10,D6
            MOVE.W #5,D7
            
            JSR     UTLCHCOL
            
            
            CMP.B   #$FF,D0
            BNE .FIN
            ;AJUSTAMOS ENCIMA DE LA PLATAFORMA
            MOVE.W  2(A0),(PLRPOSY)
            SUB.W   #CALTURA/2,(PLRPOSY)
            SUB.W   #PLRHEIGH,(PLRPOSY)
            MOVE.W  #GRAVEDAD,(VGRAVEDAD)
            BRA     .FIN
            
    
.FIN   
    
    MOVEM.W (A7)+,D0-D5

    RTS

; ------------------------------------------------------------------------------    
PPLOT
; DRAW A RECTANGLE WITH HEIGHT 40 AND WIDTH 100
; Input    - XCENTER Y YCENTER
; Output   - None
; Modifies - Nothing
; -----------------------------------------------------------------------------

            MOVEM.L D0-D7, -(A7)
            MOVE.B  #80,D0                  
            MOVE.L  #$00FFFFFF,D1
            TRAP    #15
            MOVE.B  #81,D0                  ;-------- COLORES
            MOVE.L  #$00800000,D1
            TRAP    #15
                                 
            
            
            MOVE.W  2(A0),D2            ;-------- COORDENADAS Y
            SUB.W   #CALTURA/2,D2
            MOVE.W  D2,D4
            ADD.W   #CALTURA,D4
            
            MOVE.W  (A0),D1            ;-------- COORDENADAS X
            SUB.W   #CANCHURA/2,D1
            MOVE.W  D1,D3
            ADD.W   #CANCHURA,D3
            
            MOVE.B  #87,D0                  ;-------- DIBUJAR
            TRAP    #15
            
            MOVEM.L (A7)+,D0-D7   
            
            RTS 



