GCC_NOCHECK = 
GCC_BASICO =
GCC_POSIX = $(GCC_BASICO) 
GCC_ANSI = $(GCC_POSIX) -ansi -DNCOMP_SOLO_ANSI
# -DCHT_NOGRAF elimina los iconos del contestador
GCC_DEBUG = -O0 -g3 -DNCOMP_DEBUG
LIBRERIAS = lib/incGeneral.o lib/log.o lib/infoControl.o lib/estructuras.o lib/libConf.o lib/conexiones.o lib/paquetesGeneral.o lib/ventanas.o lib/DatosUsuariosADS.o


all : librerias MSHELL ADS
	@echo 'Compilacion finalizada.'

.PHONY : clean
clean :
	@rm -f $(LIBRERIAS) buscador nor logs/* > /dev/null

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
