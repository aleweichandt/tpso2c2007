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
int 		PpcbAcr_AgregarPpcb	(tListaPpcbAcr *lista, const long pid)
{
	int pos;
	tPpcbAcr* transfAde = malloc(sizeof(tPpcbAcr));
	transfAde->pid = pid;
	lista_insertar(lista, transfAde, sizeof(tPpcbAcr), &comparaPpcbAcr, _SIN_REPET_ );
	PpcbAcr_BuscarTrans(lista,pid,&pos);
	return pos;
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
tPpcbAcr*	PpcbAcr_BuscarTrans	(tListaPpcbAcr *lista, const long pid, int *pos)
{
	unsigned int cantBytes;
	tListaPpcbAcr listaAux = *lista;
	tPpcbAcr transfBuscada ;
	tPpcbAcr *transfAde = NULL;
	
	transfBuscada.pid = pid;
	
	*pos = 0;
	while(listaAux)
	{
		transfAde = (tPpcbAcr*)nodo_datos(*lista,&cantBytes);
		if(comparaPpcbAcr ( (void*)&transfBuscada, (void*)transfAde ) == 0)
			return transfAde; 
		
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
int comparaPpcbAcr( const void *ppcb1, const void *ppcb2 )
{
	tPpcbAcr *ppcbAcr1 = (tPpcbAcr*) ppcb1,
			   *ppcbAcr2 = (tPpcbAcr*) ppcb2;
			   
	return (int)(ppcbAcr1->pid - ppcbAcr2->pid); 
}
