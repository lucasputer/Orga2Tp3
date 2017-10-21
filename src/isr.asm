; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
; definicion de rutinas de atencion de interrupciones

%include "imprimir.mac"

BITS 32

sched_tarea_offset:     dd 0x00
sched_tarea_selector:   dw 0x00

;; PIC
extern fin_intr_pic1

;; Sched
extern sched_tick
extern sched_tarea_actual

extern screen_pintar_error
extern screen_pintar_interrupcion

;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;

%macro ISR 1
global _isr%1

_isr%1:
    mov eax, %1
    push eax
    call screen_pintar_interrupcion
    ;call imprimir mensaje con la interrupcion
    jmp $

%endmacro

%macro ISR_CON_ERROR 1
global _isr%1

_isr%1:
    mov eax, %1
    push eax
    call screen_pintar_error
    ;call imprimir mensaje con la interrupcion
    ;recuperar la esp el error_core
    jmp $

%endmacro
;;
;; Datos
;; -------------------------------------------------------------------------- ;;
; Scheduler

;;
;; Rutina de atención de las EXCEPCIONES
;; -------------------------------------------------------------------------- ;;
ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR_CON_ERROR 8
ISR 9
ISR_CON_ERROR 10
ISR_CON_ERROR 11
ISR_CON_ERROR 12
ISR_CON_ERROR 13
ISR_CON_ERROR 14
ISR 15
ISR 16
ISR_CON_ERROR 17
ISR 18
ISR 19


;;
;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;

;;
;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;

;;
;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;


