GCC_NOCHECK = 
GCC_BASICO =
GCC_POSIX = $(GCC_BASICO) 
GCC_ANSI = $(GCC_POSIX) -ansi -DNCOMP_SOLO_ANSI
# -DCHT_NOGRAF elimina los iconos del contestador
GCC_DEBUG = -O0 -g3 -DNCOMP_DEBUG
LIBRERIAS = lib/incGeneral.o lib/log.o lib/infoControl.o lib/estructuras.o lib/libConf.o lib/conexiones.o lib/paquetesGeneral.o lib/ventanas.o lib/DatosUsuariosADS.o lib/FindFile.o


all : librerias MSHELL ADS ADP ACR PPCB
	@echo 'Compilacion finalizada.'

.PHONY : clean
clean :
	@rm -f lib/* mshell ads acr adp ppcb nor *.log > /dev/null

#MShell -----------------------------------------
MSHELL : librerias lib/MShellLib.o MShell/MShell.c MShell/MShell.h
	gcc $(GCC_DEBUG) $(GCC_ANSI) -lpthread -o"mshell" $(LIBRERIAS) "lib/MShellLib.o" "MShell/MShell.c"

lib/MShellLib.o : MShell/MShellLib.c MShell/MShellLib.h
	gcc $(GCC_DEBUG) $(GCC_ANSI) -c -o"lib/MShellLib.o" "MShell/MShellLib.c"

#ADS ---------------------------------------------
ADS : librerias lib/DatosUsuariosADS.o lib/ADSLib.o ADS/ADS.c ADS/ADS.h
	gcc $(GCC_DEBUG) $(GCC_ANSI) -lpthread -o"ads" $(LIBRERIAS) "lib/ADSLib.o" "ADS/ADS.c"

lib/ADSLib.o : ADS/ADSLib.c ADS/ADSLib.h
	gcc $(GCC_DEBUG) $(GCC_ANSI) -c -o"lib/ADSLib.o" "ADS/ADSLib.c"

lib/DatosUsuariosADS.o : ADS/DatosUsuariosADS.c ADS/DatosUsuariosADS.h
	gcc $(GCC_DEBUG) $(GCC_ANSI) -c -o"lib/DatosUsuariosADS.o" "ADS/DatosUsuariosADS.c"

#ADP ---------------------------------------------
ADP : librerias lib/ADPLib.o ADP/ADP.c ADP/ADP.h
	gcc $(GCC_DEBUG) $(GCC_ANSI) -lpthread -o"adp" $(LIBRERIAS) "lib/ADPLib.o" "ADP/ADP.c"

lib/ADPLib.o : lib/DatosPCBs.o lib/paquetesGeneral.o ADP/ADPLib.c ADP/ADPLib.h
	gcc $(GCC_DEBUG) $(GCC_ANSI) -c -o"lib/ADPLib.o" "ADP/ADPLib.c"
lib/DatosPCBs.o : ADP/DatosPCBs.c ADP/DatosPCBs.h
	gcc $(GCC_DEBUG) $(GCC_ANSI) -c -o"lib/DatosPCBs.o" "ADP/DatosPCBs.c"
#lib/paquetesADP.o : incGeneral/Sockets/paquetesADP.c incGeneral/Sockets/paquetesADP.h
#	gcc $(GCC_DEBUG) $(GCC_ANSI) -c -o "lib/paquetesGeneral.o" "lib/paquetesADP.o" "incGeneral/Sockets/paquetesADP.c"
#lib/paquetesACR.o : incGeneral/Sockets/paquetesACR.c incGeneral/Sockets/paquetesACR.h
#	gcc $(GCC_DEBUG) $(GCC_ANSI) -c -o"lib/paquetesGeneral.o" "lib/paquetesACR.o" "incGeneral/Sockets/paquetesACR.c"

#ACR ---------------------------------------------
ACR : librerias lib/ACRLib.o ACR/ACR.c ACR/ACR.h lib/DatosPpcbACR.o lib/DatosAdpACR.o lib/DatosRecurso.o lib/MatrizRecursos.o
	gcc $(GCC_DEBUG) $(GCC_ANSI) -Lefence -lpthread -o"acr" $(LIBRERIAS) "lib/ACRLib.o" "lib/DatosPpcbACR.o" "lib/DatosAdpACR.o" "lib/DatosRecurso.o" "ACR/ACR.c" "lib/MatrizRecursos.o"

lib/ACRLib.o : ACR/ACRLib.c ACR/ACRLib.h
	gcc $(GCC_DEBUG) $(GCC_ANSI) -c -Lefence -o"lib/ACRLib.o" "ACR/ACRLib.c"

lib/DatosPpcbACR.o : ACR/DatosPpcbACR.c ACR/DatosPpcbACR.h
	gcc $(GCC_DEBUG) $(GCC_ANSI) -c -o"lib/DatosPpcbACR.o" "ACR/DatosPpcbACR.c"

lib/DatosAdpACR.o : ACR/DatosAdpACR.c ACR/DatosAdpACR.h
	gcc $(GCC_DEBUG) $(GCC_ANSI) -c -o"lib/DatosAdpACR.o" "ACR/DatosAdpACR.c"

lib/DatosRecurso.o : ACR/DatosRecurso.c ACR/DatosRecurso.h
	gcc $(GCC_DEBUG) $(GCC_ANSI) -c -o"lib/DatosRecurso.o" "ACR/DatosRecurso.c"

lib/MatrizRecursos.o : ACR/MatrizRecursos.c ACR/MatrizRecursos.h
	gcc $(GCC_DEBUG) $(GCC_ANSI) -c -o"lib/MatrizRecursos.o" "ACR/MatrizRecursos.c"

#PPCB ---------------------------------------------
PPCB : librerias lib/PPCBLib.o PPCB/PPCB.c PPCB/PPCB.h lib/Stack.o
	gcc $(GCC_DEBUG) $(GCC_ANSI) -lpthread -o"ppcb" $(LIBRERIAS) "lib/PPCBLib.o" "lib/Stack.o" "PPCB/PPCB.c"

lib/PPCBLib.o : PPCB/PPCBLib.c PPCB/PPCBLib.h
	gcc $(GCC_DEBUG) $(GCC_ANSI) -c -o"lib/PPCBLib.o" "PPCB/PPCBLib.c"


#Librerias -------------------------------------
librerias : $(LIBRERIAS)

lib/incGeneral.o : incGeneral/incGeneral.c incGeneral/incGeneral.h
	gcc $(GCC_DEBUG) $(GCC_ANSI) -c -o"lib/incGeneral.o" "incGeneral/incGeneral.c"
	
lib/log.o : incGeneral/log.c incGeneral/log.h
	gcc $(GCC_DEBUG) $(GCC_POSIX) -c -o"lib/log.o" "incGeneral/log.c"

lib/infoControl.o: incGeneral/infoControl.c incGeneral/infoControl.h
	gcc $(GCC_DEBUG) $(GCC_POSIX) -c -o"lib/infoControl.o" "incGeneral/infoControl.c"

lib/estructuras.o : incGeneral/Estructuras/estructuras.c incGeneral/Estructuras/estructuras.h
	gcc $(GCC_DEBUG) $(GCC_ANSI) -c -o"lib/estructuras.o" "incGeneral/Estructuras/estructuras.c"

lib/incFileSplitter.o: incGeneral/incFileSplitter.c incGeneral/incFileSplitter.h
	gcc $(GCC_DEBUG) $(GCC_ANSI) -c -o"lib/incFileSplitter.o" "incGeneral/incFileSplitter.c"

lib/libConf.o : incGeneral/libConf.c incGeneral/libConf.h
	gcc $(GCC_DEBUG) $(GCC_ANSI) -c -o"lib/libConf.o" "incGeneral/libConf.c"

lib/conexiones.o : incGeneral/Sockets/conexiones.c incGeneral/Sockets/conexiones.h
	gcc $(GCC_DEBUG) $(GCC_ANSI) -c -o"lib/conexiones.o" "incGeneral/Sockets/conexiones.c"

lib/paquetesGeneral.o : incGeneral/Sockets/paquetesGeneral.c incGeneral/Sockets/paquetesGeneral.h
	gcc $(GCC_DEBUG) $(GCC_ANSI) -c -o"lib/paquetesGeneral.o" "incGeneral/Sockets/paquetesGeneral.c"
	
lib/ventanas.o : incGeneral/ventanas.c incGeneral/ventanas.h
	gcc $(GCC_DEBUG) $(GCC_NOCHECK) -c -o"lib/ventanas.o" "incGeneral/ventanas.c"

lib/FindFile.o: incGeneral/FindFile.c incGeneral/FindFile.h
	gcc $(GCC_DEBUG) $(GCC_NOCHECK) -c -o"lib/FindFile.o" "incGeneral/FindFile.c"

lib/Stack.o : incGeneral/Estructuras/Stack.c incGeneral/Estructuras/Stack.h
	gcc $(GCC_DEBUG) $(GCC_ANSI) -c -o"lib/Stack.o" "incGeneral/Estructuras/Stack.c"
