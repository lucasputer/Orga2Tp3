/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "sched.h"
#include "i386.h"
#include "game.h"



void sched_inicializar(jugador_t *j_a, jugador_t *j_b){
	//se inicializan fuera de rango, para mostrar que no hay ninguna tarea.
	ultimo_jugadorA = MAX_CANT_PIRATAS_VIVOS;
	ultimo_jugadorB = MAX_CANT_PIRATAS_VIVOS;
	jugador_actual = 0;
	modo_debugg = 0;

	indice_tarea_actual = SCHED_SIN_TAREAS;

	jugador_a = j_a;
	jugador_b = j_b;
}

uint sched_tick(){
	uint proxima = sched_proxima_a_ejecutar();
	if(proxima != 0x0070){
		indice_tarea_actual = proxima >> 3;
	}
	game_tick(indice_tarea_actual);


	return  proxima;
}

void sched_inicializar_jugador(int j){
	if(j == 0 && ultimo_jugadorA == MAX_CANT_PIRATAS_VIVOS){
		ultimo_jugadorA = 0;
	}else if(j == 1 && ultimo_jugadorB == MAX_CANT_PIRATAS_VIVOS){
		ultimo_jugadorB = 0;
	}
}

uint sched_jugador_actual(){
	return jugador_actual;
}

uint sched_proxima_a_ejecutar(){
	pirata_t * tareas_jugador;
	uint pos, pos_inicial;

	if(ultimo_jugadorA == MAX_CANT_PIRATAS_VIVOS && ultimo_jugadorB == MAX_CANT_PIRATAS_VIVOS){
		return 0x0070; //idle
	}

	if(jugador_actual == 0){
		if(ultimo_jugadorB != MAX_CANT_PIRATAS_VIVOS){
			tareas_jugador = jugador_b->piratas;
			pos_inicial = ultimo_jugadorB;
			jugador_actual = 1;
		}else{
			tareas_jugador = jugador_a->piratas;
			pos_inicial = ultimo_jugadorA;
		}
	}else{
		if(ultimo_jugadorA != MAX_CANT_PIRATAS_VIVOS){
			tareas_jugador = jugador_a->piratas;
			pos_inicial = ultimo_jugadorA;
			jugador_actual = 0;
		}else{
			tareas_jugador = jugador_b->piratas;
			pos_inicial = ultimo_jugadorB;
		}
	}

	pos = pos_inicial;
	pos++;
	while(pos < MAX_CANT_PIRATAS_VIVOS && tareas_jugador[pos].esta_vivo == 0){
		pos++;
	}

	if(pos == MAX_CANT_PIRATAS_VIVOS){
		pos = 0;
		while(pos < pos_inicial &&  tareas_jugador[pos].esta_vivo == 0){
			pos++;
		}
	}

	if(jugador_actual == 0){
		ultimo_jugadorA = pos;
	}else{
		ultimo_jugadorB = pos;
	}

	return tareas_jugador[pos].id << 3;
}

char modo_debugg_activado(){
	return modo_debugg;
}

void cambiar_modo_debugg(){
	if(modo_debugg == 1){
		modo_debugg = 0;
	}
	else{
		modo_debugg = 1;
	}
}