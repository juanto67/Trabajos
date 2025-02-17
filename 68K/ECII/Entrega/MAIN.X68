*-----------------------------------------------------------
* Title      :
* Written by :
* Date       :
* Description:
*-----------------------------------------------------------
    ORG    $1000
             INCLUDE "CONSTS.X68"
            INCLUDE "VARS.X68"
             INCLUDE "SYS.X68"
             INCLUDE "PLY.X68"
             INCLUDE "BALL.X68"
            INCLUDE "DMMCONST.X68"
            INCLUDE "DMMVARS.X68"
            
            INCLUDE "DMMCODE.X68"
            INCLUDE "UTLCODE.X68"
            INCLUDE "AGL.X68"
            INCLUDE "PLATAFORM.X68"
START:                  ; first instruction of program
           CLR.L VTIMER
           JSR DMMINIT
           JSR UTLINIT
           JSR SCRINIT
           JSR KBDINIT
           
           MOVE.W #2,(SEGUNDOS)
           JSR PLRINIT
           JSR MOUSEINIT
           LEA BALINIT,A1
           LEA BALUPD, A2
           LEA BALPLOT,A3
           
           MOVE.W #1,D0
           JSR  AGLADD
           
           LEA PINIT,A1
           LEA PUPDATE, A2
           LEA PPLOT,A3
           MOVE.W #2,D0
           JSR      AGLADD
           
         

           MOVE.W #0,(PLTEN)
           MOVE.W #0,(PLTEN1)
           MOVE.W #0,(BALEN)
            CLR.L D2
            MOVE.W #8,D0
	TRAP #15
	MOVE.L D1,(FPSIN)
            CLR.L D1
            CLR.L D0
.LOOP       CMP #0,(ENFPS)
            BEQ     .TT
            MOVE.W #8,D0
	TRAP #15
	MOVE.L D1,(FPSIN)
            CLR.L D1
            CLR.L D0
            MOVE.W  #0,(ENFPS)
.TT         
            
            JSR     SCRPLOT
            TRAP   #MOUSETRAP
	TRAP   #KBDTRAP                ; READ KEYBOARD
	
	
	
	
	
	CMP #15,(VTIMER)
            BNE     .II
            CMP     #0,(PLTEN)
            BNE     .II
            ADDQ #1,(PLTEN)
            LEA PINIT1,A1
            LEA PUPDATE, A2
            LEA PPLOT,A3
            MOVE.W #2,D0
            JSR      AGLADD
            
         

.II

            CMP #30,(VTIMER)
            BNE .III
            CMP #0,(PLTEN1)
            BNE .III
            ADDQ #1,(PLTEN1)
            LEA PINIT2,A1
            LEA PUPDATE1, A2
            LEA PPLOT,A3
            MOVE.W #2,D0
            JSR      AGLADD
            
            
.III
            JSR WIN
            
            

.IV         
             MOVE.W (SEGUNDOS),D4
            MOVE.W  (VTIMER),D5
            CMP     D4,D5
            BNE     .WINT            
            MOVE.W  (BALEN),D6
            CMP     D4,D6
            BEQ     .WINT
            MOVE.W  (SEGUNDOS),(BALEN)
            LEA BALINIT,A1
            LEA BALUPD, A2
            LEA BALPLOT,A3
            MOVE.W #1,D0
            JSR      AGLADD

            
            
                        
            
            ADD.W   #2,(SEGUNDOS)
           
           
           
            
.WINT 
	
            JSR     PLRUPD
            JSR     AGLUPD
            
            JSR     AGLPLOT
            JSR     DIBUJAR
            
            
             TST.B   (SCRINTCT)              ; WAIT FOR INTERRUPT
            BEQ     .WINT
            CLR.B   (SCRINTCT)
            MOVE.W #8,D0
	TRAP #15
	MOVE.L D1,(FPSAC)
            CLR.L D1
            CLR.L D0
            CLR.L D6
            MOVE.B (SCRCYCCT),D6
            CMP #FPS,D6
            BLT .YA
            
            JSR FPSUPD
            MOVE.W  #1,(ENFPS)
            ADDQ.W #1,(VTIMER)
            CLR.B   (SCRCYCCT)
.YA           
            TRAP    #SCRTRAP                ; SHOW GRAPHICS                
            
            BRA .LOOP
    SIMHALT             ; halt simulator

           

    END    START        ; last line of source






















*~Font name~Courier New~
*~Font size~10~
*~Tab type~0~
*~Tab size~4~
