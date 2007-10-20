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
#include <signal.h>

typedef struct
{
	char IP[LEN_IP];
	char Registro[32];
	long id;
	long pid;/*proccess id*/
	int  Q;
	int	 MemoriaRequerida;
	unsigned short int Port;
	tSocket	*pSocket;
	void* extra;
} tunPCB;

#define tListaPCB t_nodo*


/* funciones de pcb */
tunPCB* 	pcb_Crear( 	char IP[LEN_IP], char Registro[32],	long id, unsigned short int Port,
					void* extra, tSocket *pSocket, int Q, int MemoriaRequerida, long pid );
tunPCB* 	pcb_Crear2( tunPCB *pcb );					
void 		pcb_Eliminar( tunPCB *pcb );

/* funciones de las listas de pcbs */
int 		lpcb_AgregarALista( tListaPCB* lista, tunPCB* pcb );
tunPCB* 	lpcb_PasarDeLista( tListaPCB* listaO, tListaPCB* listaD, long id );
int 		lpcb_EliminarDeLista( tListaPCB* lista, long id );
void 		lpcb_LimpiarLista( tListaPCB *lista );
tunPCB* 	lpcb_Datos( tListaPCB lista );
tListaPCB 	lpcb_Siguiente( tListaPCB lista );
tunPCB* 	lpcb_ObtenerPCBXSock( tListaPCB* lista, tSocket* sock ); 
int			lpcb_DecrementarQ( tListaPCB *pLista );
int			lpcb_ActualizarQ( tListaPCB *pLista, int nQ );
int			lpcb_PasarDeLTPaLTL( tListaPCB *pListaLTP, tListaPCB *pListaLTL, int* pnMemDisp  );
void 		lpcb_MatarPCBs( tListaPCB *pLista );

#endif /*DATOSPCBS_H_*/
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
