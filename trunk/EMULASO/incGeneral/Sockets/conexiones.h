#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* socket, bind */
#include <sys/socket.h>
#include <sys/types.h>
/* htons, inet_ntoa */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
/* select */
#include <sys/time.h>
#include <sys/types.h>
/* fcntl */
#include <fcntl.h>
/* Mutex */
#include <pthread.h>

#include "../incGeneral.h"

#define SOCKET_CERRADO -1
#define ERROR -1

typedef struct sSocket{
	int descriptor;						/* Valor de retorno de socket() */
	int segundos_timeout;				/* Tiempo a esperar */
	char TimeOut_persistente;			/* El timeout debe persistir si se detecta un evento anterior? */
	
	union{								/* Las estructuras son de = tamanio */
		struct sockaddr_in addr_in;
		struct sockaddr    addr;
	}sock;	
	int sock_tamanio;					/* sizeof(struct sockaddr_in) */
	
	/* Funciones para llamar en cada evento */
	void (*callback)(struct sSocket*);	/* Funcion a llamar cuando se puede leer */
	void (*onClose)(struct sSocket*);	/* Funcion a llamar antes de eliminar el socket */
	void (*onTimeOut)(struct sSocket*);	/* Funcion a llamar si hay un timeout */	
	void *extra;						/* Un void* para pasar datos entre funciones */
	
	char estado;						/* Var. para almacenar un estado entre llamadas */
	char f_MutexActivo;					/* Indica si se usa el mutex */
	pthread_mutex_t mutex;
	pthread_t thread;
}tSocket;

enum conn_estados{
	estadoConn_escuchando,	
	estadoConn_desconocido,  		 /* Todavia no se sabe que tipo de cliente es */	
	
	estadoConn_cel_inicioHandshake,	 /* Un celular inicio el handshake y una confirmacion fue enviada  */
	estadoConn_cel_recibeHandshake,	 /* Un celular recibio el handshake y envio ip:puerto  */
	estadoConn_cel_pendiente,		 /* No hay lugar en la cola de conectados, el cel espera */
	estadoConn_cel_conectado,		 /* El cel envio su ip:puerto y esta online */
	estadoConn_cel_cargando,		 /* El cel se conecto a un cargador y esta pendiente */
	
	estadoConn_car_inicioHandshake,	 /* Envie ack a un cargador */
	estadoConn_car_conectado,		 /* El cargador envio su ip:prt y esta online */
	
	estadoConn_eliminar,			 /* Socket marcado para su eliminacion */
	estadoConn_standby				 /* Socket en pausa */
};

char* 	 conn_getLastError();
void 	 conexiones_InicializarMutex		(tSocket* sock);
void	 conexiones_EsperarConexion			(tSocket* sockIn);
tSocket* conexiones_CrearSocketAsociado		(int fd, void (*callback)(struct sSocket*));
tSocket* conexiones_CerrarSocketAsociado	(tSocket* sock);
void	 conexiones_CerrarYMarcarEliminar	(tSocket* sock);
void	 conexiones_standBySocket			(tSocket* standBy);
char*	 conexiones_getIpLocalDeSocket		(tSocket *sock);
char*	 conexiones_getIpRemotaDeSocket		(tSocket *sock);
unsigned short conexiones_getPuertoRemotoDeSocket(tSocket *sock);
tSocket* conexiones_crearSockEscucha		(unsigned short *puerto, short backlog, void (*callback)(struct sSocket*));
tSocket* conexiones_crearSockAceptado		(tSocket* sockIn, void (*callback)(struct sSocket*));
tSocket* conexiones_ConectarHost			(const char*  ip, unsigned short puerto, void (*callback)(struct sSocket*));
int 	 conexiones_sendBuff				(tSocket *sock, const char* buff, int cant);
int 	 conexiones_recvBuff				(tSocket *sock, char buff[], int cant);
void	 conexiones_aguardarConexion		(tSocket* sockets[], unsigned int *cantidad);
void 	 conexiones_CerrarSocket			(tSocket* listaSocket[], tSocket* cerrar, unsigned int *cantidad);
void	 conexiones_CerrarLista				(unsigned int primerSocket, unsigned int *ultimoSocket, tSocket **lstSockets);
/*12-06-07:LAS*/
unsigned short conexiones_getPuertoLocalDeSocket(tSocket *sock);
int compararSocket( const void *s1, const void *s2 );
#endif /*CONEXIONES_H_*/
