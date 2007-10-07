#ifndef VENTANAS_H_
#define VENTANAS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "incGeneral.h"

/* Constantes globales */
#define PANTALLA_COLS 79
#define PANTALLA_ROWS 23

/* Estructuras publicas */
typedef struct{
	int x, y, ancho, alto;
	int lineas;
	char **texto;
	char printBottomUp; /* El texto se imprime de arriba hacia abajo? */
}tVentana;

/* Prototipos publicos */
void pantalla_GoTo	(int x, int y);
void pantalla_Clear	();

void dibujar_LineaVertical		(int x, int y, int largo);
void dibujar_LineaHorizontal	(int x, int y, int largo);
void dibujar_Rectangulo			(int x, int y, int ancho, int alto);

tVentana*	ventana_Crear		(int x, int y, int ancho, int alto, char printBottomUp, const char* titulo);
int			ventana_GetAltoReal	(tVentana* wnd);
int			ventana_GetAnchoReal(tVentana* wnd);
void 		ventana_Destruir	(tVentana* wnd);
void		ventana_ReDibujar	(const tVentana *wnd, const char* titulo);
void		ventana_SetFocus	(const tVentana* wnd);
void		ventana_Print		(const tVentana* wnd, const char* msg);
void 		ventana_Printf		(const tVentana* wnd, const char* fmt, ...);
void		ventana_PrintAt		(const tVentana* wnd, const char* msg, unsigned int y);
void		ventana_Clear		(const tVentana* wnd);

#endif /*VENTANAS_H_*/
