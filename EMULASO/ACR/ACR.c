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
	time_t antes = time(NULL);
	if ( ACR_Init() == ERROR )
		return EXIT_SUCCESS;
	
	while ( 1 )
	{	
		ACR_PonerTimer(&antes);
	 	conexiones_aguardarConexion( ACR.t_ListaSockets, 
	 								&(ACR.ui_ultimoSocket) );
		/*ACR_SacarTimer();
	 	ACR_ControlarPendientes(); TODO: al sacar ver si funciona todo porque tenía problemas de starvation
	 	ACR_PonerTimer();*/
		ACR_LeerConfigRuntime();
	}
	
	return EXIT_SUCCESS;	
}


