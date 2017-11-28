/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#ifndef __SCHED_H__
#define __SCHED_H__

#include "game.h"
#include "tss.h"
#include "gdt.h"
#include "mmu.h"
#include "i386.h"

uint tareas_jugadorA[MAX_CANT_PIRATAS_VIVOS];
uint tareas_jugadorB[MAX_CANT_PIRATAS_VIVOS];
uint ultimo_jugadorA;
uint ultimo_jugadorB;
char jugador_actual_es_jugadorA;
uint indice_tarea_actual;


uint sched_tick();
uint sched_proxima_a_ejecutar();
void sched_inicializar();
jugador_t sched_jugador_actual();
#endif	/* !__SCHED_H__ */
