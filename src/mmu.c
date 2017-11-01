/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#include "mmu.h"
#include "i386.h"
/* Atributos paginas */
int primera_pagina_libre;

/* -------------------------------------------------------------------------- */

/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<i>_CODE_ADDR.
 */

/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */

void mmu_inicializar() {
	// primera_pagina_libre = 0x100000;
	primera_pagina_libre = 0x100000;
}

int dame_libre() {
	int resultado = primera_pagina_libre;
	primera_pagina_libre += 0x1000;
	return resultado;
}

void mmu_mapear_pagina(unsigned int virtual, unsigned int cr3, unsigned int fisica, int read_write, int user_supervisor){

	// desarmar la virtual en sus indices

	unsigned int pde_i = virtual >> 22;
	unsigned int pte_i = (virtual << 10) >> 22;
	pde* directorio = (pde*) cr3;
	if(directorio[pde_i].present == 0) {
		mmu_inicializar_directorio(pde_i, cr3, read_write, user_supervisor);
	}
	pte* tablas = (pte*) (directorio[pde_i].address << 12);
	mmu_inicializar_tabla(pde_i,pte_i, cr3, read_write, user_supervisor);
	tablas[pte_i].address = fisica >> 12;
	// attr

	tlbflush();
}

void mmu_unmapear_pagina(unsigned int virtual, unsigned int cr3){
	unsigned int pde_i = virtual >> 22;
	unsigned int pte_i = (virtual << 10) >> 22;
	pde* directorio = (pde*) cr3;
	pte* tablas = (pte*) (directorio[pde_i].address << 12);
	tablas[pte_i].present = 0;
	tlbflush();
}

void mmu_inicializar_directorio(int indice, int cr3, int read_write, int user_supervisor) {
	unsigned int pde_i = indice;
	pte* tabla = (pte*) dame_libre();
	//pte* tabla = (pte*) KP_ADDRESS;
	pde* directorio = (pde*) cr3;
	directorio[pde_i].present = 1;
	directorio[pde_i].read_write = read_write;
	directorio[pde_i].user_supervisor = user_supervisor;
	directorio[pde_i].address = ((int) tabla) >> 12;
	directorio[pde_i].accesed = 0;
	directorio[pde_i].ignored1 = 0;
	directorio[pde_i].page_size = 0;
	directorio[pde_i].ignored2 = 0;
	directorio[pde_i].page_cache_disable = 0;
	directorio[pde_i].page_write_through = 0;
	directorio[pde_i].global =0;
	for (int i = 0; i < 1024; i++) {
		tabla[i].present = 0;
	}	
}

void mmu_inicializar_tabla(int pde_i,int pte_i, int cr3, int read_write, int user_supervisor){
	pde* directorio = (pde*) cr3;
	pte* tabla = (pte*) (directorio[pde_i].address << 12);
	tabla[pte_i].present = 1;
	tabla[pte_i].read_write = read_write;
	tabla[pte_i].user_supervisor = user_supervisor;
	tabla[pte_i].accesed = 0;
	tabla[pte_i].dirty = 0;
	tabla[pte_i].page_cache_disable = 0;
	tabla[pte_i].page_write_through = 0;
	tabla[pte_i].pat = 0;
	tabla[pte_i].global =0;
	tabla[pte_i].ignored = 0;
}

void mmu_inicializar_dir_kernel() {

	pde* dir_kernel = ((pde*) PD_ADDRESS);
	pte* tabla0_kernel = ((pte*) KP_ADDRESS);
	pte* tabla1_kernel = ((pte*) (KP_ADDRESS + 0x1000));
	pte* tabla2_kernel = (pte*) (KP_ADDRESS + 0x2000);
	pte* tabla3_kernel = (pte*) (KP_ADDRESS + 0x3000);
	for (int i = 0; i < 1024; i++) {
		dir_kernel[i].present = 0;
		tabla0_kernel[i].present = 0;
		tabla1_kernel[i].present = 0;
		tabla2_kernel[i].present = 0;
		tabla3_kernel[i].present = 0;
	}
	
	
	int address = KP_ADDRESS;
	for(int i = 0; i < 4; i++){
		mmu_inicializar_directorio(i,PD_ADDRESS,1,0);
		dir_kernel[i].address = address >> 12;
		mmu_inicializar_tabla(0,i,PD_ADDRESS,1,0);
		tlbflush();
		address += 0x1000;
	}

	for (int i = 0; i <= MEMORY_LIMIT; i += 0x1000) {
		mmu_mapear_pagina(i, PD_ADDRESS, i, 1, 0);
	}
}

int mmu_inicializar_dir_pirata(int x, int y, int posicion_codigo){
	int nuevo_cr3 = dame_libre();
	
	for (int i = 0; i <= MEMORY_LIMIT; i += 0x1000) {
		mmu_mapear_pagina(i, nuevo_cr3, i, 1, 0);
	}

	int posicion_en_mapa = 0x500000 + (y * MAPA_ANCHO) + x;

	unsigned int actual_cr3 = rcr3();


	mmu_mapear_pagina(posicion_en_mapa, actual_cr3, posicion_en_mapa, 0, 0);

	for(int i = 0; i < 0x400; i += 1){
		((int*)posicion_en_mapa)[i] = ((int*)posicion_codigo)[i];
	}

	mmu_mapear_pagina(0x400000, nuevo_cr3, posicion_en_mapa, 0, 3);

	return nuevo_cr3;
}