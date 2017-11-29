/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "sched.h"
#include "i386.h"


void sched_inicializar(){
	//se inicializan fuera de rango, para mostrar que no hay ninguna tarea.
	ultimo_jugadorA = MAX_CANT_PIRATAS_VIVOS;
	ultimo_jugadorB = MAX_CANT_PIRATAS_VIVOS;
	jugador_actual_es_jugadorA = 1;
	int i;
	for(i = 0; i < MAX_CANT_PIRATAS_VIVOS; i ++){
		tareas_jugadorA[i] = 0;
		tareas_jugadorB[i] = 0;
	}

	indice_tarea_actual = -1;
}

uint sched_tick(){
	uint proxima = sched_proxima_a_ejecutar();
	game_tick(indice_tarea_actual);
	return  proxima;
}

uint sched_jugador_actual(){
	return 1;
	//USAR RTR PARA CALCULAR EN BASE AL TASK REGISTER CARGADO CUAL ES EL ID DEL JUGADOR A DEVOLVER
}

uint sched_proxima_a_ejecutar(){
	uint * tareas_jugador;
	uint pos, pos_inicial;

	if(ultimo_jugadorA == MAX_CANT_PIRATAS_VIVOS && ultimo_jugadorB == MAX_CANT_PIRATAS_VIVOS){
		return 0x0070; //idle
	}

	if(jugador_actual_es_jugadorA == 1){
		if(ultimo_jugadorB != MAX_CANT_PIRATAS_VIVOS){
			tareas_jugador = tareas_jugadorB;
			pos_inicial = ultimo_jugadorB;
			jugador_actual_es_jugadorA = 0;
		}else{
			tareas_jugador = tareas_jugadorA;
			pos_inicial = ultimo_jugadorA;
		}
	}else{
		if(ultimo_jugadorA != MAX_CANT_PIRATAS_VIVOS){
			tareas_jugador = tareas_jugadorA;
			pos_inicial = ultimo_jugadorA;
			jugador_actual_es_jugadorA = 1;
		}else{
			tareas_jugador = tareas_jugadorB;
			pos_inicial = ultimo_jugadorB;
		}
	}

	pos = pos_inicial;
	pos++;
	while(pos < MAX_CANT_PIRATAS_VIVOS && tareas_jugador[pos] == 0){
		pos++;
	}

	if(pos == MAX_CANT_PIRATAS_VIVOS){
		pos = 0;
		while(pos < pos_inicial &&  tareas_jugador[pos] == 0){
			pos++;
		}
	}

	if(jugador_actual_es_jugadorA){
		indice_tarea_actual = pos;
	}else{
		indice_tarea_actual = pos + 8;
	}

	return tareas_jugador[pos];
}
