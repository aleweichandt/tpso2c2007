/*
  *<<---------------- BATMAN & ROUND ROBIN ----------------------------->>
 * Archivo: DatosPCBs.h
 * 
 * Historia
 *	18-09-2007	LAS	Creacion
 *
 ------------------------------------------------------------------ 
 */

#ifndef DATOSPCBS_H_
#define DATOSPCBS_H_

#include "../incGeneral/incGeneral.h" 
#include "../incGeneral/Estructuras/estructuras.h"
#include "../incGeneral/Sockets/conexiones.h"

typedef struct
{
	char IP[LEN_IP];
	char Registro[32];
	long id;
	unsigned short int Port;
	tSocket	*pSocket;
	void* extra;
} tunPCB;

#define tListaPCB t_nodo*


/* funciones de pcb */
tunPCB* 	pcb_Crear( 	char IP[LEN_IP], char Registro[32],	long id, unsigned short int Port,
					void* extra, tSocket *pSocket );
void 		pcb_Eliminar( tunPCB *pcb );

/* funciones de las listas de pcbs */
int 		lpcb_AgregarALista( tListaPCB* lista, tunPCB* pcb );
tunPCB* 	lpcb_PasarDeLista( tListaPCB* listaO, tListaPCB* listaD, long id );
int 		lpcb_EliminarDeLista( tListaPCB* lista, long id );
void 		lpcb_LimpiarLista( tListaPCB *lista );
tunPCB* 	lpcb_Datos( tListaPCB lista );
tListaPCB 	lpcb_Siguiente( tListaPCB lista );
tunPCB* 	lpcb_ObtenerPCBXSock( tListaPCB* lista, tSocket* sock ); 

#endif /*DATOSPCBS_H_*/
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
