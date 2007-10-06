#ifndef LIBCONF_H_
#define LIBCONF_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Constantes globales */
#define INDICE_NOT_FOUND -1   /* DEBE SER < 0 PARA NO CONFUNDIR POR UN iNDICE */
#define MAXLEN 300 /*04-05-07: LAS: Con esto debe bastar levantar paths largos!*/

/* Estructuras */
typedef struct{
	char nombre[MAXLEN];
	char valor[MAXLEN];
	char seccion[MAXLEN];
}nLlave;

typedef struct{
	nLlave** llaves;
	int	 cantidadLlaves;
}tConfig;

/* Prototipos */
tConfig* config_Crear		(const char*, const char*);
tConfig* config_CrearDeBuff	(char* buffer, const char *seccion, int len);
tConfig* config_SetVal		(tConfig*, const char*, const char*, const char*);
void 	 config_DelVal		(tConfig*, const char*, const char*);
char*	 config_GetVal		(tConfig*, const char*, const char*);
int 	 config_GetVal_Int	(tConfig*, const char*, char*);
void	 config_Print		(tConfig*, FILE*);
void	 config_Guardar		(tConfig*, const char*);
void 	 config_Destroy 	(tConfig *);
	
#endif /*LIBCONF_H_*/
