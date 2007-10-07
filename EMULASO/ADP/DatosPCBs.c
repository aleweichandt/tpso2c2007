/*
  *<<---------------- BATMAN & ROUND ROBIN ----------------------------->>
 * Archivo: DatosPCBs.h
 * 
 * Historia
 *	18-09-2007	LAS	Creacion
 *
 ------------------------------------------------------------------ 
 */
 
 #include "DatosPCBs.h"
 
 /*----------------------------------------------------------------------------------*/
int compararPCBxId( const void *t1, const void *t2 );
int compararPCBXSock( const void *t1, const void *t2 );

/***********************************************************************************/
int compararPCBxId( const void *t1, const void *t2 )
{
    tunPCB  *pcb1 = (tunPCB*) t1,
            *pcb2 = (tunPCB*) t2;

    if ( pcb1->id == pcb2->id )
    {
        return 0;
    }
   
    return 1;   
}

/***********************************************************************************/
int compararPCBXSock( const void *t1, const void *t2 )
{
	tunPCB 	*pcb1 = (tunPCB*) t1,
			*pcb2 = (tunPCB*) t2;
						
	if ( pcb1->pSocket == pcb2->pSocket )
	{
		return 0;
	}
	
	return 1;	
}

/*----------------------------------------------------------------------------------*/
 

/****************************************************************************************/
tunPCB* pcb_Crear( 	char IP[LEN_IP], char Registro[32],	long id, unsigned short int Port,
					void* extra,tSocket *pSocket )
{
	tunPCB *newpcb;
	
	if ( !(newpcb = malloc( sizeof(tunPCB) ) ) )
		return NULL;
		
	strcpy( newpcb->IP, IP );
	strcpy( newpcb->Registro, Registro );
	newpcb->id = id;
	newpcb->Port = Port;
	newpcb->extra = extra;
	newpcb->pSocket = pSocket;
	
	return newpcb;
}

/****************************************************************************************/
tunPCB* pcb_Crear2( tunPCB *pcb )
{
	tunPCB *newpcb;
	
	if ( !(newpcb = malloc( sizeof(tunPCB) ) ) )
		return NULL;
		
	strcpy( newpcb->IP, pcb->IP );
	strcpy( newpcb->Registro, pcb->Registro );
	newpcb->id = pcb->id;
	newpcb->Port = pcb->Port;
	newpcb->extra = pcb->extra;
	newpcb->pSocket = pcb->pSocket;
	
	return newpcb;
}

/****************************************************************************************/
void pcb_Eliminar( tunPCB *pcb )
{
	free( pcb );
}


/* funciones de las listas de pcbs */
/****************************************************************************************/
int lpcb_AgregarALista( tListaPCB* lista, tunPCB* pcb )
{
	if ( !pcb ) 
		return ERROR;
	
	if ( !*lista ) 
		lista_inic( lista );	

	/*Si no lo encuentra lo agrega*/
	if ( !lista_buscar( lista, pcb, &compararPCBxId ) )
	{
		if( lista_insertar( lista, pcb, sizeof(tunPCB), &compararPCBxId, 1 ) )
			return OK;
	}
	
	return ERROR;	
	
}

/****************************************************************************************/
tunPCB* lpcb_PasarDeLista( tListaPCB* listaO, tListaPCB* listaD, long id )
{
	tunPCB	*pPCBAPasar,
			*pPCBNuevo;
	tunPCB pcb;
	
	pcb.id = id;
    
    tListaPCB tmp = lista_buscar( listaO, &pcb, &compararPCBxId );
    
	if ( tmp )
	{
		pPCBAPasar = lpcb_Datos( tmp );
		
		if( !(pPCBNuevo = pcb_Crear2( pPCBAPasar ) ) )
			return NULL;
		
		if ( !lpcb_EliminarDeLista( listaO, pPCBAPasar->id ) )
			return NULL;
		
		if ( lpcb_AgregarALista( listaD, pPCBNuevo ) == ERROR )
			return NULL;
	}

	return pPCBNuevo;
	
}

/****************************************************************************************/
int	lpcb_EliminarDeLista( tListaPCB* lista, long id )
{
	tunPCB pcb;
	pcb.id = id;
	
	return lista_quitar( lista, &pcb, &compararPCBxId );
}

/****************************************************************************************/
tunPCB* lpcb_Datos( tListaPCB lista )
{
	unsigned int alpe;
	return nodo_datos ( lista, &alpe );
}


/****************************************************************************************/
tListaPCB lpcb_Siguiente( tListaPCB lista )
{
	return nodo_sgte( lista );
}

/****************************************************************************************/
tunPCB* lpcb_ObtenerPCBXSock( tListaPCB *lista, tSocket* sock )
{
	tListaPCB tmp = lista_buscar( lista, sock, &compararPCBXSock );
	return ( tmp? tmp->datos : NULL );
} 
 
/*************************************************************************************************/
void lpcb_LimpiarLista( tListaPCB *lista )
/**/
{
	lista_destruir( lista );	
}
 
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
