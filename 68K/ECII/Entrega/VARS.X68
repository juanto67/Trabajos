; =============================================================================
; GAME VARS
; =============================================================================

; --- TIMED INTERRUPT RELATED VARIABLES ----------------------------------------

SCRINTCT    DS.B    1                       ; INTERRUPT COUNTER
SCRCYCCT    DS.B    1                       ; CYCLE COUNTER
            DS.W    1
ERROR       DS.W    1                       ;ERROR DE LOS FPS
FPSAC       DS.L    1
FPSIN       DS.L    1
ENFPS       DS.W    1
FPSAN       DS.W    1
; --- GAME STATE RELATED VARIABLES --------------------------------------------

STACUR      DS.W    1                       ; CURRENT GAME STATE
STANEXT     DS.W    1                       ; NEXT GAME STATE
; --- KEYBOARD RELATED VARIABLES -----------------------------------------------

KBDVAL      DS.B    1                       ; KEY STATE
KBDEDGE     DS.B    1                       ; KEY EDGE

; --- PLAYER RELATED VARIABLES ------------------------------------------------

PLRPOSX     DS.W    1                       ; PLAYER X POSITION (PIXELS)
PLRPOSY     DS.W    1                       ; PLAYER Y POSITION (PIXELS)
PLRENER     DS.W    1                       ; PLAYER ENERGY
PLRDMG      DS.B    1                       ; BIT0: UPD DAMAGE, BIT1 PLOT DMG
            DS.W    0

            
VGRAVEDAD   DS.W    1 

; --- SCREEN RELATED VARIABLES ---------------------------------------------

VTIMER      DS.W    1                       ;VALOR EN SEGUNDOS
SEGUNDOS    DS.W    1
POINT       DS.W    1                          ;PUNTOS CONSEGUIDOS  

;-----------------MOUSE DECLARATIONS VARS----------
ENATP       DS.W    1
LASTENA     DS.W    1
MOUSEPOSX   DS.W    1
MOUSEPOSY   DS.W    1
TIMEENABLE  DS.W    1

;----------SHIELD DECLARATIONS VARS
SHIELDENA   DS.W 1
SHIELDTIMEEN DS.W 1
SHIELDCOOLDOWN DS.W 1
SHIELDINICIO DS.W 1
;---------------PLATFORM DECLARATION VARS
XCENTER     DS.W 1
YCENTER     DS.W 1
MOVIMIENTO  DS.W 1
PLTEN       DS.W    1
PLTEN1      DS.W    1
;BALLS
BALEN       DS.W    1






