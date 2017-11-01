/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#ifndef __MMU_H__
#define __MMU_H__

#include "defines.h"
#include "game.h"

#define CODIGO_BASE       0X400000

#define MAPA_BASE_FISICA  0x500000
#define MAPA_BASE_VIRTUAL 0x800000

#define MEMORY_LIMIT 0x3FFFFF
#define PD_ADDRESS 0x27000
#define KP_ADDRESS 0x28000

typedef struct pde_s {
    int present:1;
    int read_write:1;
    int user_supervisor:1;
    int page_write_through:1;
    int page_cache_disable:1;
    int accesed:1;
    int ignored1:1;
    int page_size:1;
    int global:1;
    int ignored2:3;
    int address:20;
} pde;

typedef struct pte_s {
    int present:1;
    int read_write:1;
    int user_supervisor:1;
    int page_write_through:1;
    int page_cache_disable:1;
    int accesed:1;
    int dirty:1;
    int pat:1;
    int global:1;
    int ignored:3;
    int address:20;
} pte;

void mmu_inicializar();
void mmu_inicializar_dir_kernel();
int mmu_inicializar_dir_pirata(int x, int y, int posicion_codigo);
int dame_libre();
void mmu_inicializar_directorio(int indice, int cr3, int read_write, int user_supervisor);
void mmu_inicializar_tabla(int pde_i, int pte_i, int cr3, int read_write, int user_supervisor);
void mmu_mapear_pagina(unsigned int virtual, unsigned int cr3, unsigned int fisica, int read_write, int user_supervisor);
void mmu_unmapear_pagina(unsigned int virtual, unsigned int cr3);

#endif	/* !__MMU_H__ */
