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
int compararPCBXQVencido( const void *t1, const void *t2 );
int siempre( const void *t1, const void *t2 );

/*******************************************************/
int siempre( const void *t1, const void *t2 )
{
	return 0;
}

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
/***********************************************************************************/
int compararPCBXQVencido( const void *t1, const void *t2 )
/*No uso el t2!*/
{
	tunPCB	*pT1 = (tunPCB*) t1;
						
	if ( (pT1->Q) <= 0 )
		return 0;
	
	return 1;	
}

/*----------------------------------------------------------------------------------*/
 

/****************************************************************************************/
tunPCB* pcb_Crear( 	char IP[LEN_IP], char Registro[32],	long id, unsigned short int Port,
					void* extra,tSocket *pSocket, int Q, int MemoriaRequerida, long pid )
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
	newpcb->Q = Q;
	newpcb->MemoriaRequerida = MemoriaRequerida;
	newpcb->pid = pid;
	
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
	newpcb->Q = pcb->Q;
	newpcb->MemoriaRequerida = pcb->MemoriaRequerida;
	newpcb->pid = pcb->pid;
	
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
 /*************************************************************************************************/
int	lpcb_DecrementarQ( tListaPCB *pLista )
/* Decrementa los Q de cada uno y devuelve la cantidad de los que se vencieron*/
{
	int 				nCont = 0;
	tunPCB				*pPCB;
	tListaPCB			Lista = *pLista;
	
	while( Lista )
	{
		pPCB = lpcb_Datos( Lista );
		
		if ( (--(pPCB->Q)) <= 0 )
			nCont++;
		
		Lista = lpcb_Siguiente( Lista );
	}
	
	return nCont;
}

/*************************************************************************************************/
int	lpcb_ActualizarQ( tListaPCB *pLista, int nQ )
/* Actualizo los Q de cada uno y devuelve la cantidad*/
{
	int 				nCont = 0;
	tunPCB				*pPCB;
	tListaPCB			Lista = *pLista;
	
	while( Lista )
	{
		pPCB = lpcb_Datos( Lista );
		
		pPCB->Q = nQ;
		nCont++;
		
		Lista = lpcb_Siguiente( Lista );
	}
	
	return nCont;
}

/*************************************************************************************************/
int	lpcb_PasarDeLTPaLTL( tListaPCB *pListaLTP, tListaPCB *pListaLTL, int* pnMemDisp )
/*Esta funcion no es optima!
 * 1) Busco la T que tenga el Q vencido
 * 2) Creo una nueva con los datos de esta, cambiando el estado
 * 3) la quito de la LTP
 * 4) la agrego a la LTL
 * 5) Vuelvo al 1. Si no hay sale.
 * 
 * Devuelvo la cantidad que movi o -1 por error!
 * */
{
	int		nCont;
	tunPCB	*pPCBAPasar,
			*pPCBNuevo;
	/*No uso el 2do param porque en la comparacion hago Q <= 0*/
    tListaPCB tmp = lista_buscar( pListaLTP, NULL, &compararPCBXQVencido );
    nCont = 0;
    
	while ( tmp  )
	{
		pPCBAPasar = lpcb_Datos( tmp );
		
		*pnMemDisp += pPCBAPasar->MemoriaRequerida; /*Devuelvo la mem, gracias!*/
		
		if( !(pPCBNuevo = pcb_Crear2( pPCBAPasar ) ) )
			return ERROR;
		
		if ( !lpcb_EliminarDeLista( pListaLTP, pPCBAPasar->id ) )
			return ERROR;
		
		if ( !lpcb_AgregarALista( pListaLTL, pPCBNuevo ) )
			return ERROR;
		
		nCont++;
		tmp = lista_buscar( pListaLTP, NULL, &compararPCBXQVencido );
	}

	return nCont;
}
 
 /*******************************************************************************************/
 void lpcb_MatarPCBs( tListaPCB *pLista )
 {
	tListaPCB 	Lista = *pLista;
	tunPCB		*ppcb = NULL;
	
	while( Lista )
	{
		ppcb = lpcb_Datos( Lista );
		
		/*Elimino el proceso PPCB*/
		kill( ppcb->pid, SIGTERM );
		
		Lista = lpcb_Siguiente( Lista );
	}
	
	lpcb_LimpiarLista( pLista );
}
 
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
