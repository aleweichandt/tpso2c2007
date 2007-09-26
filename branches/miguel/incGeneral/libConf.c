#include "libConf.h"

int libConf_getCad(FILE *flujo, int fin, char *cad, unsigned int max)
{
	int c, pos = 0;
	
	max -= 1; /*  para el \0 */
	while ((fin != (c = fgetc(flujo))) && c != EOF)
	{
		if (pos < max) cad[pos] = (char)c;
		pos++;
	}
	cad[(pos > max)? max+1 : pos] = '\0';
	
	return pos;
}


tConfig *config_CrearVacio()
{
	tConfig *config;
	config = malloc(sizeof(tConfig));
	memset(config, '\0', sizeof(tConfig));
	config->cantidadLlaves = 0;
	config->llaves = malloc(sizeof(nLlave*));
	memset(config->llaves, '\0', sizeof(nLlave*));
		
	return config;
}


/* Busco el �ndice de una llave. Devuelve 1 si lo encuentro, NOT_FOUND sino */
/* Devuelve la posici�n en donde deber�a estar en posici�n */
char getIndiceDeLlave(tConfig *config, const char *seccion, const char *nombre, int *posicion)
{
	int i=0, f=1;
	
	if (!config || !config->llaves) return INDICE_NOT_FOUND;
	while (i<(config->cantidadLlaves)&&f)
	{
		if (!strcmp(config->llaves[i]->seccion, seccion)&&!strcmp(config->llaves[i]->nombre, nombre))
			f=0;
		else
			i++;
	}
	
	if (i>config->cantidadLlaves)
		i = config->cantidadLlaves;	
	
	*posicion = i;

	if (!config->llaves[i]) return INDICE_NOT_FOUND;
	if (strcmp(config->llaves[i]->seccion, seccion)!=0) return INDICE_NOT_FOUND;
	if (strcmp(config->llaves[i]->nombre, nombre)!=0) return INDICE_NOT_FOUND;
  	return i;
  	
	if (config->llaves[i] != 0 && strcmp(config->llaves[i]->seccion, seccion)==0 && strcmp(config->llaves[i]->nombre, nombre)==0)
		return i;
	
	return INDICE_NOT_FOUND;
}

/* Modifica el valor de una llave. Si esta no existe, la crea */
tConfig *config_SetVal(tConfig *config, const char *seccion, const char *nombre, const char *valor)
{
	int cnt, pos=0;
	
	/* Me fijo si existe la llave y actualizo su valor */
	if (getIndiceDeLlave(config, seccion, nombre, &pos) != INDICE_NOT_FOUND){
		strcpy(config->llaves[pos]->valor, valor);
		return config;
	}
	
	cnt = ++(config->cantidadLlaves);
	config->llaves = realloc(config->llaves, (1+cnt)*sizeof(nLlave*));
	
	/* Si no existe la inserto de forma ordenada */
	while (pos < cnt){
		config->llaves[cnt] = config->llaves[cnt-1];
		cnt--;
	}
	
	config->llaves[pos] = malloc(sizeof(nLlave));	
	
	cnt = 1+strlen(valor);
	strncpy(config->llaves[pos]->valor, valor, cnt);
	
	cnt = 1+strlen(nombre);
	strncpy(config->llaves[pos]->nombre, nombre, cnt);
	
	cnt = 1+strlen(seccion);
	strncpy(config->llaves[pos]->seccion, seccion, cnt);
	return config;	
}

void config_DelVal(tConfig *config,const char *seccion, const char *nombre)
{
	int pos=0;
	
	/* Me fijo si existe la llave para eliminarla */
	if (getIndiceDeLlave(config,seccion, nombre, &pos) == INDICE_NOT_FOUND) return;
	
	free(config->llaves);
	/* disminuyo el tama�o y voy copiando hacia arriba */	
	--(config->cantidadLlaves);
	while (pos < config->cantidadLlaves){
		config->llaves[pos] = config->llaves[pos+1];
		pos++;
	}
	config->llaves = realloc(config->llaves, (1+config->cantidadLlaves)*sizeof(nLlave*));	
}

char *config_GetVal(tConfig *config, const char *seccion, const char *nombre)
{
	int pos;
	if (getIndiceDeLlave(config, seccion, nombre, &pos) == INDICE_NOT_FOUND) return NULL;	
	return config->llaves[pos]->valor;
}

int config_GetVal_Int(tConfig *config, const char *seccion, char *nombre)
{
	char *buff = config_GetVal(config, seccion, nombre);
	return (buff)? atoi(buff) : 0;
}

void config_Print(tConfig *config, FILE* fp)
{
	int i=0;
	char ant[50];
	while (i<config->cantidadLlaves)
	{ 
	 	 strncpy(ant, config->llaves[i]->seccion, 1+strlen(config->llaves[i]->seccion));
	 	 fprintf(fp, "%%%s%%\n", config->llaves[i]->seccion);
		
		while ((i < config->cantidadLlaves) && (strcmp(ant, config->llaves[i]->seccion)==0))  
		{	
			fprintf(fp, "<%s>=%s\n", config->llaves[i]->nombre, config->llaves[i]->valor);
	   		i++;
		}
	}	
}

void config_Guardar (tConfig* cfg, const char* arch)
{	
	FILE* fp = fopen(arch, "w+");
	if (!fp) return;
	
	config_Print(cfg, fp);
	fclose(fp);	/*12/07/2007	GT	Cierro el archivo*/
}
int getCol(int car)
{
	switch(car)
	{
		case '<': return 0;
		case '>': return 1;
		case '=': return 2;
		case '%': return 4;
	}				
	return 3;
}

tConfig* config_CrearDeBuff(char* buffer, const char *seccion, int len)
{
	static int tran[5][5] = {
		/*   <	>  =  *  %        Estado                             */
    	{2, 0, 0, 0, 1}, /* 0. Descartando caracteres espureos   */
		{2, 2, 2, 2, 2}, /* 1. Guardando seccion 				 */
		{3, 3, 3, 3, 3}, /* 2. guardando nombre  				 */
		{3, 3, 0, 3, 3}, /* 3. Buscando valor    				 */	
	};	
	
	char nombre[MAXLEN]; char valor[MAXLEN];
	int estado = 0; int i, pos = 0; char c;
	tConfig* cfg;
	if (!(cfg = config_CrearVacio())) return NULL;	
	
	while ((c=buffer[pos++])!='\0' && pos <= len)
	{
		estado = tran[estado][getCol(c)];
		if (estado == 2){
			strtok(&(buffer[pos]), ">");
			for (i=0; buffer[pos] && i < MAXLEN;) nombre[i++] = buffer[pos++];
			nombre[i++] = '\0';
			
		}else if (estado == 3){
			strtok(&(buffer[pos]), "\n");
			for (i=0; buffer[pos] && i < MAXLEN;) valor[i++] = buffer[pos++];
			valor[i++] = '\0';
			
			config_SetVal(cfg, seccion, nombre, valor);
			estado = 0;
			memset(nombre, '\0', sizeof(nombre));
			memset(valor, '\0', sizeof(valor));
		}			
		pos++;	
	}
	
	return cfg;	
}

tConfig *config_Crear(const char *ruta, const char *s____eccion)
{	
	static int tran[5][5] = {
		/*   <	>  =  *  %        Estado                             */
    	{2, 0, 0, 0, 1}, /* 0. Descartando caracteres espureos   */
		{2, 2, 2, 2, 2}, /* 1. Guardando seccion 				 */
		{3, 3, 3, 3, 3}, /* 2. guardando nombre  				 */
		{3, 3, 0, 3, 3}, /* 3. Buscando valor    				 */	
	};	
	
	char nombre[MAXLEN]; char valor[MAXLEN]; char seccion[MAXLEN];
	int estado = 0;
	int c; FILE* fp;
	tConfig* cfg;
	
	if (!(cfg = config_CrearVacio())) return NULL;	
	fp = fopen(ruta, "r+");
	if (!fp){
		free(cfg);
		return NULL;  /* Si no hay archivo creo config vacia */
	}
	
	memset(nombre, '\0', sizeof(nombre));
	memset(valor, '\0', sizeof(valor));
	memset(seccion, '\0', sizeof(seccion));
	while ((c=fgetc(fp))!=EOF)
	{
		estado = tran[estado][getCol(c)];
		if (estado == 1) {
			libConf_getCad(fp, '%', seccion, MAXLEN);
			fgetc(fp);
		}else if (estado == 2){
			libConf_getCad(fp, '>', nombre, MAXLEN);
		}else if (estado == 3){
			libConf_getCad(fp, '\n', valor, MAXLEN);
			config_SetVal(cfg, seccion, nombre, valor);
			estado = 0;
		}				
	}
	
	fclose(fp);
	return cfg;	
}

void config_Destroy (tConfig *config)
{
   int i;

   if (!config) return;

   for (i = 0; i < config->cantidadLlaves; i++)
  	 {      
		if (config->llaves[i] != NULL)
			free (config->llaves[i]);
			config->llaves[i] = NULL;
   	 }

   if (config->llaves != NULL)
     {
		free (config->llaves);
		config->llaves = NULL;
     }
   
   free(config);
     
    return; 
   
}
