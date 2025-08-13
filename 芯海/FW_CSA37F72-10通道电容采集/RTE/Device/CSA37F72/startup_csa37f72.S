;/*---------------------------------------------------------------------------------------------------------*/
;/*                                                                                                         */
;/* Copyright(c) 2020 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD. All rights reserved.                           */
;/*                                                                                                         */
;/*---------------------------------------------------------------------------------------------------------*/

; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000200

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB

; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size
                
__Vectors       DCD     __initial_sp              ; Top of Stack        0
                DCD     Reset_Handler             ; Reset Handler       4
                DCD     NMI_Handler               ; NMI Handler         8
                DCD     HardFault_Handler         ; Hard Fault Handler  C
                DCD     0                         ; Reserved            10
                DCD     0                         ; Reserved            14
                DCD     0                         ; Reserved            18
                DCD     0                         ; Reserved            1C
                DCD     0                         ; Reserved            20
                DCD     0                         ; Reserved            24
                DCD     0                         ; Reserved            28
                DCD     SVC_Handler               ; SVCall Handler      2C
                DCD     0                         ; Reserved            30
                DCD     0                         ; Reserved            34
                DCD     PendSV_Handler            ; PendSV Handler      38
                DCD     SysTick_Handler           ; SysTick Handler     3C

                ; External Interrupts : maximum of 32 External Interrupts are possible
                DCD     PVD_IRQHandler            ; Programable VCC detect Interrupt
                DCD     WWDT_IRQHandler           ; Window WatchDog Interrupt
                DCD     TIM0_IRQHandler           ; TIM0 Interrupt
                DCD     TIM1_IRQHandler			  ; TIM1 Interrupt
                DCD     0 		                  ; Reserved
                DCD     I2C_IRQHandler			  ; I2C Interrupt
                DCD     0 			              ; Reserved
                DCD     GPIOA_IRQHandler 		  ; GPIOA Interrupt
                DCD     WKUP_IRQHandler 		  ; Wake up Interrupt
                DCD     0			              ; Reserved
                DCD     USART_IRQHandler		  ; USART Interrupt
                DCD     FAULT_IRQHandler 		  ; RAM Parity Check FAULT Interrupt
                DCD     GPIOB_IRQHandler 		  ; GPIOA Interrupt
                DCD     0    		              ; Reserved
                DCD     AFE2_IRQHandler    		  ; AFE2 Interrupt

__Vectors_End

__Vectors_Size  EQU  __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY
          
; Reset handler routine
Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  __main                
                IMPORT  SystemInit
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP
                
                
; Dummy Exception Handlers (infinite loops which can be modified)                
                
NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP

Default_Handler PROC

                EXPORT  PVD_IRQHandler            [WEAK]
                EXPORT  WWDT_IRQHandler           [WEAK]
                EXPORT  TIM0_IRQHandler           [WEAK]
                EXPORT  TIM1_IRQHandler           [WEAK]
                EXPORT  I2C_IRQHandler            [WEAK]             
                EXPORT  GPIOA_IRQHandler          [WEAK]
                EXPORT  WKUP_IRQHandler           [WEAK]
                EXPORT  USART_IRQHandler          [WEAK]
                EXPORT  FAULT_IRQHandler          [WEAK]
                EXPORT  GPIOB_IRQHandler          [WEAK]
                EXPORT  AFE2_IRQHandler           [WEAK]                  
               
PVD_IRQHandler
WWDT_IRQHandler
TIM0_IRQHandler
TIM1_IRQHandler
I2C_IRQHandler
GPIOA_IRQHandler
WKUP_IRQHandler
USART_IRQHandler
FAULT_IRQHandler
GPIOB_IRQHandler
AFE2_IRQHandler

                B       .
                
                ENDP

                ALIGN

;*******************************************************************************
; User Stack and Heap initialization
;*******************************************************************************
                IF      :DEF:__MICROLIB
                
                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit
                
                ELSE
                
                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
__user_initial_stackheap

                LDR     R0, =  Heap_Mem
                LDR     R1, = (Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR

                ALIGN

                ENDIF

    
                END


