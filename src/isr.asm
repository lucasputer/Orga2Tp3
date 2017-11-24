; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
; definicion de rutinas de atencion de interrupciones

%include "imprimir.mac"

BITS 32

sched_tarea_offset:     dd 0x00
sched_tarea_selector:   dw 0x00

lanzar_jugadorA: dw 0xAA
lanzar_jugadorB: dw 0xB6

;; PIC
extern fin_intr_pic1

;; Sched
extern sched_tick
extern sched_tarea_actual

extern screen_pintar_error
extern screen_pintar_interrupcion
extern screen_pintar_tecla
extern sched_tick
extern game_tick

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


%macro ISR_TECLADO 1
global _isr%1

_isr%1:
    pusha
    call fin_intr_pic1
    xor eax, eax
    in al, 0x60

    cmp eax, lanzar_jugadorA
    je .lanzar_exploradorA

    cmp eax, lanzar_jugadorB
    je .lanzar_exploradorB

    push eax
    call screen_pintar_tecla
    pop eax
    jmp .fin_teclado

    .lanzar_exploradorA:
    jmp .fin_teclado

    .lanzar_exploradorB:
    jmp .fin_teclado

    .fin_teclado:
    popa
    iret

%endmacro

%macro ISR_SOFTWARE 1
global _isr%1

_isr%1:
    pusha
    call fin_intr_pic1
    ;call screen_pintar_tecla
    popa
    cmp eax, 1
    je .moverse
    cmp eax, 2
    je .cavar
    cmp eax, 3
    je .posicion

    .moverse:
    .cavar:
    .posicion:
        
    iret

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
ISR_TECLADO 33
ISR_SOFTWARE 70

;;
;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;
global _isr32
_isr32:
    pusha
    call fin_intr_pic1
    ;mov eax, 32
    ;xchg bx, bx
    ;call screen_pintar_interrupcion
    
    ; call sched_tick
    call game_tick

    ; str cx
    ; cmp ax, cx
    ; je .fin_isr32
    ; mov [sched_tarea_selector], ax
    ; jmp far [sched_tarea_offset]

    .fin_isr32:

    popa
    iret
;;
;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;

;;
;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;


