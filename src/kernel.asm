; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================

%include "imprimir.mac"

extern GDT_DESC
extern screen_inicializar
extern IDT_DESC
extern idt_inicializar
extern resetear_pic
extern habilitar_pic
extern mmu_inicializar_dir_kernel
extern mmu_inicializar
extern tss_inicializar
extern tss_inicializar_idle
extern mmu_inicializar_dir_pirata

;UTILS TIENE LOS DEFINES DE VARIAS COSAS..ANDA A MIRAR
%include "utils.asm"


global start


;; Saltear seccion de datos
jmp start

;;
;; Seccion de datos.
;; -------------------------------------------------------------------------- ;;
iniciando_mr_msg db     'Iniciando kernel (Modo Real)...'
iniciando_mr_len equ    $ - iniciando_mr_msg

iniciando_mp_msg db     'Iniciando kernel (Modo Protegido)...'
iniciando_mp_len equ    $ - iniciando_mp_msg

;;
;; Seccion de código.
;; -------------------------------------------------------------------------- ;;

;; Punto de entrada del kernel.
BITS 16
start:
    ; Deshabilitar interrupciones
    cli

    ; Cambiar modo de video a 80 X 50
    mov ax, 0003h
    int 10h ; set mode 03h
    xor bx, bx
    mov ax, 1112h
    int 10h ; load 8x8 font

    ; Imprimir mensaje de bienvenida
    imprimir_texto_mr iniciando_mr_msg, iniciando_mr_len, 0x07, 0, 0


    ; Habilitar A20
    call habilitar_A20
    ;xchg bx, bx
    ; Cargar la GDT
    lgdt [GDT_DESC]
    ;xchg bx, bx
    ; Setear el bit PE del registro CR0
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    ; Saltar a modo protegido
    jmp CODE_0:modoprotegido
    
    BITS 32
    
    modoprotegido:
    
    ; Establecer selectores de segmentos
    ;xchg bx, bx
    xor eax, eax
    mov ax, DATA_0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov gs, ax
    mov ax, VIDEO
    mov fs, ax
    ; Establecer la base de la pila
    mov esp, 0x27000
    mov ebp, 0x27000
    ; Imprimir mensaje de bienvenida
    ;xchg bx, bx
    imprimir_texto_mp iniciando_mp_msg, iniciando_mp_len, 0x07, 0, 0
    ; Inicializar el juego
    
    ; Inicializar pantalla
    call screen_inicializar

    call idt_inicializar
    ;xchg bx, bx
    ; Cargar IDT
    lidt [IDT_DESC]

    ; Inicializar el manejador de memoria
    call mmu_inicializar

    ; Inicializar el directorio de paginas
    call mmu_inicializar_dir_kernel

    ; Cargar directorio de paginas

    ; Habilitar paginacion
    mov eax, 0x27000 ;page_directory == 0x27000
    mov cr3, eax

    mov eax, cr0
    or eax, 0x80000000 ; habilito paginacion
    mov cr0, eax
    ; Inicializar tss
    call tss_inicializar
    ; Inicializar tss de la tarea Idle
    call tss_inicializar_idle
    ; Inicializar el scheduler

    ; Inicializar la IDT
    ;xchg bx, bx
    call idt_inicializar
    ; ;xchg bx, bx
    ; ; Cargar IDT
    lidt [IDT_DESC]
    ; Configurar controlador de interrupciones
    
    ;DIVIDO POR CERO:
    ;mov al, 2
    ;mov cl, 0
    ;div cl
    
    
    call resetear_pic
    ;xchg bx, bx
    call habilitar_pic
    ;xchg bx, bx
    ;xchg bx, bx
    ; Cargar tarea inicial
    mov ax, 0x0068
    ltr ax
    ; Habilitar interrupciones
    sti 

    xchg bx, bx
    push 0x10000
    push 0x00
    push 0x00
    
    call mmu_inicializar_dir_pirata
    xchg bx, bx
    mov cr3, eax

    ; Saltar a la primera tarea: Idle
    jmp 0x0070:0
    ; Ciclar infinitamente (por si algo sale mal...)
    mov eax, 0xFFFF
    mov ebx, 0xFFFF
    mov ecx, 0xFFFF
    mov edx, 0xFFFF
    jmp $
    jmp $

;; -------------------------------------------------------------------------- ;;

%include "a20.asm"