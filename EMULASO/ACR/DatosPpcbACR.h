/*
  *<<---------------- BATMAN & ROUND ROBIN ----------------------------->>
 * Archivo: DatosPpcbACR.h
 * 
 * Historia
 * 23/09/2007	GT	Creacion
 *
 ------------------------------------------------------------------ 
 */
#ifndef DATOSPPCBACR_
#define DATOSPPCBACR_

#include <time.h>
#include "../incGeneral/incGeneral.h"
#include "../incGeneral/Estructuras/estructuras.h"
#include "../incGeneral/Sockets/conexiones.h"

typedef enum tActividad
{
	Estado_Activo,
	Estado_Inactivo
} tActividad;


typedef struct tPpcbAcr 
{
	long pid;					/*asignado por el sistema*/
	int  pidChild;				/*asignado por el fork*/
	char szUsuario[LEN_USUARIO];
	char szComando[LEN_COMANDO_EJEC];
	long lIdSesion;				/*23/09/2007	GT	TODO: Verificar con el ADS su tipo de datos*/
	int iMemoria;
	
	tActividad sActividad;		/* determina si está activo o si countdown de vida en el sistema */
	time_t sFechaInactvdad;		/* instante de ingreso al sistema o de inactividad*/

	tSocket* socket;
	
} tPpcbAcr;

#define tListaPpcbAcr t_nodo*

void			PpcbAcr_IniciarLista	(tListaPpcbAcr lista);
int 			PpcbAcr_AgregarPpcb		(tListaPpcbAcr *lista, const long pid);
void			PpcbAcr_EliminarPpcb	(tListaPpcbAcr *lista, const long pid);
void			PpcbAcr_EliminarTodas	(tListaPpcbAcr *lista);
tPpcbAcr*		PpcbAcr_BuscarTrans		(tListaPpcbAcr *lista, const long pid, int *pos);
tPpcbAcr*		PpcbAcr_Datos			(tListaPpcbAcr lista);
tListaPpcbAcr	PpcbAcr_Siguiente		(tListaPpcbAcr lista);

int 			comparaPpcbAcr			( const void *ppcb1, const void *ppcb2 );

#endif /*DATOSPPCBACR_*/