#include "conexiones.h"

/* errno */
#include <errno.h>
extern int errno;

/* Constantes "locales". Son #undef al final de conexiones.c */
#define PROTOCOLO_AUTO 0

/* Devuelve el ltimo mensaje de error */
char* conn_getLastError()
{
	return strerror(errno);
}
/****************************************************/

tSocket* CrearSocket()
{
	tSocket* tmp = malloc(sizeof(tSocket));
	if (!tmp) return NULL;
	bzero(tmp, sizeof(tSocket));
	return tmp;
} 
/****************************************************/

void DesbloquearMutex(tSocket *sockIn)
{
	pthread_mutex_unlock(&(sockIn->mutex));
}
void conexiones_InicializarMutex(tSocket* sockIn)
{
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	memcpy(&(sockIn->mutex), &mutex, sizeof(pthread_mutex_t));
	pthread_mutex_lock(&(sockIn->mutex));	/* Esperemos que en todos los sistemas el semaforo inicie en uno */
	sockIn->callback = &DesbloquearMutex;
	sockIn->f_MutexActivo = 1;
}
void conexiones_EsperarConexion(tSocket* sockIn)
{
	pthread_mutex_lock(&(sockIn->mutex));
} 
/****************************************************/

void conexiones_EliminarSocket(tSocket* sock)
{
	int tmp_errno = errno;  /* Si cierro un sock no abierto se crea un error que ignoro */
	
	if (!sock) return;
	
	if (sock->onClose) (*sock->onClose)(sock);
	if (sock->f_MutexActivo) pthread_mutex_destroy(&(sock->mutex));
	if (sock->thread){
		pthread_cancel(sock->thread);
		pthread_join(sock->thread, NULL);
	}
	close(sock->descriptor);		
	free(sock);
	
	errno = tmp_errno;	
}
/****************************************************/

void conexiones_standBySocket(tSocket* standBy)
{
	close(standBy->descriptor);
	standBy->estado = estadoConn_standby;
}

/****************************************************/
void conexiones_CerrarYMarcarEliminar(tSocket* sock)
{
	int tmp_errno = errno;  /* Si cierro un sock no abierto se crea un error que ignoro */
	
	if (!sock) return;
	
	if (sock->f_MutexActivo) pthread_mutex_destroy(&(sock->mutex));
	close(sock->descriptor);		
	sock->estado = estadoConn_eliminar;
	
	errno = tmp_errno;	
}
/****************************************************/

tSocket* conexiones_CrearSocketAsociado(int fd, void (*callback)(struct sSocket*))
{
	tSocket* tmp = CrearSocket();
	if (!tmp) return NULL;
	tmp->descriptor = fd;
	tmp->callback = callback;
	
	return tmp;
} 
/****************************************************/

tSocket* conexiones_CerrarSocketAsociado(tSocket* sock)
{
	conexiones_EliminarSocket(sock);
	return NULL;
}
/****************************************************/

tSocket* conexiones_crearSockEscucha(unsigned short *puerto, short backlog, void (*callback)(struct sSocket*))
{
	int yes; 
	tSocket *sock = CrearSocket();
	yes = 1; /* Param para getsockopt */
	
	if ( !sock ) 
		return NULL;
	
	sock->descriptor = socket(AF_INET, SOCK_STREAM, PROTOCOLO_AUTO);
	if( ERROR == sock->descriptor )
	{
		conexiones_EliminarSocket(sock);
		return NULL;
	}
	
	if( ERROR == setsockopt(sock->descriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) )
	{
		conexiones_EliminarSocket(sock);
		return NULL;
	}
	
	/* Preparo el socket de escucha */
	sock->sock.addr_in.sin_family = AF_INET;
	sock->sock.addr_in.sin_port = htons((*puerto>1024)?*puerto:0); /* Si port<1024 => port=rnd */
	/* Para especificar una direccin usar inet_addr("192.168.0.100"); */
	sock->sock.addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(sock->sock.addr_in.sin_zero), 8);	
	
	if (ERROR == bind(sock->descriptor, &(sock->sock.addr), sizeof(sock->sock.addr)))
	{
		conexiones_EliminarSocket(sock);
		return NULL;
	}
		
	if (ERROR == listen(sock->descriptor, backlog))
	{
		conexiones_EliminarSocket(sock);
		return NULL;
	}
	
	sock->sock_tamanio = sizeof(struct sockaddr);
	yes = getsockname(sock->descriptor, &(sock->sock.addr), (socklen_t*)&(sock->sock_tamanio));
	*puerto = ntohs(sock->sock.addr_in.sin_port);
	sock->estado = estadoConn_escuchando;
	sock->callback = callback;
	
	return sock;
}
/****************************************************/

char* conexiones_getIpLocalDeSocket(tSocket *sock)
{
	sock->sock_tamanio = sizeof(struct sockaddr);
	getsockname(sock->descriptor, &(sock->sock.addr), (socklen_t*)&(sock->sock_tamanio));
	return inet_ntoa(sock->sock.addr_in.sin_addr);
}
char* conexiones_getIpRemotaDeSocket(tSocket *sock)
{
	sock->sock_tamanio = sizeof(struct sockaddr);
	getpeername(sock->descriptor, &(sock->sock.addr), (socklen_t*)&(sock->sock_tamanio));
	return inet_ntoa(sock->sock.addr_in.sin_addr);
}
unsigned short conexiones_getPuertoRemotoDeSocket(tSocket *sock)
{
	sock->sock_tamanio = sizeof(struct sockaddr);
	getpeername(sock->descriptor, &(sock->sock.addr), (socklen_t*)&(sock->sock_tamanio));
	return ntohs(sock->sock.addr_in.sin_port);
}

/*12-06-07:LAS*/
unsigned short conexiones_getPuertoLocalDeSocket(tSocket *sock)
{
	sock->sock_tamanio = sizeof(struct sockaddr);
	getsockname(sock->descriptor, &(sock->sock.addr), (socklen_t*)&(sock->sock_tamanio));
	return ntohs(sock->sock.addr_in.sin_port);
}

/****************************************************/
tSocket* conexiones_crearSockAceptado(tSocket* sockIn, void (*callback)(struct sSocket*))
{
	unsigned int sockaddr_len;
	tSocket* sck;
	
	sck = CrearSocket();
	if (!sck) return NULL;
	sockaddr_len = sizeof(sck->sock.addr);
	sck->callback = callback;
	sck->estado = estadoConn_desconocido;
	sck->descriptor = accept(sockIn->descriptor, &(sck->sock.addr), &sockaddr_len);
	
	if(ERROR == sck->descriptor){
		conexiones_EliminarSocket(sck);
		return NULL;
	}
	
	return sck;
}
/****************************************************/

/* Me conecto a un remote host. Si no se puede, no hago nada */
tSocket* conexiones_ConectarHost(const char* ip, unsigned short puerto, void (*callback)(struct sSocket*))
{
	tSocket *sock = CrearSocket();
	
	if (!sock) return NULL;
	
	sock->descriptor = socket(AF_INET, SOCK_STREAM, PROTOCOLO_AUTO);
	if (ERROR == sock->descriptor){
		conexiones_EliminarSocket(sock);
		return NULL;
	}

	sock->sock.addr_in.sin_family = AF_INET;
	sock->sock.addr_in.sin_port = htons(puerto);
	/* Para especificar una direccin usar inet_addr("192.168.0.100"); */
	sock->sock.addr_in.sin_addr.s_addr = inet_addr(ip);
	bzero(&(sock->sock.addr_in.sin_zero), 8);
		
	if (ERROR == connect(sock->descriptor, &(sock->sock.addr), sizeof(sock->sock.addr))){
		conexiones_EliminarSocket(sock);
		return NULL;
	}
	
	sock->callback = callback;
	sock->estado = estadoConn_desconocido;
	return sock;
}
/****************************************************/

/* Envia los datos en el buffer, hasta que se vacie */
int conexiones_sendBuff(tSocket *sock, const char* buff, int cant)
{
	int totalEnviado = 0; int enviado = 0;
	
	/*LAS:12-06-07: Me cubro contra errores graves o pongo un APB como quieran.*/
	if ( !sock )
	{
		printf("Atencio! Llego el  socket en NULL en conexiones_sendBuff");
		return 0;
	}
	
	if (!buff || !sock->descriptor) return 0;
  
  	totalEnviado = enviado = 0;
  
	while ( enviado < cant ) 
	{
		enviado = send(sock->descriptor, buff+totalEnviado, cant-totalEnviado, MSG_NOSIGNAL);
		
		if ( enviado < 0 ) 
			return ERROR;
			
		totalEnviado += enviado;
	}
    
	return totalEnviado;
}

/****************************************************/
/* Recibe los datos en el buffer, hasta variar el socket o hasta llegar al maximo disponible */
int conexiones_recvBuff(tSocket *sock, char buff[], int cant)
{
	char reintentar = 3;
	int retval = 0, recibido = 0, lastRecibido = 0; /* DET */
		
	/*LAS:12-06-07: Me cubro contra errores graves o pongo un APB como quieran.*/
	if ( !sock )
	{
		printf("Atencio! Llego el  socket en NULL en conexiones_recvBuff");
		return 0;
	}
	
	bzero(buff, cant);
	if (cant == 0) return 0;

	/* OK, una cosa loca para el fcntl: primero el socket es bloqueante, una vez que llegan */
	/* datos se transforma en no bloqueante y se lee hasta que devuelva un error (socket vacio) */
	/* una vez listo, se vuelve a transformar en socket bloqueante */
	while (recibido != cant && retval >= 0 && reintentar > 0)
	{
		retval = recv(sock->descriptor, &(buff[recibido]), cant-recibido, 0);
		recibido += retval;
		if (lastRecibido == recibido) { /*DET*/
			--reintentar;
		}
		fcntl(sock->descriptor, F_SETFL, O_NONBLOCK);
		lastRecibido = recibido; /*DET */
	}
	fcntl(sock->descriptor, F_SETFL, 0); /* Sacar O_NONBLOCK */
	
	return recibido;	
}

/****************************************************/
/* Recibe los datos en el buffer, hasta variar el socket o hasta llegar al maximo disponible */
/*
int conexiones_recvBuff(tSocket *sock, char buff[], int cant)
{
	char reintentar = 3;
	int retval = 0, recibido = 0;
		
	if ( !sock )
	{
		printf("Atencio! Llego el  socket en NULL en conexiones_sendBuff");
		return 0;
	}
	
		
	bzero(buff, cant);
	if (cant == 0) return 0;

	while (recibido != cant && retval >= 0 && reintentar > 0)
	{
		retval = recv(sock->descriptor, &(buff[recibido]), cant-recibido, 0);
		recibido += retval;
		--reintentar;
		fcntl(sock->descriptor, F_SETFL, O_NONBLOCK);
	}
	fcntl(sock->descriptor, F_SETFL, 0);
	
	return recibido;	
}
*/

/****************************************************/

/* Funcion generica para paquetes sin callback establecido */
void socketHndGenerico(tSocket* socket)
{
	#ifdef COMP_DEBUG
		printf("Hay un socket sin handler!");
		abort();
	#endif
	return;
}
void conexiones_aguardarConexion(tSocket* sockets[], unsigned int *cantidad)
{
	#define cant *cantidad
	#define ESPERAR_INDEFINIDAMENTE NULL
	int i, maximo=-1, retval=0;
	struct timeval timeout; int pos_timeout = -1;	
	fd_set lista_escucha;

ReiniciarSelect:
	/* Preparo una lista de sockets para escuchar */
	FD_ZERO(&lista_escucha);
	timeout.tv_sec = 0; timeout.tv_usec = 0;  
	
	for (i=0; (i<=cant) && (sockets[i]); ++i){
		if (sockets[i]->estado == estadoConn_standby) continue;
		
		if(sockets[i]->estado == estadoConn_eliminar){
			conexiones_CerrarSocket(sockets, sockets[i], cantidad);
			goto ReiniciarSelect;
		}
		
		FD_SET(sockets[i]->descriptor, &lista_escucha);
		if (sockets[i]->descriptor > maximo) 
			maximo = sockets[i]->descriptor;
			
		/* Selecciono el menor timeout para salir del select */
		if (sockets[i]->onTimeOut && sockets[i]->segundos_timeout && (sockets[i]->segundos_timeout < timeout.tv_sec || timeout.tv_sec == 0)){
			timeout.tv_sec = sockets[i]->segundos_timeout; 
			pos_timeout = i;
		}
	}

	retval = select(1+maximo, &lista_escucha, NULL, NULL, ( (timeout.tv_sec > 0) ? &timeout : ESPERAR_INDEFINIDAMENTE ));
	
	/* Si select sale por una interrupcion me voy de la funcion */
	/* Si hay una conexion queda pendiente, o en el backlog */
	if (errno == EINTR){ errno = 0; return; }
	
	/* Si el select salio por timeout entonces llamo al callback adecuado */
	if (!retval && pos_timeout >= 0){
		sockets[pos_timeout]->segundos_timeout = 0;
		(*(sockets[pos_timeout]->onTimeOut))(sockets[pos_timeout]);
		return;
	}
	
	/* Llamo al callback de cada socket c/datos */
	for (i=0; (i<=cant) && (sockets[i]); ++i){
		/* Cuidado: no se puede confiar en el valor de sockets[i] luego de llamar al */
		/* callback, ya que es posible que la lista sea realloc'd. Groso valgrind. */
		if (FD_ISSET(sockets[i]->descriptor, &lista_escucha)){
			
			/* Evito que el socket se dispare por timeout, si es que estaba */
			/* seteado y marcado como no persistente */
			if (!sockets[i]->TimeOut_persistente)
				sockets[i]->segundos_timeout = 0;
			
			/* Hack feo: evito refresh de pantalla */
			if (sockets[i] && sockets[i]->f_MutexActivo){
				(*(sockets[i]->callback))(sockets[i]);
				goto ReiniciarSelect;
			}
			
			if (*(sockets[i]->callback)) /* Hay un callback seteado? */
				(*(sockets[i]->callback))(sockets[i]);
			else
				socketHndGenerico(sockets[i]); /* Descarto el paquete */
				
			/* No se sigue procesando la lista ya que no se sabe su estado */
			return;
		}
	}
}
/****************************************************/
/* Cierro un socket a pedido, elimino los sockets marcados como pendientes y compacto el vector */
void conexiones_CerrarSocket(tSocket* listaSocket[], tSocket* cerrar, unsigned int *cantidad)
{
	unsigned int i=0, escribirEn = 0;
	
	while(i <= *cantidad)
	{
		do{
			/* Si es un socket vacio lo sobreescribo con otro */
			if (listaSocket[i] == NULL) break;

			/* Cierro sockets marcados para eliminar */
			if (listaSocket[i]->estado == estadoConn_eliminar){
				
				if (listaSocket[i]->thread){
					pthread_cancel(listaSocket[i]->thread);
					pthread_join(listaSocket[i]->thread, NULL);
				}
								
				free(listaSocket[i]);
				listaSocket[i] = NULL;
				break;
				
			}
		
			if (listaSocket[i] == cerrar){
				conexiones_EliminarSocket(listaSocket[i]);
				listaSocket[i] = NULL;
				break;
			}
		
			listaSocket[escribirEn++] = listaSocket[i];
		}while(0);
		++i;
	}
	
	*cantidad = escribirEn-1;
}

/*********************************************************/
/*  Cierro todas las conexiones de una lista de sockets	 */
/*********************************************************/
void conexiones_CerrarLista (unsigned int primerSocket, unsigned int *ultimoSocket, tSocket **lstSockets)
{
	int i = *ultimoSocket;
	
	while (i > primerSocket){
		if (lstSockets[i]->thread){
			pthread_cancel(lstSockets[i]->thread);
			pthread_join(lstSockets[i]->thread, NULL);
		}
					
		conexiones_EliminarSocket(lstSockets[i]);
		lstSockets[i] = NULL;
		--i;
	}
	
	*ultimoSocket = primerSocket;
}

/****************************************************/
int compararSocket( const void *s1, const void *s2 )
{
	tSocket		*psock1 = (tSocket*) s1,
				*psock2 = (tSocket*) s2;
						
	if ( psock1 == psock2 )
	{
		return 0;
	}
	
	return 1;	
}

#undef PROTOCOLO_AUTO
#undef ERROR
