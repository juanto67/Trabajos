; =============================================================================
; TITLE    : SYSTEM MANAGEMENT (SYSTEM CODE)
; AUTHOR   : Juan, Antonio y Jia Sheng
; COMMENTS : IT IS DIVIDED IN THREE BLOCKS:
;            - SYS : GENERIC.
;            - SCR : SCREEN MANAGEMENT.
;            - KBD : KEYBOARD MANAGEMENT.
;            - MOUSE : MOUSE MANAGEMENT
;            - FIL : FILES MANAGEMENT
;            - FPS : FPS MANAGEMENT
; =============================================================================

; =============================================================================
SCRISR
; SCREEN TIMER ISR. INCREASES THE INTERRUPT COUNTER AND UPDATES DOUBLE BUFFER.
; INPUT    - NONE
; OUTPUT   - NONE
; MODIFIES - NONE
; -----------------------------------------------------------------------------
            ADDQ.B  #1,(SCRINTCT)           ; ACTUALIZA INT FLAG/CONTADOR
            ADDQ.B  #1,(SCRCYCCT)           ; ACTUALIZAR EL CONTADOR DE CICLOS
            RTE
            
; =============================================================================
SCRINIT
; INITIALIZE TRAPS IN ADDITION TO THE SCREEN AND FPS.
; INPUT    - NONE
; OUTPUT   - NONE
; MODIFIES - NONE
; -----------------------------------------------------------------------------
            ORI.W   #$0700,SR               ; DESHABILITAR INTERRUPCIONES
            MOVE.L  #SCRUPD,($80+SCRTRAP*4) ; TRAP
            MOVE.L  #SCRWIDTH<<16|SCRHEIGH,D1 ; PONEMOS LA RESOLUCIÓN
            MOVE.B  #33,D0
            TRAP    #15
            MOVE.L  #1,D1                   ; ACTIVAMOS EL MODO VENTANA
            TRAP    #15
            MOVE.W  #$FF00,D1               ; VACIAMOS EL BUFFER VISIBLE
            MOVE.B  #11,D0
            TRAP    #15
            MOVE.B  #17,D1                  ; ACTIVACIÓN DOBLE BUFFER
            MOVE.B  #92,D0
            TRAP    #15
            MOVE.W  #$FF00,D1               ; VACIAMOS EL BUFFER ESCONDIDO
            MOVE.B  #11,D0
            TRAP    #15
            MOVE.L  #SCRISR,($64)       ; 60+SCRIRQ*4) ; EL VECTOR DE EXCEPCIONES
            MOVE.B  #5,D1                   ; HABILITAMOS EL PROCESAMIENTO DE LAS EXCEPCIONES
            MOVE.B  #32,D0
            TRAP    #15
            ; HABILITAMOS LAS INTERRUPCIONES "TIMED"
           CLR.L D5
           CLR.L D6
           CLR.L D4
           MOVE.W   #1,(ENFPS)
            MOVE.W   #0,(FPSAN)
           MOVE.W #1000,D5
           MOVE.W #FPS,D4
            MOVE.B  #32,D0
            MOVE.B  #6,D1
            MOVE.B  #$81,D2
            DIVU.W  #FPS, D5         ; Divide D0 por D1 (D0 = cociente y resto)
            MOVE.W D5,D6
            MOVE.L D6,D3
            TRAP    #15
            CLR.L D6
             MOVE.L  D5, D6         ; Copia D0 para extraer el resto
            ANDI.L  #$00FF, D6     ; Mascara para obtener el resto (20)
            MULU.W  #1000, D6      ; Multiplicar el resto por 1000

            DIVU.W  D4,D6          ; D4 = parte decimal (2857)
            MOVE.W  D6,(ERROR)

            
            
            CLR.B   (SCRINTCT)              ; PREPARAMOS LOS DATOS DE LA INTERRUPCION
            CLR.B   (SCRCYCCT) 
            ANDI.W  #$F8FF,SR               ; ACTIVAMOS LAS INTERRUPCIONES            
            RTS

; =============================================================================
FPSUPD
; UPDATE THE FPS WITH THE GIVEN PARAMETERS.
; INPUT    - (FPSIN) AND (FPSAC) AND (SCRCRYCCT)
; OUTPUT   - (FPSAN)
; MODIFIES - (FPSAN)
; -----------------------------------------------------------------------------
            MOVEM.W D0-D6,-(A7)
            MOVE.L (FPSIN),D3               ;CALCULA LA DIFERENCIA
            MOVE.L  (FPSAC),D4
            SUB.L   D3,D4
           
            CMP.W   #0,D4                   ;MIRAMOS SI ES NEGATIVA O 0
            BEQ     .FF
            CMP.W   #0,D4
            BGE     .ND
            NEG.L D4                        ;NEGAMOS PARA CALCULAR EL MODULO
.ND

            MULS    #10, D4                 ;CONVERSIÓN A MS                 
            CLR.L D6
            CLR.L D5
            MOVE.B  (SCRCYCCT),D6           ;DIVIDIMOS POR EL NÚMERO DE CICLOS
.AA         SUB.L   D6,D4                   ;PARA OBTENER LA MEDIA
            CMP.L   #0,D4
            BLT     .TN
            ADDQ.W  #1,D5
            BRA .AA
.TN         CLR.L D3
            CLR.L D4
            CMP.W   #0,D5                   ;MIRAMOS SI ES 0
            BEQ     .FF
            MOVE.L  #1000,D3
.CA         SUB.L     D5,D3                 ;1000/TIME(ms)
            CMP.L   #0,D3
            BLT     .SN
            ADDQ.W  #1,D4
            BRA .CA
.SN         
                
            
            MOVE.W  D4,(FPSAN)              ;GUARDAMOS EL RESULTADO PARA USARLO DESPUÉS
       
     
            BRA .TRAP
.FF         MOVE.W #100,(FPSAN)             ;SIGNIFICA QUE ES MENOR A 1*10^-2 s  
.TRAP                                       ;POR LO TANTO, MÁS DE 100 FPS

            MOVEM.W (A7)+,D0-D6
            RTS 
            

; =============================================================================
SCRPLOT
; TRAP SERVICE ROUTINE IN CHARGE OF VISUALIZING CURRENT FRAME AND CLEARING
; BUFFER FOR THE NEXT ONE.
; INPUT    - NONE
; OUTPUT   - NONE
; MODIFIES - NONE
; -----------------------------------------------------------------------------            
	MOVEM.L D0-D7,-(A7)
	
	MOVE.L #$00FF0000,D1
            MOVE.W #81,D0        ; SET FILL COLOR
            TRAP #15
            
	LEA TIMER,A1
	CLR.L D1
            CLR.L D0
            MOVE.W (VTIMER),D4
            MOVE.W (VTIMER),D1
            MOVE.B #17,D0
            TRAP #15 ;VISUALIZACIÓN DEL TIMER
            
             ;DEBAJO HACEMOS LA VISUALIZACION DE LOS FPS
            LEA TXTFPS,A1
            CLR.L D1
            
            MOVE.W #FPS,D1
            MOVE.B #17,D0
            TRAP #15
            
             LEA TXT2FPS,A1
            CLR.L D1
            MOVE.W (FPSAN),D1
            MOVE.B #17,D0
            TRAP #15


            
            MOVE.L #$00FFFFFF,D1
            MOVE.W #80,D0        ; SET PEN COLOR
            TRAP #15
            
            LEA SHIELD,A1
	
            MOVE.B #14,D0
            TRAP #15
            ;Si en D3 hay un 0, indicamos que hay que dibujar el 
            ;rectangulo del escudo, es 1 será el del teleport.
            MOVE.L #0,D3
            MOVE.W (SHIELDTIMEEN),D5
            CMP D5,D4
            ;Si VTIMER es mayor a SHIELDTIMEEN, se pueden dar dos casos,
            ; que VTIMER sea menor al COOLDOWN, lo que significa que debemos
            ;dibujar rojo o, que VTIMER sea mayor al COOLDOWN lo que indicará
            ;que debemos dibujar verde. 
            BGT .POSIBLEROJO
            
            CMP D5,D4
            BEQ .ROJO
            BRA .VERDE
            
            
           
            
       
            
.VERDE           
            
            MOVE.L #$0000FF00,D1
            MOVE.W #81,D0        ; SET FILL COLOR
            TRAP #15
            ;Diferenciamos del TP y SHIELD
            CMP #0,D3
            BEQ .CHECKPOINT
            BRA .CHECKPOINT2

            
.POSIBLEROJO
            MOVE.L #0,D3
            MOVE.W (SHIELDCOOLDOWN),D5
            CMP D4,D5
            BGT .ROJO
            ;Si VTIMER es mayor a COOLDOWN dibujaremos VERDE.
            BRA .VERDE
            
.CHECKPOINT
            ;Dibujado del rectángulo de SHIELD
            MOVE.W #65,D1
            MOVE.W #15,D2
            MOVE.W #80,D3
            MOVE.W #30,D4
            MOVE.W #87,D0
            TRAP #15
            
            
            MOVE.L #$00FF0000,D1
            MOVE.W #81,D0        ; SET FILL COLOR
            TRAP #15
            
            
            ;VISUALIZACIÓN DEL TEXTO DEL TP
            LEA TELEPORT,A1
	
            MOVE.B #14,D0
            TRAP #15
            
            CLR.L D2
            
            
            MOVE.L #$0000FF00,D1
            MOVE.W #81,D0        ; SET FILL COLOR
            TRAP #15
            
            ;SI TIMEENABLE>=VTIMER, entonces dibujaremos ROJO
            ;sino VERDE
            MOVE.L #1,D3
            MOVE.W (VTIMER),D4
            MOVE.W (TIMEENABLE),D5
            CMP D4,D5
            BGE .ROJO
            BRA .VERDE
            
.CHECKPOINT2
            ;DIBUJADO DEL RECTANGULO
            MOVE.W #75,D1
            MOVE.W #30,D2
            MOVE.W #90,D3
            MOVE.W #45,D4
            MOVE.W #87,D0
            TRAP #15
           
           
            
            MOVE.L #$00FF0000,D1
            MOVE.W #81,D0        ; SET FILL COLOR
            TRAP #15
          
            
            

			
	MOVEM.L (A7)+,D0-D7
            RTS


.ROJO

            MOVE.L #$000000FF,D1
            MOVE.W #81,D0        ; SET FILL COLOR
            TRAP #15
            
            CMP #0,D3
            BEQ .CHECKPOINT
            BRA .CHECKPOINT2
            
; =============================================================================
KBDINIT     
; INITIALIZES THE SYSTEM VARIABLES KBDSYS AND KBDVAL
; INPUT    - NONE
; OUTPUT   - NONE
; MODIFIES - NONE
; -----------------------------------------------------------------------------
            CLR.B   (KBDVAL)                    ; INICIALIZAR VARIABLES
            CLR.B   (KBDEDGE)
             CLR (SHIELDENA)
            
           CLR (SHIELDINICIO)
            MOVE.L  #KBDREAD, ($80+KBDTRAP*4)    ; INSTALACIÓN DEL TRAP
            RTS
            
; =============================================================================
KBDREAD
; TRAP SERVICE ROUTINE.
; UPDATES THE SYSTEM VARIABLES KBDEDGE AND KBDVAL ACCORDING TO THE BITFIELD
; DESCRIBED IN SYSCONST. KBDVAL HOLDS THE CURRENT VALUES AND KBDEFGE HOLDS
; RISING EDGES FROM PREVIOUS CALL TO THE CURRENT ONE.
; INPUT    - NONE
; OUTPUT   - (KBDVAL) AND (KBDEDGE)
; MODIFIES - NONE
; -----------------------------------------------------------------------------
            MOVEM.L D0-D7,-(A7)
            MOVE.B  #19,D0
            MOVE.L  #KBDDOWN<<24+KBDRIGHT<<16+KBDUP<<8+KBDLEFT,D1
            TRAP    #15
            BSR     .STORE                  ; ALMACENAMOS LOS VALORES
            MOVE.B  (KBDVAL),D0             ; CALCULO DEL "EDGE"
            NOT.B   D0                      ; EDGE=NOT(PREVIOUS) AND CURRENT.
            AND.B   D2,D0
            MOVE.B  D2,(KBDVAL)             ; ALMACENAMOS LOS VALORES
            MOVE.B  D0,(KBDEDGE)
            
            ;MIRAMOS SI EL ESPACIO ESTÁ SIENDO PRESIONADO
            
            MOVE.B  #19,D0
            MOVE.L  #KBDSHIELD<<24,D1
            TRAP #15
            BTST #24,D1

            
            BNE .SHIELDPLOT
.RETURN
            MOVEM.L (A7)+,D0-D7
            RTE

.STORE      MOVE.W  #3,D3                   ; TRANSLATES THE 8-BIT ENCODING
.LOOP       LSL.L   #8,D1                   ; OF TRAP #15 TASK 19 INTO A 1-BIT
            ROXL.B  #1,D2                   ; PER KEYPRESS ENCODING
            DBRA.W  D3,.LOOP
            RTS
           
           



            

.SHIELDPLOT
            ;A la hora de dibujar el escudo tenemos que tener en cuenta varias cosas:
            ;1: El escudo se puede activar durante 4 segundos desde la primera activación
            ;2: Después del periodo de activación hay 4 segundos en los que no se podrá
            ;utilizar, de manera obligatoria.
            
            ;Es necesario conocer el instante del inicio de la activación para guardar
            ;datos que nos servirán posteriormente
            MOVE.W (SHIELDINICIO),D1
            CMP #0,(SHIELDINICIO)
            BEQ .DIB2 ;para obtener datos iniciales
            
            
            ;Se ha presionado espacio, pero sabemos si podemos dibujar el escudo??
            ;Si SHIELDENA es 1,entonces ya estaremos en cooldown.
            CMP #0,(SHIELDENA) 
            BEQ  .CHECKTIMER2
            
            
.CHECKLIST  
            ;Ahora que sabemos que estamos en COOLDOWN, verificamos cuando
            ;se acaba todo para reiniciar las variables necesarias         
            MOVE.W (VTIMER),D3
            MOVE.W (SHIELDCOOLDOWN),D4
            
            SUB.W (SHIELDCOOLDOWN),D3

            CMP #0,D3
            ;Se acabó el cooldown
            BGE .ENABLE            
            
            ;Seguimos en cooldown
            BRA .RETURN
.DIB2
             
          
            MOVE.W (VTIMER),D7
            ADD.W #4,D7
            MOVE.W D7,(SHIELDTIMEEN) ;Periodo en el que se acaba la activación
            ADD.W #4,D7
            MOVE.W D7,(SHIELDCOOLDOWN) ;Periodo en el que se acaba el cooldown
            
            MOVE.W #1,(SHIELDINICIO) ;Hemos empezado la activación
            
            CLR.W D7
            
            
            



.DIB

            ;Dibujado del escudo                  
            CLR.L D1
            MOVE.L #$0000FF00,D1
            MOVE.W #80,D0        ; SET PEN COLOR
            TRAP #15
            MOVE.W (PLRPOSX),D5
            MOVE.W (PLRPOSY),D6
            ADD.W #15,D5 
            ADD.W #15,D6
            
         
            SUB.W #30,D5
            MOVE.W D5,D1
  
            ADD.W #60,D5
            MOVE.W D5,D3
            
            SUB.W #30,D6
            MOVE.W D6,D2
            ADD.W #60,D6
            
            MOVE.W D6,D4
            MOVE.W #91,D0
            TRAP #15
            
            
            BRA .RETURN

.CHECKTIMER2:
            ;Miramos si estamos en el periodo de activación
            MOVE.W (SHIELDTIMEEN),D4
            MOVE.W (VTIMER),D3
            
            SUB.W D3,D4
           
            CMP  #0,D4
            BLE .FIN1
            
           
            
            ;Si no estamos en cooldown, dibujaremos el escudo.
            BRA .DIB
            

.FIN1:
            ;Estamos en cooldown         
            MOVE.W #1,(SHIELDENA)
            BRA .RETURN


.ENABLE:
            ;Reinicio de vars
            CLR.W (SHIELDINICIO)
            CLR.W (SHIELDENA)
            BRA .RETURN
                                  
; =============================================================================           
MOUSEINIT     
; INITIALIZES THE SYSTEM VARIABLES MOUSEPOSX AND MOUSEPOSY AND ENATP AND LASTENA
; INPUT    - NONE
; OUTPUT   - NONE
; MODIFIES - NONE
; -----------------------------------------------------------------------------
            CLR.W   (MOUSEPOSX)                    ;INICIALIZAR VARS
            CLR.W   (MOUSEPOSY)
            CLR.W   (ENATP)
            CLR.W   (LASTENA)
	
            MOVE.L  #REVISION, ($80+MOUSETRAP*4)    ; INSTALAR
            RTS
            
; =============================================================================            
REVISION:
; TRAP SERVICE ROUTINE HANDLES THE USE OF THE MOUSE.
; INPUT    - NONE
; OUTPUT   - NONE
; MODIFIES - NONE
; -----------------------------------------------------------------------------
            MOVEM.L D0-D7,-(A7)

	; LECTURA DEL ESTADO ACTUAL DEL RATÓN
	MOVE.B  #61,D0
	MOVE.B  #00,D1
	
	TRAP    #15
            MOVE.W  D1,(MOUSEPOSX)
            

            
	

            SWAP    D1            ; Intercambia los 16 bits superiores e inferiores de D1

            MOVE.W  D1,(MOUSEPOSY)
            SWAP    D1
	

	; DETECTAMOS SI SE HA PRESIONADO CLICK
	BTST    #0,D0
	
            BEQ     NOT_PRESSED
            ;SE HA PRESIONADO, PERO ESTAMOS EN COOLDOWN???
            ;Si ENATP es 0, haremos el TP
            CMP.W   #0,(ENATP)
            BNE CHECKTIMER

PRESSED:

            ;Guardamos la ultima activación
             MOVE.W (VTIMER),(LASTENA)
             MOVE.W (VTIMER),D2
             ADD.W #6,D2
             ;Guardamos cuando podras activarlo de nuevo
             MOVE.W D2,(TIMEENABLE)
             ;Se realiza el TP
            MOVE.W (MOUSEPOSX),(PLRPOSX)
            MOVE.W (MOUSEPOSY),(PLRPOSY)
            ;Estamos en cooldown
            MOVE.W #1,(ENATP)
            
            BRA NOT_PRESSED
            

CHECKTIMER:
            ;Verificacion de si salimos del periodo de cooldown
            MOVE.W (TIMEENABLE),D4
            MOVE.W (VTIMER),D3
            
            CMP D3,D4
            BGE NOT_PRESSED
            CLR.W (ENATP)
            
         
NOT_PRESSED:          
	MOVEM.L  (A7)+,D0-D7
	RTE
	
           
; -----------------------------------------------------------------------------
SCRUPD
; TRAP SERVICE ROUTINE IN CHARGE OF VISUALIZING CURRENT FRAME AND CLEARING
; BUFFER FOR THE NEXT ONE AND VISUALIZE THE FPS.
; INPUT    - (FPSAN)
; OUTPUT   - NONE
; MODIFIES - NONE
; -----------------------------------------------------------------------------
            MOVEM.W D0-D6/A1,-(A7)
            CLR.L D0
            CLR.L D1
            MOVE.B  #94,D0                  ; ACTUALIZA DOUBLE BUFFER
            TRAP    #15
            MOVE.W  #$FF00,D1               ; LIMPIA EL BUFFER ESCONDIDO
            MOVE.B  #11,D0
            TRAP    #15
            MOVEM.W (A7)+,D0-D6/A1
            RTE
            
; -----------------------------------------------------------------------------
FILLOAD
; LOADS AN FILE FROM DISK TO MEMORY
; INPUT    : A0 - DESTINATION ADDR. WITH ENOUGH SPACE AVAILABLE.
;            A1 - POINTER TO FILE NAME
; OUTPUT   : NONE
; MODIFIES : NONE
; NOTE     : THE READ DATA LOOP READS 1 BYTE AT A TIME TO ENSURE ALL FILES CAN
;            BE LOADED. HOWEVER, IF THE FILE SIZE IS KNOWN BEFOREHAND, NO LOOP
;            WOULD BE NECESSARY AND A SINGLE CALL TO TRAP #15 TASK 53 WOULD READ
;            THE WHOLE FILE AS LONG AS THE FILE SIZE IS PLACED INTO D2.L.
; NOTE     : THE ZERO-TERMINATOR (JUST BEFORE CLOSE THE FILE) IS A REQUIREMENT
;            FOR TEXT-FILES IF THE TEXT IS TO BE PRINTED. FOR OTHER USES, IT
;            CAN BE REMOVED.
; ------------------------------------------------------------------------------
            MOVEM.L D0-D2/A1,-(A7)          ; SAVE REGISTERS
            ; CLOSE ALL FILES
            MOVE.B  #50,D0
            TRAP    #15
            ; OPEN FILE (ASSUMES A1 POINTS TO FILENAME)
            MOVE.B  #51,D0
            TRAP    #15                     ; D1.L:=FILE HANDLER
            ; READ DATA
            MOVE.L  A0,A1                   ; A1 MUST BE THE BUFFER ADDRESS
            MOVE.B  #53,D0                  ; READ DATA TASK
            MOVE.L  #4096,D2                   ; READ ALL THE FILE
            TRAP    #15
                               ; PUT A ZERO-TERMINATOR.
            ; CLOSE THE FILE
            MOVE.B  #56,D0
            TRAP    #15
            MOVEM.L (A7)+,D0-D2/A1          ; RESTORE REGISTERS
            RTS

; -----------------------------------------------------------------------------
WIN:
; DETECT THE WINNING CONDITION AND DRAW A MESSAGE WITH A PICTURE
; INPUT    : NONE
; OUTPUT   : NONE
; MODIFIES : NONE
; ------------------------------------------------------------------------------
            MOVEM.L D0-D6,-(A7)
            MOVEM.L A0-A6,-(A7)
            
            CMP #80,(VTIMER)
            BNE RETURN
            ;EL JUEGO HA TERMINADO
            ;DESHABILITAMOS DOUBLE BUFFER Y BORRAMOS PANTALLA
            MOVE.W #16,D1
            MOVE.W #92,D0
            TRAP #15
            
            ;VACIAMOS PANTALLA
            MOVE.W #11,D0
            MOVE.W #$FF00,D1
            TRAP #15
            
            
            MOVE.L #$00000000,D1
            MOVE.W #81,D0        ; SET FILL COLOR
            TRAP #15
            
            			
	MOVE.L #$000000FF,D1
            MOVE.W #80,D0        ; SET PEN COLOR
            TRAP #15
            
            ;ESCRIBIMOS LOS MENSAJES CORRESPONDIENTES  
            LEA WIN1,A1
	
            MOVE.B #14,D0
            TRAP #15
            
            LEA WIN2,A1
	
            MOVE.B #14,D0
            TRAP #15

            
.DIBUJAR    ;DIBUJAR IMAGEN FINAL
            MOVE.W #1024,D4
            SUBQ.W #1,D4
            LEA WINPIX,A0
            LEA FILENAMEWIN,A1
            JSR FILLOAD
            
            LEA X1,A3
            LEA Y1,A4
           
LOOPDIB1:
            CLR.L D1
            CLR.L D2
            MOVE.B #80,D0
            MOVE.L (A0)+,D1
            TRAP #15 ;PEN COLOR
            MOVE.B #82,D0
            CLR.L D1
            CLR.L D2
            MOVE.W (A3)+,D1
            MOVE.W (A4)+,D2
            ADD.L #DSPLZ1,D1
            ADD.L #DSPLZ2,D2
            
            TRAP #15 ;Dibujar pixel
            DBRA D4,LOOPDIB1
           
            
            MOVEM.L (A7)+,A0-A6
            MOVEM.L (A7)+,D0-D6
            SIMHALT

            


            
RETURN
            MOVEM.L (A7)+,A0-A6
            MOVEM.L (A7)+,D0-D6
            RTS
           












*~Font name~Courier New~
*~Font size~10~
*~Tab type~0~
*~Tab size~4~
