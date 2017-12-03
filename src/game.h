/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#ifndef __GAME_H__
#define __GAME_H__

#include "defines.h"

typedef enum direccion_e { ARR = 0x4, ABA = 0x7, DER = 0xA, IZQ = 0xD} direccion;

#define MAX_CANT_PIRATAS_VIVOS           8

#define JUGADOR_A                         0
#define JUGADOR_B                         1

#define MAPA_ANCHO                       80
#define MAPA_ALTO                        44

#define BOTINES_CANTIDAD 8



struct jugador_t;

typedef struct pirata_t
{
    uint id;
    struct jugador_t *jugador;
    uint es_minero;
    uint x;
    uint y;
    uint esta_vivo;
    // id unica, posicion, tipo, reloj
} pirata_t;


typedef struct jugador_t
{
    uint index;
    pirata_t piratas[MAX_CANT_PIRATAS_VIVOS];
    int* direcciones_page_tables;
    int x_puerto;
    int y_puerto;
    int puntos;
    int posiciones_exploradas[MAPA_ANCHO][MAPA_ALTO];
    // coordenadas puerto, posiciones exploradas, mineros pendientes, etc
} jugador_t;


extern jugador_t jugadorA, jugadorB;

// ~ auxiliares dadas ~
uint game_xy2lineal();
pirata_t* id_pirata2pirata(uint id);

// ~ auxiliares sugeridas o requeridas (segun disponga enunciado) ~
void game_pirata_inicializar(pirata_t *pirata, jugador_t *j, uint id, uint tipo);
void game_pirata_erigir(pirata_t *pirata, jugador_t *j, uint tipo);
void game_explorador_visitar_posiciones(jugador_t* jugador, uint x, uint y);
void game_pirata_exploto(uint id);
void game_inicializar();
uint game_posicion_valida(int x, int y);

void game_jugador_inicializar(jugador_t *j);
void game_jugador_lanzar_pirata(int j, uint tipo);
void game_jugador_anotar_punto(jugador_t *j);
void game_chequear_botin(jugador_t *jugador, pirata_t* pirata);

uint game_valor_tesoro(uint x, uint y);
pirata_t* game_pirata_en_posicion(uint x, uint y);

uint game_syscall_pirata_posicion(uint id_jugador, int indice);
uint game_syscall_pirata_mover(uint id, direccion key);
uint game_syscall_manejar(uint syscall, uint param1);
void game_tick(uint id_pirata);
void game_terminar_si_es_hora();
void game_atender_teclado(unsigned char tecla);


#endif  /* !__GAME_H__ */
