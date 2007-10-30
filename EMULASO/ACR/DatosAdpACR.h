#ifndef DATOSADPACR_H_
#define DATOSADPACR_H_

#include "../incGeneral/incGeneral.h"
#include "../incGeneral/Sockets/conexiones.h"

typedef struct
{
	tSocket** lista;
	int cantidad;
} tListaAdps;


void		DatosAdpACR_Ini					(tListaAdps* lista);
void		DatosAdpACR_Agregar				(tListaAdps* lista, tSocket *adp);
tSocket*	DatosAdpACR_Obtener				(tListaAdps* lista_res, int indice);
void 		DatosAdpACR_EliminarDeLaLista	(tListaAdps* lista_res, tSocket* adp );
void 		DatosAdpACR_EliminarLista		(tListaAdps* lista_res );

#endif /*DATOSADPACR_H_*/
