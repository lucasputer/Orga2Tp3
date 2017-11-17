/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de estructuras para administrar tareas
*/

#include "tss.h"
#include "mmu.h"



tss tss_jugadorA[MAX_CANT_PIRATAS_VIVOS];
tss tss_jugadorB[MAX_CANT_PIRATAS_VIVOS];

int cantidad_tareas_a = 0;
int cantidad_tareas_b = 0;

void tss_inicializar() {

	tss_inicial.ptl=0;
	tss_inicial.unused0=0;
	tss_inicial.esp0=0;
	tss_inicial.ss0=0;
	tss_inicial.unused1=0;
	tss_inicial.esp1=0;
	tss_inicial.ss1=0;
	tss_inicial.unused2=0;
	tss_inicial.esp2=0;
	tss_inicial.ss2=0;
	tss_inicial.unused3=0;
	tss_inicial.cr3=0;
	tss_inicial.eip=0;
	tss_inicial.eflags=0;
	tss_inicial.eax=0;
	tss_inicial.ecx=0;
	tss_inicial.edx=0;
	tss_inicial.ebx=0;
	tss_inicial.esp=0;
	tss_inicial.ebp=0;
	tss_inicial.esi=0;
	tss_inicial.edi=0;
	tss_inicial.es=0;
	tss_inicial.unused4=0;
	tss_inicial.cs=0;
	tss_inicial.unused5=0;
	tss_inicial.ss=0;
	tss_inicial.unused6=0;
	tss_inicial.ds=0;
	tss_inicial.unused7=0;
	tss_inicial.fs=0;
	tss_inicial.unused8=0;
	tss_inicial.gs=0;
	tss_inicial.unused9=0;
	tss_inicial.ldt=0;
	tss_inicial.unused10=0;
	tss_inicial.dtrap=0;
	tss_inicial.iomap=0;


	gdt[GDT_IDX_TSS_INIT].limit_0_15 = 0x1000;
    gdt[GDT_IDX_TSS_INIT].base_0_15 = ((unsigned int)(&tss_inicial));
    gdt[GDT_IDX_TSS_INIT].base_23_16 = ((unsigned int)(&tss_inicial)>>16);
    gdt[GDT_IDX_TSS_INIT].type = 0x09;
    gdt[GDT_IDX_TSS_INIT].s = 0x0;
    gdt[GDT_IDX_TSS_INIT].dpl = 0x0;
    gdt[GDT_IDX_TSS_INIT].p = 0x1;
    gdt[GDT_IDX_TSS_INIT].limit_16_19 = 0x0;
    gdt[GDT_IDX_TSS_INIT].avl = 0x0;
    gdt[GDT_IDX_TSS_INIT].l = 0x0;
    gdt[GDT_IDX_TSS_INIT].db = 0x0;
    gdt[GDT_IDX_TSS_INIT].g = 0x0;
    gdt[GDT_IDX_TSS_INIT].base_31_24 = ((unsigned int)(&tss_inicial)>>24);

}

void tss_inicializar_idle() {
	tss_idle.ptl=0;
	tss_idle.unused0=0;
	tss_idle.esp0=0;
	tss_idle.ss0=0;
	tss_idle.unused1=0;
	tss_idle.esp1=0;
	tss_idle.ss1=0;
	tss_idle.unused2=0;
	tss_idle.esp2=0;
	tss_idle.ss2=0;
	tss_idle.unused3=0;
	tss_idle.cr3=0x27000;
	tss_idle.eip=0x16000;
	tss_idle.eflags=0x202;
	tss_idle.eax=0;
	tss_idle.ecx=0;
	tss_idle.edx=0;
	tss_idle.ebx=0;
	tss_idle.esp=0x1000;
	tss_idle.ebp=0x1000;
	tss_idle.esi=0;
	tss_idle.edi=0;
	tss_idle.es=0x0050;
	tss_idle.unused4=0;
	tss_idle.cs=0x0040;
	tss_idle.unused5=0;
	tss_idle.ss=0x0050;
	tss_idle.unused6=0;
	tss_idle.ds=0x0050;
	tss_idle.unused7=0;
	tss_idle.fs=0x0050;
	tss_idle.unused8=0;
	tss_idle.gs=0x0050;
	tss_idle.unused9=0;
	tss_idle.ldt=0;
	tss_idle.unused10=0;
	tss_idle.dtrap=0;
	tss_idle.iomap=0;

	gdt[GDT_IDX_TSS_IDLE].limit_0_15 = 0x1000;
    gdt[GDT_IDX_TSS_IDLE].base_0_15 = ((unsigned int)(&tss_idle));
    gdt[GDT_IDX_TSS_IDLE].base_23_16 = ((unsigned int)(&tss_idle)>>16);
    gdt[GDT_IDX_TSS_IDLE].type = 0x09;
    gdt[GDT_IDX_TSS_IDLE].s = 0x0;
    gdt[GDT_IDX_TSS_IDLE].dpl = 0x0;
    gdt[GDT_IDX_TSS_IDLE].p = 0x1;
    gdt[GDT_IDX_TSS_IDLE].limit_16_19 = 0x0;
    gdt[GDT_IDX_TSS_IDLE].avl = 0x0;
    gdt[GDT_IDX_TSS_IDLE].l = 0x0;
    gdt[GDT_IDX_TSS_IDLE].db = 0x0;
    gdt[GDT_IDX_TSS_IDLE].g = 0x0;
    gdt[GDT_IDX_TSS_IDLE].base_31_24 = ((unsigned int)(&tss_idle)>>24);
 
}

void init_tss(tss* new_tss, int cs, int ds, int ss0) {
	new_tss->ptl=0;
	new_tss->unused0=0;
	new_tss->esp0=dame_libre();
	new_tss->ss0=ss0;
	new_tss->unused1=0;
	new_tss->esp1=0;
	new_tss->ss1=0;
	new_tss->unused2=0;
	new_tss->esp2=0;
	new_tss->ss2=0;
	new_tss->unused3=0;
	new_tss->cr3=0;
	new_tss->eip=0;
	new_tss->eflags=0x202;
	new_tss->eax=0;
	new_tss->ecx=0;
	new_tss->edx=0;
	new_tss->ebx=0;
	new_tss->esp=0;
	new_tss->ebp=0;
	new_tss->esi=0;
	new_tss->edi=0;
	new_tss->es=ds;
	new_tss->unused4=0;
	new_tss->cs=cs;
	new_tss->unused5=0;
	new_tss->ss=ds;
	new_tss->unused6=0;
	new_tss->ds=ds;
	new_tss->unused7=0;
	new_tss->fs=ds;
	new_tss->unused8=0;
	new_tss->gs=ds;
	new_tss->unused9=0;
	new_tss->ldt=0;
	new_tss->unused10=0;
	new_tss->dtrap=0;
	new_tss->iomap=0;

}


void tss_inicializar_tarea(int tarea, int jugador, int x, int y) {
	//JUGADOR A
	if(jugador == 1) {
		int fisicas[3];
		int i = 0;
		for(i=0;i<3;i++){
			fisicas[i] = dame_libre();
		}
		int t_pd = mmu_inicializar_dir_pirata(x, y, tarea, fisicas);
		//mmu_mapear_pagina(0x101000, t_pd, dame_proxima_libre())
		tss tss_act = tss_jugadorA[cantidad_tareas_a];
		init_tss(&tss_act, 0x48,0x58,0x50);
		tss_act.esp = tarea + 0xfff;
		tss_act.ebp = tarea + 0xfff;
		tss_act.eip = tarea;
		tss_act.cr3 = t_pd;
		cantidad_tareas_a++;
	} else {
		//JUGADOR B
	}
}