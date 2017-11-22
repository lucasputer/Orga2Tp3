/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "screen.h"
#include "game.h"


extern jugador_t jugadorA, jugadorB;


static ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO;

const uchar espacio = 32;
const uchar fondo_negro = 0x00;
const uchar fondo_gris = 0x70;
const uchar fondo_rojo = 0x40;
const uchar fondo_azul = 0x10;
const uchar texto_blanco = 0x0F;
const uchar texto_rojo = 0x04;
const uchar texto_azul = 0x01;
const uchar fondo_rojo_letras_blancas = 0x4F;
const char* nombre_grupo = "Jan Michael Vincent";
const char* relojes = "1 2 3 4 5 6 7 8";
const char* teclado = "1234567890-=qqqwertyuiop[]\\aasdfghjkl;'zzzzxcvbnm,./";
const uint nombre_grupo_len = 19;
const char reloj[] = "|/-\\";
const char* container_tarea_idle = "( )";
const uint container_tarea_idle_offset = 5;
#define reloj_size 4



void screen_actualizar_reloj_global()
{
    static uint reloj_global = 0;

    reloj_global = (reloj_global + 1) % reloj_size;

    screen_pintar(reloj[reloj_global], C_BW, 49, 79);
}

void screen_pintar(uchar c, uchar color, uint fila, uint columna)
{
    p[fila][columna].c = c;
    p[fila][columna].a = color;
}

void screen_pintar_rect(uchar c, uchar color, int fila, int columna, int alto, int ancho)
{   
    for(int row = fila; row < fila + alto; row++){
        for (int col = columna; col < columna + ancho; col++){
            p[row][col].c = c;
            p[row][col].a = color;
        }
    }
}

void screen_pintar_linea_h(uchar c, uchar color, int fila, int columna, int ancho)
{
    screen_pintar_rect(c, color, fila, columna, 1, ancho);
}

void screen_pintar_linea_v(uchar c, uchar color, int fila, int columna, int alto)
{
    screen_pintar_rect(c, color, fila, columna, alto, 1);
}

void screen_inicializar()
{
    //linea negra
    screen_pintar_linea_h(espacio, fondo_negro, 0, 0, VIDEO_COLS);
    // fondo gris
    screen_pintar_rect(espacio, fondo_gris, 1, 0, 44, VIDEO_COLS);
    //barra de abajo
    screen_pintar_rect(espacio, fondo_negro, 45, 0, 5, VIDEO_COLS);
    //equipo rojo
    screen_pintar_rect(espacio, fondo_rojo, 45, 32, 5, 7);
    //equipo azul
    screen_pintar_rect(espacio, fondo_azul, 45, 39, 5, 7);
    //nombre del grupo
    print(nombre_grupo,VIDEO_COLS - nombre_grupo_len,0,texto_blanco);
    //puntajes iniciales
    screen_pintar_puntajes();
    screen_pintar_relojes();
    //lugares disponibles;
    for(int i = 1; i < 9; i++){
        screen_pintar_lugar_disponible(ROJO, i);
        screen_pintar_lugar_disponible(AZUL, i);
    }
    print(container_tarea_idle,VIDEO_COLS - container_tarea_idle_offset,49,texto_blanco);

}

void  screen_pintar_relojes(){
    int columna = 3;
    int fila = 46;
    print(relojes, columna, fila, texto_blanco);
    columna = 60;
    print(relojes, columna, fila, texto_blanco);
}

void screen_pintar_lugar_disponible(equipo eq, int posicion){
    int fila = 48;
    int color = fondo_negro;
    int columna = 0;
    int offset = (posicion - 1)*2;
    if(eq == ROJO){
        color = color | texto_rojo;
        columna = 3 + offset;
    }else{
        color = color | texto_azul;
        columna = 60 + offset;
    }
    char * c = "X ";
    print(c,columna,fila,color);
}

void screen_pintar_puntajes(){
    screen_pintar_puntaje("000",ROJO);
    screen_pintar_puntaje("000",AZUL);
}

void screen_pintar_puntaje(const char * puntaje, equipo equipo){
    uchar color = texto_blanco;
    if (equipo == ROJO){
        color =  color | fondo_rojo;
        print(puntaje,34,47,color);
    }else{
        color = color | fondo_azul;
        print(puntaje,41,47,color);
    }
}

uchar screen_valor_actual(uint fila, uint columna)
{
    return p[fila][columna].c;
}

void print(const char * text, uint x, uint y, unsigned short attr) {
    int i;
    for (i = 0; text[i] != 0; i++)
     {
        screen_pintar(text[i], attr, y, x);

        x++;
        if (x == VIDEO_COLS) {
            x = 0;
            y++;
        }
    }
}

void print_hex(uint numero, int size, uint x, uint y, unsigned short attr) {
    int i;
    char hexa[8];
    char letras[16] = "0123456789ABCDEF";
    hexa[0] = letras[ ( numero & 0x0000000F ) >> 0  ];
    hexa[1] = letras[ ( numero & 0x000000F0 ) >> 4  ];
    hexa[2] = letras[ ( numero & 0x00000F00 ) >> 8  ];
    hexa[3] = letras[ ( numero & 0x0000F000 ) >> 12 ];
    hexa[4] = letras[ ( numero & 0x000F0000 ) >> 16 ];
    hexa[5] = letras[ ( numero & 0x00F00000 ) >> 20 ];
    hexa[6] = letras[ ( numero & 0x0F000000 ) >> 24 ];
    hexa[7] = letras[ ( numero & 0xF0000000 ) >> 28 ];
    for(i = 0; i < size; i++) {
        p[y][x + size - i - 1].c = hexa[i];
        p[y][x + size - i - 1].a = attr;
    }
}

void print_dec(uint numero, int size, uint x, uint y, unsigned short attr) {
    int i;
    char letras[16] = "0123456789";

    for(i = 0; i < size; i++) {
        int resto  = numero % 10;
        numero = numero / 10;
        p[y][x + size - i - 1].c = letras[resto];
        p[y][x + size - i - 1].a = attr;
    }
}


void screen_pintar_error(uint intCode, uint errorCode){
    print("Interrupcion: ",0,0,fondo_rojo_letras_blancas);
    print_dec(intCode, 3, 13, 0, fondo_rojo_letras_blancas);
    print(" Error Code: ",17,0,fondo_rojo_letras_blancas);
    print_hex(errorCode, 8, 29, 0, fondo_rojo_letras_blancas);
}
void screen_pintar_interrupcion(uint intCode){
    print("Interrupcion:",0,0,fondo_rojo_letras_blancas);
    print_dec(intCode, 3, 14, 0, fondo_rojo_letras_blancas);
}
void screen_pintar_tecla(uint intCode){
    if(intCode > 53){
        intCode -= 130;
        char* s = "";
        s[0] = teclado[intCode];
        print(s,0, 0, fondo_rojo_letras_blancas);
    }
}
