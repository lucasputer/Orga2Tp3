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
#define MAX_SIN_CAMBIOS                9999

//pase BOTINES_CANTIDAD al .h, esta feito, consultar 

uint botines[BOTINES_CANTIDAD][3] = { // TRIPLAS DE LA FORMA (X, Y, MONEDAS)
                                        {30,  1, 50}, {31, 38, 50}, {15, 21, 100}, {45, 21, 100} ,
                                        {47,  30, 50}, {48, 38, 50}, {64, 21, 100}, {34, 21, 100}
                                    };

jugador_t jugadorA;
jugador_t jugadorB;
uint tiempo_restante = MAX_SIN_CAMBIOS;

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

    int i;
    for(i = 0; i < MAX_CANT_PIRATAS_VIVOS; i++){
    	j->piratas[i].esta_vivo = 0;
    }

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

void game_jugador_lanzar_explorador(int j)
{	
	jugador_t* jugador = &jugadorA;
	if(j == 1){
		jugador = &jugadorB;
	}
	game_jugador_lanzar_pirata(j, 0,jugador->x_puerto, jugador->y_puerto);
}

void game_jugador_lanzar_minero(int j, int x, int y)
{
	game_jugador_lanzar_pirata(j, 1, x, y);
}

void game_jugador_lanzar_pirata(int j, uint tipo, int x, int y)
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
	tss_inicializar_pirata(tipo, i,jugador,jugador->piratas[i],x,y);

	if(tipo == 0){
		game_chequear_botin(jugador,&(jugador->piratas[i]));
	}
	screen_pirata_movimiento(jugador, tipo,jugador->piratas[i].y, jugador->piratas[i].x, MAPA_ALTO, MAPA_ANCHO);


	sched_inicializar_jugador(j);
}


void game_chequear_botin(jugador_t* jugador, pirata_t* pirata)
{
	if(pirata->es_minero == 0){
		int i = -1;
		int j = -1;
		int b = 0;
	    for(i=-1; i<2; i++) {
	        for(j=-1; j<2; j++) {
	        	if(game_posicion_valida(pirata->x + i, pirata->y + j)){
	        		int pos = posiciones_exploradas[jugador->index][pirata->x + i][pirata->y + j];
	        		if( pos != 1 && pos != 2 && pos != 3){
	        			screen_pintar_vacio(jugador, pirata->x +i, pirata->y +j);
	        			posiciones_exploradas[jugador->index][pirata->x + i][pirata->y + j] = 1;
	        			for(b = 0; b < BOTINES_CANTIDAD; b++){
	        				if( pirata->x + i == botines[b][0] && pirata->y + j == botines[b][1]){
	        					if(botines[b][2] > 0){
	        						game_jugador_lanzar_minero(jugador->index, pirata->x + i, pirata->y + j);
	        						screen_pintar_botin(jugador, pirata->x + i, pirata->y + j);
	        						posiciones_exploradas[jugador->index][pirata->x + i][pirata->y + j] = 2;
	        					}else{
	        						screen_pintar_botin_vacio(jugador, pirata->x +i, pirata->y +j);
	        						posiciones_exploradas[jugador->index][pirata->x + i][pirata->y + j] = 3;
	        					}
	        				}
	        			}
	        		}
	        		
	        	}
	        }
	    }	
	}	
}


uint game_syscall_pirata_mover(uint id_jugador, direccion dir)
{	
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
 	if(!game_posicion_valida(x,y))
 		error();

 	if(pirata_actual->es_minero&& posiciones_exploradas[jugador->index][x][y] == 0)
 		error();

 	
 	int x_prev, y_prev;
 	x_prev = pirata_actual->x;
 	y_prev = pirata_actual->y;
 	pirata_actual->x = x;
 	pirata_actual->y = y;
 	// marco como visitadas las nuevas posiciones
 	if(!(pirata_actual-> es_minero)){
 		game_chequear_botin(jugador,pirata_actual);
 	}
 	screen_pirata_movimiento(jugador, pirata_actual->es_minero, x, y, x_prev, y_prev);
	
 	mmu_mover_pirata(rcr3(),pirata_actual->x, pirata_actual->y, pirata_actual->es_minero, jugador->index);

    return 0;
}

void game_syscall_pirata_cavar()
{
    int x,y;
    game_posicion_pirata_actual(&x,&y);

    uint id_jugador = sched_jugador_actual();
    jugador_t* jugador;
	if(id_jugador == 0){
		jugador = &jugadorA;
	}else{
		jugador = &jugadorB;
	}

    int i = 0;
    int found = 0;
    while(i < BOTINES_CANTIDAD && !found){
    	if(botines[i][0] == x && botines[i][1] == y && botines[i][2] > 0){
    		found = 1;
    	}else{
    		i++;
    	}
    }

    if(!found){
    	i = 0;
    	found = 0;
	    while(i < BOTINES_CANTIDAD && !found){
	    	if(botines[i][0] == x && botines[i][1] == y && botines[i][2] == 0){
	    		screen_pintar_botin_vacio(jugador, x, y);
	    		found = 1;
	    	}
	    	else{
	    		i++;
	    	}
	    }
    	tss_matar_tarea();
    }else{
    	game_jugador_anotar_punto(jugador);
    	botines[i][2] = botines[i][2] - 1;
    	if(botines[i][2] == 0){
    		posiciones_exploradas[jugador->index][x][y] = 3;
    	}
    }
}

void game_posicion_pirata_actual(int* x, int* y){
	int indice = rtr() >> 3;
	indice -= 15;
	jugador_t* jugador = &jugadorA;
	if(indice >= 8){
		indice -= 8;
		jugador = &jugadorB;
	}

	pirata_t* pirata = &(jugador->piratas[indice]);
	*x =  pirata->x;
	*y = pirata->y;
}

uint game_syscall_pirata_posicion(uint id_jugador, int indice)
{	
	jugador_t* jugador;

	int x,y;

	if(indice == -1){
		game_posicion_pirata_actual(&x,&y);
	}else{
		if(id_jugador == 0) {
			jugador = &jugadorA;
		}else{
			jugador = &jugadorB;
		}
		x = jugador->piratas[indice].x;
		y = jugador->piratas[indice].y;
	}
	return (y << 8 | x);
}


void game_jugador_anotar_punto(jugador_t *j)
{
	j->puntos = j->puntos + 1;
	tiempo_restante = MAX_SIN_CAMBIOS;
	screen_pintar_puntaje(j->puntos, j->index);
}



void game_terminar_si_es_hora()
{
	int i = 0, j = 0;
	tiempo_restante--;
	while(i < BOTINES_CANTIDAD){
		if(botines[i][2] != 0){
			i = BOTINES_CANTIDAD;
		}else{
			i++;
		}
		j++;
	}
	if(i == j || tiempo_restante == 0){
		int ganador = 0;
		if(jugadorA.puntos < jugadorB.puntos)
			ganador = 1;

		screen_pintar_ganador(ganador);
		sched_terminar_juego();
	}
}
 
void game_matar_pirata(uint tr) {
	uint id_pirata = tr >> 3;
	
	int i = 0;
	while (i< MAX_CANT_PIRATAS_VIVOS && jugadorA.piratas[i].id != id_pirata)
		i++;

	if(i < MAX_CANT_PIRATAS_VIVOS) 
		jugadorA.piratas[i].esta_vivo = 0;
	
	i = 0;
	while (i< MAX_CANT_PIRATAS_VIVOS && jugadorB.piratas[i].id != id_pirata) 
		i++;
	
	if(i < MAX_CANT_PIRATAS_VIVOS) 
		jugadorB.piratas[i].esta_vivo = 0;

	char hayVivoA = 0;
	char hayVivoB = 0;
	for(i = 0 ; i < MAX_CANT_PIRATAS_VIVOS  ; i++){
		if(jugadorA.piratas[i].esta_vivo == 1)
			hayVivoA = 1;
		if(jugadorB.piratas[i].esta_vivo == 1)
			hayVivoB = 1;
	}

	if(hayVivoA == 0)
		set_ultimo_jugador_a(MAX_CANT_PIRATAS_VIVOS);

	if(hayVivoB == 0)
		set_ultimo_jugador_b(MAX_CANT_PIRATAS_VIVOS);

	screen_limpiar_reloj_pirata(id_pirata);

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
