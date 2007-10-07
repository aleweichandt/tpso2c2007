/*
  *<<---------------- BATMAN & ROUND ROBIN ----------------------------->>
 * Archivo: ACR.c
 * 
 * Historia
 * 23/09/2007	GT	Creacion
 *
 ------------------------------------------------------------------ 
 */

 #include "ACR.h"

/**********************************************************/
int main(int argc, char *argv[])
{
	if ( ACR_Init() == ERROR )
		return EXIT_SUCCESS;
	
	while ( 1 )
	{		
	 	conexiones_aguardarConexion( ACR.t_ListaSockets, 
	 								&(ACR.ui_ultimoSocket) );
	 								
		ACR_LeerConfigRuntime();
	}
	
	return EXIT_SUCCESS;	
}


