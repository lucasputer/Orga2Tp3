; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
; definicion de rutinas de atencion de interrupciones

%include "imprimir.mac"

BITS 32

sched_tarea_offset:     dd 0x00
sched_tarea_selector:   dw 0x00

%define lanzar_jugadorA 0xAA
%define lanzar_jugadorB 0xB6

%define tecla_debugg 0x95

;; PIC
extern fin_intr_pic1

;; Sched
extern sched_tick
extern sched_tarea_actual

extern screen_pintar_error
extern screen_pintar_interrupcion
extern screen_pintar_tecla
extern sched_tick
extern sched_jugador_actual
extern game_tick
extern game_jugador_lanzar_pirata
extern game_syscall_pirata_mover
extern sched_modo_debugg
extern cambiar_modo_debugg
extern sched_juego_pausado
extern sched_pausar_juego
extern sched_despausar_juego
extern tss_matar_tarea
;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;

%macro ISR 1
global _isr%1

_isr%1:
    mov eax, %1
    mov ebx, eax

    call sched_modo_debugg
    cmp eax, 0
    je .noPausar
    
    call sched_pausar_juego

    .noPausar:
    push ebx
    call screen_pintar_interrupcion
    add esp, 4

    call tss_matar_tarea
    
    .fin: 
    jmp 0x0070:0
    jmp $


%endmacro

%macro ISR_CON_ERROR 1
global _isr%1

_isr%1:
    mov eax, %1
    mov ebx, eax

    call sched_modo_debugg
    cmp eax, 0
    je .noPausar
    
    call sched_pausar_juego

    .noPausar:
    push ebx
    call screen_pintar_error
    add esp, 4

    call tss_matar_tarea
    
    .fin: 
    jmp 0x0070:0
    jmp $

%endmacro

;;
;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;
global _isr32
_isr32:
    .start:
    pushad
    call fin_intr_pic1

    ;si el juego esta pausado, ignora la interrupcion
    call sched_juego_pausado
    cmp eax, 1
    je .fin_isr32 

    call sched_tick
    str cx
    cmp ax, cx
    je .fin_isr32

    mov [sched_tarea_selector], ax
    jmp far [sched_tarea_offset]

    .fin_isr32:

    popad
    iret
;;
;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;
global _isr33

_isr33:
    pushad
    call fin_intr_pic1

    xor edx, edx
    xor eax, eax
    in al, 0x60

    mov dl, al

    cmp edx, tecla_debugg
    je .mode_debugg

    ;si la tecla no fue tecla_debugg y el juego esta pausado, ignora la interrupcion
    call sched_juego_pausado
    cmp eax, 1
    je .fin_teclado 

    cmp edx, lanzar_jugadorA
    mov ecx, 0
    je .lanzar_explorador

    cmp edx, lanzar_jugadorB
    mov ecx, 1
    je .lanzar_explorador

    jmp .fin_teclado

    .lanzar_explorador:
    xor ebx, ebx
    push ebx
    push ecx
    call game_jugador_lanzar_pirata
    
    add esp, 8

    jmp .fin_teclado

    .mode_debugg:
    
    call sched_juego_pausado
    cmp eax, 1
    je .despausar

    call cambiar_modo_debugg
    jmp .fin_teclado

    .despausar:
    call sched_despausar_juego
    jmp .fin_teclado

    .fin_teclado:
    popad
    iret

;;
;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;
global _isr70

_isr70:
    pushad

    cmp eax, 1
    je .moverse
    cmp eax, 2
    je .cavar
    cmp eax, 3
    je .posicion

    .moverse:
        call sched_jugador_actual
        push ecx ; para enviar a la funcion
        push eax ; para enviar a la funcion
        call game_syscall_pirata_mover
        add esp, 8
    .cavar:
    .posicion:

    jmp 0x0070:0
    popad  
    iret


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



