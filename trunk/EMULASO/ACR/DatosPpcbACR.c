/*
  *<<---------------- BATMAN & ROUND ROBIN ----------------------------->>
 * Archivo: DatosPpcbACR.c
 * 
 * Historia
 *	23/09/2007	GT	Creación
 ------------------------------------------------------------------ 
 */

#include "DatosPpcbACR.h"

/*********************************************************/
void		PpcbAcr_IniciarLista	(tListaPpcbAcr lista)
{
	lista_inic(&lista);
}

/*********************************************************/
int 		PpcbAcr_AgregarPpcb	(tListaPpcbAcr *lista, tPpcbAcr *ppcb )
{
	int pos;

	if ( !ppcb ) 
		return (int)NULL;
	
	if (!*lista) 
		lista_inic( lista );	

	/*Si no lo encuentra lo agrega*/
	if ( !lista_buscar( lista, ppcb, &comparaPpcbAcr ) )
		lista_insertar( lista, ppcb, sizeof(tPpcbAcr), &comparaPpcbAcrTime, 1 );
	
	/*PpcbAcr_BuscarPpcb(lista,ppcb->pid,&pos);*/
	return 1;
}

/*********************************************************/
void	PpcbAcr_EliminarPpcb(tListaPpcbAcr *lista, const long pid)
{
	int pos;
	tPpcbAcr transfBuscada;
	transfBuscada.pid = pid;
	lista_quitar(lista,&transfBuscada,comparaPpcbAcr);
	
}

/*********************************************************/
void PpcbAcr_EliminarTodas(tListaPpcbAcr *lista)
{
	lista_destruir(lista);
}

/*********************************************************/
tPpcbAcr*	PpcbAcr_BuscarPpcb	(tListaPpcbAcr *lista, const long pid, int *pos)
{
	unsigned int cantBytes;
	tListaPpcbAcr listaAux = *lista;
	tPpcbAcr transfBuscada ;
	tPpcbAcr *transfAde = NULL;
	
	transfBuscada.pid = pid;
	
	*pos = 0;
	while(listaAux)
	{
		transfAde = (tPpcbAcr*)nodo_datos(listaAux,&cantBytes);
		if(comparaPpcbAcr ( (void*)&transfBuscada, (void*)transfAde ) == 0)
			return transfAde; 

		listaAux = nodo_sgte( listaAux );		
		++*pos;
	}
	return NULL;
}

/*********************************************************/
tPpcbAcr*	PpcbAcr_Datos	(tListaPpcbAcr lista)
{
	unsigned int alpe;
	return nodo_datos ( lista, &alpe );
}

/*********************************************************/
tListaPpcbAcr	PpcbAcr_Siguiente	(tListaPpcbAcr lista)
{
	return nodo_sgte( lista );
}

/*********************************************************/
tPpcbAcr* PpcbAcr_ObtenerPpcbXSock( tListaPpcbAcr *lstppcb, tPpcbAcr* ppcbSockIn )
{
	tListaPpcbAcr tmp = lista_buscar( lstppcb, ppcbSockIn, &compararPpcbXSock );
	return (tmp? tmp->datos : NULL);
}

tPpcbAcr* PpcbAcr_ObtenerPpcbXPid( tListaPpcbAcr *lstppcb, tPpcbAcr* ppcbPid )
{
	tListaPpcbAcr tmp = lista_buscar( lstppcb, ppcbPid, &comparaPpcbAcr );
	return (tmp? tmp->datos : NULL);
}

void PpcbAcr_ReordenarLista(tListaPpcbAcr *lista){
	lista_reordenar(lista,comparaPpcbAcrTime);
}

/*********************************************************/
int comparaPpcbAcr( const void *ppcb1, const void *ppcb2 )
{
	tPpcbAcr *ppcbAcr1 = (tPpcbAcr*) ppcb1,
			   *ppcbAcr2 = (tPpcbAcr*) ppcb2;
			   
	return (int)(ppcbAcr1->pid - ppcbAcr2->pid); 
}

int comparaPpcbAcrTime( const void *ppcb1, const void *ppcb2 )
{
	tPpcbAcr *ppcbAcr1 = (tPpcbAcr*) ppcb1,
			   *ppcbAcr2 = (tPpcbAcr*) ppcb2;
			   
	return (int)(ppcbAcr1->sFechaInactvdad - ppcbAcr2->sFechaInactvdad); 
}

/*********************************************************/
int compararPpcbXSock( const void *ppcb1, const void *ppcb2 )
{
	tPpcbAcr	*ppcbAcr1 = (tPpcbAcr*) ppcb1,
				*ppcbAcr2 = (tPpcbAcr*) ppcb2;
						
	if ( ppcbAcr1->socket == ppcbAcr2->socket )
	{
		return 0;
	}
	
	return 1;	
}

