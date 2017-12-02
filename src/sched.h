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

uint ultimo_jugadorA;
uint ultimo_jugadorB;
char jugador_actual;
uint indice_tarea_actual;
jugador_t* jugador_a;
jugador_t* jugador_b;
char modo_debugg;


uint sched_tick();
uint sched_proxima_a_ejecutar();
void sched_inicializar();
uint sched_jugador_actual();
void sched_inicializar_jugador(int j);
char modo_debugg_activado();
void cambiar_modo_debugg();
#endif	/* !__SCHED_H__ */
