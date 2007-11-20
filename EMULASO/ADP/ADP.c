/*
  *<<---------------- BATMAN & ROUND ROBIN ----------------------------->>
 * Archivo: ADP.c
 * 
 * Historia
 *	10-09-2007	LAS	Creacion
 *
 ------------------------------------------------------------------ 
 */
#ifndef ADS_P_
#define ADS_P_

#include "ADP.h"

/**********************************************************/
int main(int argc, char *argv[])
{
/*	TestPlanificacion();*/
	
	if ( ADP_Init() == ERROR )
		return EXIT_SUCCESS;
	
	while ( 1 )
	{
		ADP.m_ListaSockets[SOCK_ESCUCHA]->onTimeOut = &ADP_TimeOut;
		ADP.m_ListaSockets[SOCK_ESCUCHA]->segundos_timeout = 1;
		
	 	conexiones_aguardarConexion( ADP.m_ListaSockets, 
	 								&(ADP.m_ultimoSocket) );
	}
	
	return EXIT_SUCCESS;	
}


#endif /*ADS_P_*/
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
