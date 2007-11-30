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
/*****************************************************************/
/*char* AplicarXorEnString(char *szStringOrigen, int clave)
{
	int i;
	if(clave == -1)
	{
		return szStringOrigen;
	}
	for ( i = 0; i < PAQUETE_MAX_TAM; ++i) 
	{
		szStringOrigen[i] ^= clave;
	}
	return szStringOrigen;
}*/

/*******************************************************************************************/
/*	Reduce un IP dado de la forma "192.168.0.1" a un char[4] = {192,0,0,1} 
 * 	Precondiciones: szIP debe ser de la forma "#.#.#.#" y szIPReducido debe ser un char[4]*/
int ReducirIP( const char* szIP, unsigned char* szIPReducido )
{
	char 	szAux[ LEN_IP ];/*Como strtok modifca la cadena uso un char auxiliar*/
	char* 	pNro;
	int		indice = 0;
	
	strncpy( szAux, szIP, LEN_IP );
	
	pNro = strtok( szAux, "." );
	
	while ( pNro )
	{
		szIPReducido[ indice++ ] = (unsigned char) atoi( pNro );
		
		pNro = strtok( NULL, ".");
	}

	if ( indice == 4 )
		return OK; /*Exitosa si cargo los cuatro valores!*/
	
	return ERROR;	
} 

/*Funcion inversa a la de REducirIP!*/
int AmpliarIP( const unsigned char* szIPReducido, char* szIP )
/**/
{
	sprintf( szIP, "%d.%d.%d.%d", 	(int) szIPReducido[0], 
				(int) szIPReducido[1], (int) szIPReducido[2], 
				(int) szIPReducido[3] );
	return OK;
}

/******************************************************************/
void ArmarPathPCBConfig( char* szPathOut, long lpcb_id, int size )
/*02-10-07:LAS:*/
{
	char			szNombreArch[50];
	memset( szNombreArch, 0, 50 );/*por las dudas*/

	/*Se puede agregar aca la comprobacion de la carpeta de los pcbs*/	
	getcwd( szPathOut, size );/*esto no estaba bien: sizeof (szNombreArch)*/
	sprintf( szNombreArch, "/config.ppcb%ld", lpcb_id );
	strcat( szPathOut, szNombreArch );
}

/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
