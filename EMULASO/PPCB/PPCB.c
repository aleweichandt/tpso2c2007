
/*
 ============================================================================
 Name        : PPCB.c
 Author      : Trimboli, Damián
 Version     : 1.0
 Copyright   : Your copyright notice
 Description : App de PPCB
 ============================================================================
 */

#ifndef PPCB_P_
#define PPCB_P_


#include "PPCB.h"

/**********************************************************/
int main(int argc, char *argv[])
{	
	if (argc == 8 || argc == 3) {
	
		if ( PCB_Init(argc, argv) == ERROR )
				return EXIT_SUCCESS;

	} else {
		
		return EXIT_FAILURE;
		
	}
	
	while ( 1 )
	{	
		/*TODO: Este codigo hay que habilitarlo en el momento de ejecutar el programa*/
		/*if( PCB.nIdProcesoPadre == _ID_ACR_ )
		{
			PCB.m_socketACR->onTimeOut= PCB_ExecuteProgram;
			PCB.m_socketACR->segundos_timeout = ALRM_T;
		}else{
			PCB.m_socketADP->onTimeOut= PCB_ExecuteProgram;
			PCB.m_socketADP->segundos_timeout = ALRM_T;
		}*/	
		conexiones_aguardarConexion( PCB.m_ListaSockets, 
	 								&(PCB.m_ultimoSocket) );
	}
	
	return EXIT_SUCCESS;	
}



#endif /*PPCB_P_*/
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/

