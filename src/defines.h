/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

    Definiciones globales del sistema.
*/

#ifndef __DEFINES_H__
#define __DEFINES_H__

/* Tipos basicos */
/* -------------------------------------------------------------------------- */
#define NULL                    0
#define TRUE                    0x00000001
#define FALSE                   0x00000000
#define ERROR                   1

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;

/* Constantes basicas */
/* -------------------------------------------------------------------------- */
#define PAGE_SIZE               0x00001000
#define TASK_SIZE               4096

#define BOOTSECTOR              0x00001000 /* direccion fisica de comienzo del bootsector (copiado) */
#define KERNEL                  0x00001200 /* direccion fisica de comienzo del kernel */

/* Indices en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_COUNT 30

#define GDT_IDX_NULL_DESC           0
#define GDT_IDX_CODE_0              8 // 0x0040
#define GDT_IDX_CODE_3              9 // 0x0048 
#define GDT_IDX_DATA_0              10 // 0x0050 == 0101 0000
#define GDT_IDX_DATA_3              11 // 1011 << 3 == 0101 1000 ==> 0x0058
#define GDT_IDX_VIDEO               12 // 0110 0000  == 0x0060 
#define GDT_IDX_TSS_INIT			13	//01101000 = 0110 1000 = 0X0068
#define GDT_IDX_TSS_IDLE			14  // 01110000 = 0111 0000 = 0X0070        
#define GDT_IDX_TSS_PRIMER_TAREA_JUGADOR_A      15  //1111 000 =  0111 1000 = 0X0078
#define GDT_IDX_TSS_PRIMER_TAREA_JUGADOR_B      23  // 1011 1000 = 0X00B8
#define GDT_IDX_TSS_EXPLORADOR		15


#define SCHED_SIN_TAREAS 1001

/* Offsets en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_OFF_NULL_DESC           (GDT_IDX_NULL_DESC      << 3)

/* Selectores de segmentos */
/* -------------------------------------------------------------------------- */

void* error();

#define ASSERT(x) while(!(x)) {};


#endif  /* !__DEFINES_H__ */
