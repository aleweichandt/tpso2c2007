/*
  *<<---------------- BATMAN & ROUND ROBIN ----------------------------->>
 * Archivo: incGeneral.c
 * 
 * Historia
 *	16-04-2007	LAS	
 *
 ------------------------------------------------------------------ 
 */


#include "incGeneral.h"

#define BUFF_GET_CAD 40

/* Devuelve una cadena a partir de un flujo, reservando el espacio necesario */
int getCadT(FILE *flujo, int fin, char *cad)
{
	int c, pos = 0; int tam = 0;

	while ((fin != (c = fgetc(flujo))) && c != EOF)
	{
		if (pos == _LARGO_CMD_-2)
		{
			break;
			/*
			tam += BUFF_GET_CAD;
			*cad = realloc(*cad, (tam + 1) * sizeof(char));
			*/
		}

		cad[pos++] = (char)c;
	}
	/**cad = realloc(*cad, (pos+1)*sizeof(char));*/
	cad[pos] = '\0';

	return pos;
}


/*****************************************************************/
int ContarCharEnString( const char *szString, char cCharAContar )
{
	int nRetorno = 0,
		i;
	
	for ( i = 0; i < strlen(szString); i++ )
		if ( szString[i] == cCharAContar )
			nRetorno++;
	
	return nRetorno;
}
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
