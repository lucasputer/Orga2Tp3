/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#include "mmu.h"
#include "i386.h"
#include "screen.h"
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
		mmu_inicializar_directorio(pde_i, cr3, read_write, user_supervisor, dame_libre());
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

void mmu_inicializar_directorio(int indice, int cr3, int read_write, int user_supervisor, int dir_fisica_pte) {
	unsigned int pde_i = indice;
	pte* tabla = (pte*) dir_fisica_pte;
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
	int i = 0;
	for (i = 0; i < 1024; i++) {
		dir_kernel[i].present = 0;
		tabla0_kernel[i].present = 0;
		tabla1_kernel[i].present = 0;
		tabla2_kernel[i].present = 0;
		tabla3_kernel[i].present = 0;
	}
	
	
	int address = KP_ADDRESS;
	for(i = 0; i < 4; i++){
		mmu_inicializar_directorio(i,PD_ADDRESS,1,0, dame_libre());
		dir_kernel[i].address = address >> 12;
		mmu_inicializar_tabla(0,i,PD_ADDRESS,1,0);
		tlbflush();
		address += 0x1000;
	}

	for (i = 0; i <= MEMORY_LIMIT; i += 0x1000) {
		mmu_mapear_pagina(i, PD_ADDRESS, i, 1, 0);
	}
}

int mmu_inicializar_dir_pirata(int x, int y, int posicion_codigo, int tipo, int * dir_fisicas_pte_jugador){
	int nuevo_cr3 = dame_libre();
	
	int i = 0;
	int j = 0;

	static char primera_vez_A = 1;
	static char primera_vez_B = 1;
	//recorremos el dir_fisicas_pte_jugador
	//para cada dir, inicializamos una entrada en el pde para que el pte quede apuntando a esa fisica.
	//Al hacerlo asi(con las fisicas hardcodeadas) todas las tareas ven lo mismo.
	for(i = 0; i < FISICAS_PTE_JUGADOR; i++) {
		int virtual = MAPA_BASE_VIRTUAL + i*PDE_MAPPING_SIZE;
		int indice = virtual >> 22;
		pde* pd = ((pde*) nuevo_cr3);
		if(pd[indice].present == 0) {
			mmu_inicializar_directorio(indice, nuevo_cr3, 1, 1, dir_fisicas_pte_jugador[i]);
			pte* pt = ((pte*) (pd[indice].address << 12));
			if(posicion_codigo >= 0x10000 && posicion_codigo < 0x12000){
				if(primera_vez_A == 1){
					for(j = 0; j < 1024; j++) {
						pt[j].present = 0;
					}
					primera_vez_A = 0;					
				}	
			}else if(posicion_codigo >= 0x12000 && posicion_codigo < 0x14000){
				if(primera_vez_B == 1){
					for(j = 0; j < 1024; j++) {
						pt[j].present = 0;
					}
					primera_vez_B = 0;					
				}	
			}

		}
		
		
	}

		//identitiy mapping para la seccion del kernel
	for (i = 0; i <= MEMORY_LIMIT; i += 0x1000) {
		mmu_mapear_pagina(i, nuevo_cr3, i, 0, 0);
	}
	//Luego en base a x,y calculamos la posicion en el mapa y llamamos a mmu_mapear_pagina.
	//Que como ya cargamos las entradas de la pte con las mismas direcciones fisicas de todas las tareas.
	//La tarea queda mapeada con todas las paginas ya exploradas por otras tareas

	//calcular la posicion en el mapa virtual
	int dir_virtual_posicion_en_mapa = MAPA_BASE_VIRTUAL + ((y * MAPA_ANCHO) + x)*0x1000; 
	//calcular la posicion en el mapa fisico
	int dir_fisica_posicion_en_mapa = MAPA_BASE_FISICA + ((y * MAPA_ANCHO) + x)*0x1000;
	//calcular las posiciones aledañas
	if(tipo == 0){
		for(i=-1; i<2; i++) {
			for(j=-1; j<2; j++) {
				if( x + i >= 0 && x + i < MAPA_ANCHO && y + j >= 0 && y + j < MAPA_ALTO){
					mmu_mapear_pagina(dir_virtual_posicion_en_mapa + ((i * MAPA_ANCHO) + j)*0x1000, nuevo_cr3, dir_fisica_posicion_en_mapa  + ((i * MAPA_ANCHO) + j)*0x1000, 1, 1);
				}
			}
		}
	}

	//mapeamos la virtual 0x400000 al codigo de la codigo de la tarea
	mmu_mapear_pagina(CODIGO_BASE, nuevo_cr3, dir_fisica_posicion_en_mapa, 1, 1);
	

	//Recupero el cr3 actual para mapearle una virtual a
	unsigned int actual_cr3 = rcr3();


	//este mapeo deberia ser una posicion hardcodeada mas grande que todo el especio de memoria del so para evitar colisiones
	//Esto genera que el c3 actual conozca la direccion fisica donde copiar el codigo de la tarea
	mmu_mapear_pagina(DIR_VIRTUAL_COPIAR_CODIGO_TAREA, actual_cr3, dir_fisica_posicion_en_mapa, 1, 1);
	//copiando el codigo de la tarea
	for(i = 0; i < 0x400; i += 1){
		((int*)DIR_VIRTUAL_COPIAR_CODIGO_TAREA)[i] = ((int*)posicion_codigo)[i];
	}
	//desmapeamos la pagina para que otras tareas no puedan ver el codigo de otra tarea
	mmu_unmapear_pagina(DIR_VIRTUAL_COPIAR_CODIGO_TAREA, actual_cr3);

	return nuevo_cr3;

}


void mmu_mover_pirata(uint cr3_tarea_actual, uint x, uint y, uint tipo, uint index_jugador) {
	//calcular la posicion en el mapa virtual
	int dir_virtual_posicion_en_mapa = MAPA_BASE_VIRTUAL + ((y * MAPA_ANCHO) + x)*0x1000; 
	//calcular la posicion en el mapa fisico
	int dir_fisica_posicion_en_mapa = MAPA_BASE_FISICA + ((y * MAPA_ANCHO) + x)*0x1000;
	//calcular las posiciones aledañas
	int i, j;
	if(tipo == 0) {
		for(i=-1; i<2; i++) {
			for(j=-1; j<2; j++) {
				if( x + i >= 0 && x + i < MAPA_ANCHO && y + j >= 0 && y + j < MAPA_ALTO){
					mmu_mapear_pagina(dir_virtual_posicion_en_mapa + ((i * MAPA_ANCHO) + j)*0x1000, cr3_tarea_actual, dir_fisica_posicion_en_mapa  + ((i * MAPA_ANCHO) + j)*0x1000, 1, 1);
				}
			}
		}
	}
	

	//este mapeo deberia ser una posicion hardcodeada mas grande que todo el especio de memoria del so para evitar colisiones
	//Esto genera que el c3 actual conozca la direccion fisica donde copiar el codigo de la tarea
	mmu_mapear_pagina(DIR_VIRTUAL_COPIAR_CODIGO_TAREA, cr3_tarea_actual, dir_fisica_posicion_en_mapa, 1, 1);

	
	//copiando el codigo de la tarea
	for(i = 0; i < 0x400; i += 1){
		((int*)DIR_VIRTUAL_COPIAR_CODIGO_TAREA)[i] = ((int*)CODIGO_BASE)[i];
	}

	mmu_mapear_pagina(CODIGO_BASE, cr3_tarea_actual, dir_fisica_posicion_en_mapa, 1, 1);


	//desmapeamos la pagina para que otras tareas no puedan ver el codigo de otra tarea
	mmu_unmapear_pagina(DIR_VIRTUAL_COPIAR_CODIGO_TAREA, cr3_tarea_actual);
	

}

uint dame_fisica(uint virtual, uint cr3){

	pde* pd = ((pde*) cr3);

	uint pde_i = virtual >> 22;
	uint pte_i = (virtual << 10) >> 22;
	uint offset = (virtual << 20) >> 20;
	
	pte* pt = ((pte*) (pd[pde_i].address << 12));

	uint pagina = pt[pte_i].address << 12;

	uint fisica = pagina + offset;

	return fisica;
}