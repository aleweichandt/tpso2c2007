/*
  *<<---------------- BATMAN & ROUND ROBIN ----------------------------->>
 * Archivo: MShell.c
 * 
 * Historia
 *	03-09-2007	LAS	Creacion
 *
 ------------------------------------------------------------------ 
 */

#include "MShellLib.h"


/**********************************************************/
int main(int argc, char *argv[])
{
	if ( MSH_Init() == ERROR )
		return EXIT_SUCCESS;
	
	while ( 1 )
	{		
		ventana_SetFocus( MShell.m_pwLocal );
	 	fflush(stdout); 
		
	 	conexiones_aguardarConexion( MShell.m_ListaSockets, 
	 								&(MShell.m_ultimoSocket) );
	}
	
	return EXIT_SUCCESS;	
}


/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
