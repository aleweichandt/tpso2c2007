
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
	if (argc == 8 || argc == 2) {
	
		if ( PCB_Init(argc, argv) == ERROR )
				return EXIT_SUCCESS;

	} else {
		
		return EXIT_FAILURE;
		
	}
	
	while ( 1 )
	{		
	
		conexiones_aguardarConexion( PCB.m_ListaSockets, 
	 								&(PCB.m_ultimoSocket) );
	}
	
	return EXIT_SUCCESS;	
}



#endif /*PPCB_P_*/
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/

