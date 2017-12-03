/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "game.h"
#include "mmu.h"
#include "tss.h"
#include "screen.h"
#include "sched.h"

#include <stdarg.h>


#define POS_INIT_A_X                      1
#define POS_INIT_A_Y                      1
#define POS_INIT_B_X         MAPA_ANCHO - 2
#define POS_INIT_B_Y          MAPA_ALTO - 2

#define CANT_POSICIONES_VISTAS            9
#define MAX_SIN_CAMBIOS                 999

//pase BOTINES_CANTIDAD al .h, esta feito, consultar 

uint botines[BOTINES_CANTIDAD][3] = { // TRIPLAS DE LA FORMA (X, Y, MONEDAS)
                                        {30,  3, 50}, {31, 38, 50}, {15, 21, 100}, {45, 21, 100} ,
                                        {49,  3, 50}, {48, 38, 50}, {64, 21, 100}, {34, 21, 100}
                                    };

jugador_t jugadorA;
jugador_t jugadorB;



void* error()
{
	__asm__ ("int3");
	return 0;
}

uint game_xy2lineal (uint x, uint y) {
	return (y * MAPA_ANCHO + x);
}

uint game_posicion_valida(int x, int y) {
	return (x >= 0 && y >= 0 && x < MAPA_ANCHO && y < MAPA_ALTO);
}

pirata_t* id_pirata2pirata(uint id_pirata)
{
    // ~ completar ~
	return NULL;
}

uint game_dir2xy(direccion dir, int *x, int *y)
{
	switch (dir)
	{
		case IZQ: *x = -1; *y =  0; break;
		case DER: *x =  1; *y =  0; break;
		case ABA: *x =  0; *y =  1; break;
		case ARR: *x =  0; *y = -1; break;
    	default: return -1;
	}

	return 0;
}

uint game_valor_tesoro(uint x, uint y)
{
	int i;
	for (i = 0; i < BOTINES_CANTIDAD; i++)
	{
		if (botines[i][0] == x && botines[i][1] == y)
			return botines[i][2];
	}
	return 0;
}

// dada una posicion (x,y) guarda las posiciones de alrededor en dos arreglos, uno para las x y otro para las y
void game_calcular_posiciones_vistas(int *vistas_x, int *vistas_y, int x, int y)
{
	int next = 0;
	int i, j;
	for (i = -1; i <= 1; i++)
	{
		for (j = -1; j <= 1; j++)
		{
			vistas_x[next] = x + j;
			vistas_y[next] = y + i;
			next++;
		}
	}
}


void game_inicializar()
{
	int paginas;
	int direcciones_fisicas_page_table_A[FISICAS_PTE_JUGADOR];
	int direcciones_fisicas_page_table_B[FISICAS_PTE_JUGADOR];
	for(paginas = 0; paginas < FISICAS_PTE_JUGADOR; paginas ++) {
		direcciones_fisicas_page_table_A[paginas] = dame_libre();
		direcciones_fisicas_page_table_B[paginas] = dame_libre();
	}
	jugadorA.direcciones_page_tables = direcciones_fisicas_page_table_A;
	jugadorB.direcciones_page_tables = direcciones_fisicas_page_table_B;
	game_jugador_inicializar(&jugadorA);
	game_jugador_inicializar(&jugadorB);

	sched_inicializar(&jugadorA, &jugadorB);
}

void game_jugador_inicializar_mapa(jugador_t *jug)
{

}

void game_jugador_inicializar(jugador_t *j)
{
	static int index = 0;

	j->index = index;
	j->puntos = 0;
    
    if(index == 0){
    	j->x_puerto = POS_INIT_A_X;
    	j->y_puerto = POS_INIT_A_Y;
    }else{
    	j->x_puerto = POS_INIT_B_X;
    	j->y_puerto = POS_INIT_B_Y;
    }

    for(int i = 0; i < MAX_CANT_PIRATAS_VIVOS; i++){
    	j->piratas[i].esta_vivo = 0;
    }

    for(int i = 0; i < BOTINES_CANTIDAD; i++){
    	j->botines_descubiertos[i][0] = MAPA_ANCHO;
    	j->botines_descubiertos[i][1] = MAPA_ALTO;
    }
   	j->ultimo_botin_index = 0;
    

    index++;
}

void game_pirata_inicializar(pirata_t *pirata, jugador_t *j, uint id, uint tipo)
{
	pirata->id = id;
	pirata->es_minero = tipo;
	pirata->jugador = j;
	pirata->x = j->x_puerto;
	pirata->y = j->y_puerto;
	pirata->esta_vivo = 1;
}

void game_tick(uint id_pirata)
{
	screen_actualizar_reloj_global();
	if(id_pirata != SCHED_SIN_TAREAS){
		screen_actualizar_reloj_pirata(id_pirata);
	}
}


void game_pirata_relanzar(pirata_t *pirata, jugador_t *j, uint tipo)
{
}


void game_jugador_lanzar_pirata(int j, uint tipo)
{
	jugador_t *jugador;
	if(j == 0){
		jugador = &jugadorA;
	}else{
		jugador = &jugadorB;
	}

	//chequear si tiene lugar para lanzar
	int i = 0;
	int found = 0;
	while(i < MAX_CANT_PIRATAS_VIVOS && !found){
		if(jugador->piratas[i].esta_vivo == 0){
			found = 1;
		}else{
			i++;
		}
	}

	if(!found)
		return;

	uint id = i;
	if(j == 1){
		id += GDT_IDX_TSS_PRIMER_TAREA_JUGADOR_B;
	}else{
		id += GDT_IDX_TSS_PRIMER_TAREA_JUGADOR_A;
	}

	game_pirata_inicializar(&(jugador->piratas[i]), jugador, id, tipo);
	//lanzar la tarea
	tss_inicializar_pirata(tipo, i,*jugador,jugador->piratas[i]);

	screen_pirata_movimiento(jugador, tipo,jugador->piratas[i].y, jugador->piratas[i].x, MAPA_ALTO, MAPA_ANCHO);

	sched_inicializar_jugador(j);
}

void game_pirata_habilitar_posicion(jugador_t *j, pirata_t *pirata, int x, int y)
{
}


void game_chequear_botin(jugador_t* jugador, pirata_t* pirata)
{
	if(pirata->es_minero == 0){
	    for(int i=-1; i<2; i++) {
	        for(int j=-1; j<2; j++) {
	            for(int b = 0; b < BOTINES_CANTIDAD; b++){
	            	if( pirata->x + i == botines[b][0] && pirata->y + j == botines[b][1]){

	            		//chequeo si es la primera vez que encuentro el botin
	            		int primera_vez = 1, d = 0;
	            		while(primera_vez == 1 && d < (jugador->ultimo_botin_index)){
	            			if(jugador->botines_descubiertos[d][0] == pirata->x && jugador->botines_descubiertos[d][1] == pirata->y){
	            				primera_vez = 0;
	            			}
	            			d++;
	            		}

	            		if(primera_vez == 1){
	            			//lanzar minero
							game_jugador_lanzar_pirata(jugador->index, 1);
							//agregar botin a botines encontrados
							jugador->botines_descubiertos[jugador->ultimo_botin_index][0] = pirata->x;
							jugador->botines_descubiertos[jugador->ultimo_botin_index][1] = pirata->y;
							jugador->ultimo_botin_index += 1;
	            		}
						//pintar botin
						screen_pintar_botin(jugador, pirata->x +i, pirata->y +j);
					}
	            }
	        }
	    }		
	}else{
		for(int b = 0; b < BOTINES_CANTIDAD; b++){
			if(pirata->x == botines[b][0] && pirata->y == botines[b][1]){
				//sumar puntos, sacar botin
			}
		}
	}
}


uint game_syscall_pirata_mover(uint id_jugador, direccion dir)
{	
	int x_prev = MAPA_ANCHO;
	int y_prev = MAPA_ALTO;
	int x = 0;
	int y = 0;
	game_dir2xy(dir, &x, &y);
	uint id_pirata_actual = rtr() >> 3;
	pirata_t* pirata_actual;
	jugador_t* jugador;
	if(id_jugador == 0) {
		jugador = &jugadorA;
		uint index_pirata_actual = id_pirata_actual - GDT_IDX_TSS_PRIMER_TAREA_JUGADOR_A;
 		pirata_actual = &(jugador->piratas[index_pirata_actual]);
	}else{
		jugador = &jugadorB;
		uint index_pirata_actual_jugador_b = id_pirata_actual - GDT_IDX_TSS_PRIMER_TAREA_JUGADOR_B;
 		pirata_actual = &(jugador->piratas[index_pirata_actual_jugador_b]);
	}

	
 	x+=pirata_actual->x;
 	y+=pirata_actual->y;
 	if(!game_posicion_valida(x,y)) {
 		error();
 	}

 	x_prev = pirata_actual->x;
 	y_prev = pirata_actual->y;
 	pirata_actual->x = x;
 	pirata_actual->y = y;
	
 	mmu_mover_pirata(rcr3(),pirata_actual->x, pirata_actual->y, pirata_actual->es_minero, jugador->index);
 	screen_pirata_movimiento(jugador, pirata_actual->es_minero, pirata_actual->x, pirata_actual->y, x_prev, y_prev);

 	game_chequear_botin(jugador,pirata_actual);

    return 0;
}

uint game_syscall_cavar(uint id)
{
    // ~ completar ~

	return 0;
}

uint game_syscall_pirata_posicion(uint id, int idx)
{
    // ~ completar ~
    return 0;
}

uint game_syscall_manejar(uint syscall, uint param1)
{
    // ~ completar ~
    return 0;
}

void game_pirata_exploto(uint id)
{
}

pirata_t* game_pirata_en_posicion(uint x, uint y)
{
	return NULL;
}


void game_jugador_anotar_punto(jugador_t *j)
{
}



void game_terminar_si_es_hora()
{
}


#define KB_w        0x11 // 0x91
#define KB_s        0x1f // 0x9f
#define KB_a        0x1e // 0x9e
#define KB_d        0x20 // 0xa0
#define KB_e        0x12 // 0x92
#define KB_q        0x10 // 0x90
#define KB_i        0x17 // 0x97
#define KB_k        0x25 // 0xa5
#define KB_j        0x24 // 0xa4
#define KB_l        0x26 // 0xa6
#define KB_shiftL   0x2a // 0xaa
#define KB_shiftR   0x36 // 0xb6


void game_atender_teclado(unsigned char tecla)
{
}
