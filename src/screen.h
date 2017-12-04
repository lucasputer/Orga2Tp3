/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#ifndef __SCREEN_H__
#define __SCREEN_H__

/* Definicion de la pantalla */
#define VIDEO_FILS 50
#define VIDEO_COLS 80

#define VIDEO                   0x000B8000 /* direccion fisica del buffer de video */


#include "colors.h"
#include "defines.h"
/* Estructura de para acceder a memoria de video */
typedef struct ca_s {
    unsigned char c;
    unsigned char a;
} ca;

struct pirata_t;
typedef struct pirata_t pirata_t;

struct jugador_t;
typedef struct jugador_t jugador_t;

int ee_printf(const char *fmt, ...);


void screen_pintar(unsigned char c, unsigned char color, uint fila, uint columna);
void print(const char * text, unsigned int x, unsigned int y, unsigned short attr);
void print_hex(unsigned int numero, int size, unsigned int x, unsigned int y, unsigned short attr);
void print_dec(unsigned int numero, int size, unsigned int x, unsigned int y, unsigned short attr);
void screen_pintar_rect(unsigned char c, unsigned char color, int fila, int columna, int alto, int ancho);
void screen_pintar_linea_h(unsigned char c, unsigned char color, int fila, int columna, int ancho);
void screen_pintar_linea_v(unsigned char c, unsigned char color, int fila, int columna, int alto);
void screen_inicializar();
void screen_pintar_puntajes();
void screen_pintar_puntaje(int puntaje, uint id_jugador);
void screen_pintar_relojes();
void screen_pintar_lugar_disponible(uint id_jugador, int posicion);

// void screen_pintar_error(const int intCode, const int errorCode);
void screen_pintar_error(uint intCode, uint errorCode);
void screen_pintar_interrupcion(uint intCode);


void screen_actualizar_reloj_global();
// void screen_actualizar_reloj_pirata (jugador_t *j, pirata_t *pirata);
void screen_actualizar_reloj_pirata(uint id_pirata);
void screen_limpiar_reloj_pirata(uint id_pirata);
unsigned char screen_color_jugador(jugador_t *j);
unsigned char screen_caracter_pirata(unsigned int tipo);
void screen_pirata_movimiento(jugador_t *j, uint tipo, uint x, uint y, uint x_prev, uint y_prev);
void screen_pintar_reloj_pirata(char c, uint id_jugador, int posicion);
void screen_pintar_reloj_piratas(jugador_t *j);
void screen_pintar_relojes();
uint screen_posicion_valida(int x, int y);
void screen_pintar_botin(jugador_t *j, uint x, uint y);
void screen_pintar_botin_vacio(jugador_t *j, uint x, uint y);
void screen_pintar_vacio(jugador_t *j, uint x, uint y);
void screen_borrar_pirata(pirata_t* pirata);

void screen_actualizar_posicion_mapa(uint x, uint y);
void screen_stop_game_show_winner(jugador_t *j);

void screen_pintar_modo_debug();
void screen_despintar_modo_debug();


#endif  /* !__SCREEN_H__ */

