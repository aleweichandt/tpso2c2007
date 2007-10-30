#include "DatosAdpACR.h"

/*******************************************************/
void DatosAdpACR_Ini( tListaAdps* lista )
{
	lista->cantidad=0;
	lista->lista = NULL;
}

/*******************************************************/
void DatosAdpACR_Agregar( tListaAdps* lista, tSocket *adp )
{
	
	lista->lista = realloc(lista->lista, (++(lista->cantidad))*sizeof(tSocket*));
	lista->lista[lista->cantidad - 1] = adp;

}

/*******************************************************/
tSocket* DatosAdpACR_Obtener( tListaAdps* lista_res, int indice )
{

	if (!lista_res || lista_res->cantidad == 0 || lista_res->cantidad <= indice ) return NULL;
	
	return lista_res->lista[indice];
}

/*******************************************************/
void DatosAdpACR_EliminarDeLaLista( tListaAdps* lista_res, tSocket* adp )
{
	int i = 0;
	tSocket* sockAux = DatosAdpACR_Obtener(lista_res,i);
	
	while(sockAux){
		if( adp == sockAux ){
			for (; i < lista_res->cantidad-1; ++i) 
				lista_res->lista[i] = lista_res->lista[i+1];
			lista_res->cantidad--;
			return;
		}
		++i;
		sockAux = DatosAdpACR_Obtener(lista_res,i);
	}
	
}

/*******************************************************/
void DatosAdpACR_EliminarLista( tListaAdps* lista_res )
{
	if (!lista_res) return;
	free(lista_res->lista);
}
/*--------------------------< FIN ARCHIVO >-----------------------------------------------*/
