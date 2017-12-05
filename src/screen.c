/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "screen.h"
#include "game.h"
#include "i386.h"
#include "tss.h"


extern jugador_t jugadorA, jugadorB;


static ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO;
ca ptemp[VIDEO_FILS][VIDEO_COLS];

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
int estadoRelojes[16];
#define reloj_size 4



void screen_actualizar_reloj_global()
{
    static uint reloj_global = 0;

    reloj_global = (reloj_global + 1) % reloj_size;

    screen_pintar(reloj[reloj_global], C_BW, 49, 79);
}

void screen_actualizar_reloj_pirata(uint id_pirata)
{   
    id_pirata = id_pirata - GDT_IDX_TSS_PRIMER_TAREA_JUGADOR_A + 1;

    int estadoReloj = (estadoRelojes[id_pirata-1] + 1) % reloj_size;
    char c = reloj[estadoReloj];
    estadoRelojes[id_pirata-1] = estadoReloj;
    if(id_pirata <= 8){
        screen_pintar_reloj_pirata(c, 0, id_pirata);
    }else{
        id_pirata -= 8;
        screen_pintar_reloj_pirata(c, 1, id_pirata);
    }
}

void screen_limpiar_reloj_pirata(uint id_pirata)
{   
    id_pirata = id_pirata - GDT_IDX_TSS_PRIMER_TAREA_JUGADOR_A + 1;
    char c = 'X';
    if(id_pirata <= 8){
        screen_pintar_reloj_pirata(c, 0, id_pirata);
    }else{
        id_pirata -= 8;
        screen_pintar_reloj_pirata(c, 1, id_pirata);
    }
}


void screen_pintar_reloj_pirata(char c, uint id_jugador, int posicion){
    int fila = 48;
    int color = fondo_negro;
    int columna = 0;
    int offset = (posicion - 1)*2;
    if(id_jugador == 0){
        color = color | texto_rojo;
        columna = 3 + offset;
    }else{
        color = color | texto_azul;
        columna = 60 + offset;
    }
    screen_pintar(c,color,fila,columna);
}

void screen_pirata_movimiento(jugador_t *jugador, uint tipo,uint x, uint y, uint x_prev, uint y_prev){

    char exp = 'E';
    if(tipo == 1){
        exp = 'M';
    }

    y += 1; //hay que correrlo uno porque la pantalla arranca un pixel abajo
    uchar color_fondo = screen_color_jugador(jugador);


    uchar color_act = C_FG_WHITE;
    if(jugador->index == 0){
        color_act = color_act | fondo_rojo;
    }else{
        color_act = color_act | fondo_azul;
    }
    screen_pintar(exp, color_act, y, x);

    int pos_prev = posiciones_exploradas[jugador->index][x_prev][y_prev];
    if (x_prev < MAPA_ANCHO && y_prev < MAPA_ALTO && pos_prev == 1){
        uchar color_prev = color_fondo | C_FG_BLACK;
        screen_pintar(exp, color_prev, y_prev + 1, x_prev);
    }else if (pos_prev == 2){
        screen_pintar_botin(jugador, x_prev, y_prev);
    }else if (pos_prev == 3){
        screen_pintar_botin_vacio(jugador, x_prev, y_prev);
    }

}

void screen_pintar_vacio(jugador_t *j, uint x, uint y){
    uchar color = screen_color_jugador(j);
    screen_pintar(' ', color, y + 1, x);
}

void screen_pintar_botin(jugador_t *j, uint x, uint y){
    uchar color = screen_color_jugador(j) | C_FG_BLACK;
    screen_pintar('o', color, y + 1, x);
}

void screen_pintar_botin_vacio(jugador_t *j, uint x, uint y){
    uchar color = screen_color_jugador(j) | C_FG_BLACK;
    screen_pintar('x', color, y + 1, x);
}


uint screen_posicion_valida(int x, int y) {
    return (x >= 0 && y >= 1 && x < MAPA_ANCHO && y - 1< MAPA_ALTO);
}

void screen_borrar_pirata(pirata_t* pirata){
    char exp = 'E';
    if(pirata->es_minero == 1){
        exp = 'M';
    }
    uchar color = screen_color_jugador(pirata->jugador) | C_FG_BLACK;
    screen_pintar(exp, color, pirata->y + 1 , pirata->x);
}

uchar screen_color_jugador(jugador_t *j){
    uchar color = C_BG_GREEN;
    if(j->index == 1){
        color = C_BG_CYAN;
    }

    return color;
}

void screen_pintar(uchar c, uchar color, uint fila, uint columna)
{
    p[fila][columna].c = c;
    p[fila][columna].a = color;
}

void screen_pintar_rect(uchar c, uchar color, int fila, int columna, int alto, int ancho)
{      
    int row, col;
    for(row = fila; row < fila + alto; row++){
        for (col = columna; col < columna + ancho; col++){
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
    
    //lugares para modo debug y pausa
    print(" ", 47 , 0, 0x47);
    print(" ", 50 , 0, 0x47);
    
    //lugares disponibles;
    int i;
    for(i = 1; i <= 8; i++){
        screen_pintar_lugar_disponible(0, i);
        screen_pintar_lugar_disponible(1, i);
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

void screen_pintar_lugar_disponible(uint id_jugador, int posicion){
    screen_pintar_reloj_pirata('X',id_jugador,posicion);
}

void screen_pintar_puntajes(){
    screen_pintar_puntaje(0,0);
    screen_pintar_puntaje(0,1);
}

void screen_pintar_puntaje(int puntaje, uint id_jugador){
    uchar color = texto_blanco;
    int start = 34;
    if (id_jugador == 0){
        color =  color | fondo_rojo;
        
    }else{
        color = color | fondo_azul;
        start = 41;
    }
    print_dec(puntaje,3, start, 47, color);
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

void screen_pintar_posicion(uint intCode, uint errorCode){
    print("X: ",0,0,fondo_rojo_letras_blancas);
    print_dec(intCode, 3, 13, 0, fondo_rojo_letras_blancas);
    print("Y: ",17,0,fondo_rojo_letras_blancas);
    print_dec(errorCode, 8, 29, 0, fondo_rojo_letras_blancas);
    breakpoint();
    
}

void screen_pintar_interrupcion(uint intCode){
    print("Interrupcion:",0,0,fondo_rojo_letras_blancas);
    print_dec(intCode, 3, 14, 0, fondo_rojo_letras_blancas);
}
void screen_pintar_ganador(uint ganador){
    char color = fondo_rojo_letras_blancas;
    if(ganador){
        color = fondo_azul | texto_blanco;
    }

    screen_pintar_linea_h(espacio, fondo_negro, 16, 25, 30);
    screen_pintar_linea_h(espacio, fondo_negro, 32, 25, 30);
    screen_pintar_linea_v(espacio, fondo_negro, 16, 25, 17);
    screen_pintar_linea_v(espacio, fondo_negro, 16, 55, 17);

    screen_pintar_rect(espacio, color, 17, 26, 15, 29);

    print("GANADOR",36,24,color);
}
void screen_pintar_tecla(uint intCode){
    if(intCode > 53){
        intCode -= 130;
        char* s = "";
        s[0] = teclado[intCode];
        print(s,0, 0, fondo_rojo_letras_blancas);
    }
}

void screen_pintar_modo_debugg(uint pila1, uint pila2, uint pila3, uint pila4, uint pila5){

    uint id_pirata_actual = rtr() >> 3;
    tss* tss_aux = (tss*)(gdt[id_pirata_actual].base_31_24 << 24 | gdt[id_pirata_actual].base_23_16 << 16 | gdt[id_pirata_actual].base_0_15);


    tss tss_entry = *tss_aux;
    uchar fondo_debug = C_BG_LIGHT_GREY;
    uchar borde_debug = fondo_negro;
    uchar letra_registro = C_BG_LIGHT_GREY | C_FG_BLACK;
    uchar letra_valor = C_BG_LIGHT_GREY | C_FG_WHITE;

    int fila, columna;
    for(fila = 0; fila < VIDEO_FILS ; fila++){
        for(columna = 0 ; columna < VIDEO_COLS ; columna++){
            ptemp[fila][columna].c = p[fila][columna].c;
            ptemp[fila][columna].a = p[fila][columna].a;
        }
    }

    //linea negra
    screen_pintar_rect(espacio, fondo_debug, 8, 26, 34, 28);

    screen_pintar_linea_h(espacio, borde_debug, 7, 25, 29);
    screen_pintar_linea_h(espacio, C_BG_RED, 8, 26, 28);
    screen_pintar_linea_h(espacio, C_BG_RED, 40, 26, 28);
    screen_pintar_linea_h(espacio, borde_debug, 41, 25, 30);
    screen_pintar_linea_v(espacio, borde_debug, 7, 25, 34);
    screen_pintar_linea_v(espacio, borde_debug, 7, 54, 34);


    print("eax", 27, 10, letra_registro);
    print_hex(tss_entry.eax, 8, 31, 10, letra_valor);

    print("ebx", 27, 12,letra_registro);
    print_hex(tss_entry.ebx, 8, 31, 12, letra_valor);

    print("ecx", 27, 14,letra_registro);
    print_hex(tss_entry.ecx, 8, 31, 14, letra_valor);

    print("edx", 27, 16,letra_registro);
    print_hex(tss_entry.edx, 8, 31, 16, letra_valor);

    print("esi", 27, 18,letra_registro);
    print_hex(tss_entry.esi, 8, 31, 18, letra_valor);

    print("edi", 27, 20,letra_registro);
    print_hex(tss_entry.edi, 8, 31, 20, letra_valor);

    print("ebp", 27, 22,letra_registro);
    print_hex(tss_entry.esp, 8, 31, 22, letra_valor);

    print("eip", 27, 24,letra_registro);
    print_hex(tss_entry.eip, 8, 31, 24, letra_valor);

    print("cs", 28, 26,letra_registro);
    print_hex(tss_entry.cs, 8, 31, 26, letra_valor);

    print("ds", 28, 28,letra_registro);
    print_hex(tss_entry.ds, 8, 31, 28, letra_valor);

    print("es", 28, 30,letra_registro);
    print_hex(tss_entry.cs, 8, 31, 30, letra_valor);

    print("fs", 28, 32,letra_registro);
    print_hex(tss_entry.fs, 8, 31, 32, letra_valor);

    print("gs", 28, 34,letra_registro);
    print_hex(tss_entry.gs, 8, 31, 34, letra_valor);

    print("ss", 28, 36,letra_registro);
    print_hex(tss_entry.ss, 8, 31, 36, letra_valor);

    print("eflags", 27, 38,letra_registro);
    print_hex(tss_entry.eflags, 8, 34, 38, letra_valor);

    print("cr0", 41, 10,letra_registro);
    print_hex(rcr0(), 8, 45, 10, letra_valor);

    print("cr2", 41, 12,letra_registro);
    print_hex(rcr2(), 8, 45, 12, letra_valor);

    print("cr3", 41, 14,letra_registro);
    print_hex(tss_entry.cr3, 8, 45, 14, letra_valor);

    print("cr4", 41, 16,letra_registro);
    print_hex(rcr4(), 8, 45, 16, letra_valor);

    print("stack", 41, 19,letra_registro);
    print_hex(pila1, 8, 41, 21, letra_valor);
    print_hex(pila2, 8, 41, 23, letra_valor);
    print_hex(pila3, 8, 41, 25, letra_valor);
    print_hex(pila4, 8, 41, 27, letra_valor);
    print_hex(pila5, 8, 41, 29, letra_valor);
}

void screen_despintar_modo_debug(){
    int fila, columna;
    for(fila = 0; fila < VIDEO_FILS ; fila++){
        for(columna = 0 ; columna < VIDEO_COLS ; columna++){
            p[fila][columna].c = ptemp[fila][columna].c;
            p[fila][columna].a = ptemp[fila][columna].a;
        }
    }
}