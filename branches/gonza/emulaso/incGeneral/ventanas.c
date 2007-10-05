#include "ventanas.h"

/************************************************************************\
 * 							FUNCIONES DE PANTALLA						*
\************************************************************************/

#ifdef COMP_SOLO_ANSI	/* Si solo vamos a compilar ANSI, no se puede usar \e */
	void pantalla_GoTo(int x, int y)
	{
		return;
	}
	void pantalla_Clear()
	{
		return;	
	}
#else
	void pantalla_GoTo(int x, int y)
	{
		printf("\e[%d;%dH", y, x);
	}
	void pantalla_Clear()
	{
		printf("\e[2J");
		pantalla_GoTo(0, 0);	
	}
#endif


/************************************************************************\
 * 							FUNCIONES DE DIBUJO							*
\************************************************************************/
void dibujar_LineaVertical(int x, int y, int largo)
{
	while (largo)
	{
		pantalla_GoTo(x, y+largo);
		printf("|");
		--largo;
	}
}
void dibujar_LineaHorizontal(int x, int y, int largo)
{
	while (largo)
	{
		pantalla_GoTo(x+largo, y);
		printf("=");
		--largo;
	}
}
void dibujar_Rectangulo(int x, int y, int ancho, int alto)
{
	dibujar_LineaVertical(x, y-1, alto+1);
	dibujar_LineaVertical(x+ancho, y-1, alto+1);
	dibujar_LineaHorizontal(x, y, ancho-1);
	dibujar_LineaHorizontal(x, y+alto, ancho-1);
}

/************************************************************************\
 * 							FUNCIONES DE VENTANAS						*
\************************************************************************/
#define DY wnd->y+wnd->alto-wnd->lineas-1
#define DX 1+wnd->x
#define WORDWRAP 10

void ventana_ReDibujar(const tVentana *wnd, const char* titulo)
{
	if (!wnd) return;
	dibujar_Rectangulo(wnd->x, wnd->y, wnd->ancho, wnd->alto);
	dibujar_LineaHorizontal(wnd->x, wnd->y+2, wnd->ancho-1);
	pantalla_GoTo(wnd->x+1, wnd->y+1);
	printf("%s", titulo);
}

void ventana_SetFocus(const tVentana* wnd)
{
	if (!wnd) return;
	pantalla_GoTo(1+wnd->x, 3+wnd->y);
}

tVentana* ventana_Crear(int x, int y, int ancho, int alto, char printBottomUp, const char* titulo)
{
	tVentana *tmp; int i;

	if (ancho < 5 || alto < 5) return NULL;

	tmp = malloc(sizeof(tVentana));
	if (!tmp) return NULL;
	tmp->x = x;
	tmp->y = y;
	tmp->ancho = ancho;
	tmp->alto = alto;
	tmp->printBottomUp = printBottomUp;
	tmp->lineas = (alto-4);	/* 3 lineas de titulo, 1 de borde inferior */

	tmp->texto = malloc((1+tmp->lineas) * sizeof(char*));
	bzero(tmp->texto, (1+tmp->lineas) * sizeof(char*));

	if (!tmp->texto){
		free(tmp);
		return NULL;
	}
	for (i=0; i <= tmp->lineas; ++i)
	{
		tmp->texto[i] = malloc((1+ancho-2) * sizeof(char));
		bzero(tmp->texto[i], (1+ancho-2) * sizeof(char));
	}

	ventana_ReDibujar(tmp, titulo);
	ventana_SetFocus(tmp);

	return tmp;
}

int	ventana_GetAltoReal(tVentana* wnd)
{
	if (!wnd) return 0;
	return wnd->lineas+1;
}

int	ventana_GetAnchoReal(tVentana* wnd)
{
	if (!wnd) return 0;
	return wnd->ancho-2;
}

void ventana_Destruir(tVentana* wnd)
{
	int i;
	if (!wnd) return;

	for (i=0; i <= wnd->lineas; ++i)
		free(wnd->texto[i]);

	free(wnd->texto);
	free(wnd);
}

void ventana_PrintAt(const tVentana* wnd, const char* msg, unsigned int y)
{
	int i = 0;
	if (!wnd){
		printf("%s\n", msg);
		return;
	}
	if (y > wnd->lineas) return;

	/* strncpy(wnd->texto[y], msg, wnd->ancho-2); */
	while (*msg && *msg != '\n' && i < wnd->ancho-2)
		wnd->texto[y][i++] = *(msg++);

	/* relleno con espacios lo que queda, para limpiar la linea */
	while (i < wnd->ancho-2)
		wnd->texto[y][i++] = ' ';

	wnd->texto[y][wnd->ancho-2] = '\0';

	if (wnd->printBottomUp)
		pantalla_GoTo(DX, DY+wnd->lineas-y);
	else
		pantalla_GoTo(DX, DY+y-1);

	printf("%s\n", wnd->texto[y]);
}

void ventana_Printf(const tVentana* wnd, const char* fmt, ...)
{
	va_list args;	
	char buffer[500];
	
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);
	va_end(args);
	ventana_Print(wnd, buffer);
}

void ventana_Print(const tVentana* wnd, const char* msg)
{
	char* tmp;
	int j, i;

	if (!wnd){
		printf("%s\n", msg);	/* Si no hay modo grafico... */
		return;
	}

	i = wnd->lineas;
	while (i)
	{
		tmp = wnd->texto[i];
		wnd->texto[i] = wnd->texto[i-1];
		wnd->texto[i-1] = tmp;

		/* Direccion de impresion. No mezclar! (O si?) */
		if (wnd->printBottomUp)
			pantalla_GoTo(DX, DY+wnd->lineas-i);
		else
			pantalla_GoTo(DX, DY+i-1);

		printf("%s\n", wnd->texto[i]);
		--i;
	}

	if (wnd->printBottomUp)
		pantalla_GoTo(DX, DY+wnd->lineas);
	else
		pantalla_GoTo(DX, DY);

	/* strncpy */
	i = 0;
	while (*msg && *msg != '\n' && i < wnd->ancho-2)
		wnd->texto[0][i++] = *(msg++);

	/* relleno con espacios lo que queda, para limpiar la linea */
	while (i < wnd->ancho-2)
		wnd->texto[0][i++] = ' ';

	/* Si corto la linea en medio de una palabra, voy para atras */
	if(*msg && *msg != '\n' && *msg != ' '){
		j = 0;
		do{
			msg--;
			wnd->texto[0][i-(j++)] = ' ';
		}while (*msg != ' ' && j < WORDWRAP);	/* Si len(palabra) > WORDWRAP, cortarla */

	}

	if (*msg && (*msg == '\n' || *msg == ' ')){
		msg++;
	}
	
	wnd->texto[0][wnd->ancho-2] = '\0';
	if (!*msg){
		printf("%s\n", wnd->texto[0]);
	}else{
		ventana_Print(wnd, msg);	/* msg fue desplazado hasta strlen */
	}
}

void ventana_Clear(const tVentana* wnd)
{
	int i; char *msg = "";

	if (!wnd) return;
	for (i=0; i <= wnd->lineas; ++i)
		ventana_Print(wnd, msg);
}

/***********************/
