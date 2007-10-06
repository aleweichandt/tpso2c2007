/*
  *<<---------------- BATMAN & ROUND ROBIN ----------------------------->>
 * Archivo: ADS.c
 * 
 * Historia
 *	10-09-2007	LAS	Creacion
 *
 ------------------------------------------------------------------ 
 */

#ifndef ADS_C_
#define ADS_C_

#include "ADS.h"

/**********************************************************/
int main(int argc, char *argv[])
{
	if ( ADS_Init() == ERROR )
		return EXIT_SUCCESS;
	
	while ( 1 )
	{		
	 	conexiones_aguardarConexion( ADS.m_ListaSockets, 
	 								&(ADS.m_ultimoSocket) );
	}
	
	return EXIT_SUCCESS;	
}


#endif /*ADS_C_*/
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
