#!/bin/sh
############################################################
# Script de creacion de usuarios.
############################################################

#Path absoluto del archivo de usuarios.
PATH_ARCHIVO_USUARIOS="/home/leonardo/Pruebas/usuarios"
#Path absoluto donde residen las claves de los usuarios.
PATH_CLAVES="/home/leonardo/Pruebas/claves"


echo "Datos del usuario a crear"
echo -e "-------------------------\n"
read -p "Nombre: " nombre_usuario
read -p "Password de $nombre_usuario: " pass_usuario
read -p "Recursos compartidos permitidos (Separados por coma): " recursos_permitidos

# Insercion del registro en el archivo de usuarios.
REGISTRO_USUARIO="$nombre_usuario:$pass_usuario:$recursos_permitidos"

echo -e "\nSe creara el siguiente registro en el archivo de usuarios: " $REGISTRO_USUARIO

echo $REGISTRO_USUARIO >> $PATH_ARCHIVO_USUARIOS


# Creacion de la clave de (des)encripcion usuario.
echo "Generando clave de (des)encripcion: $nombre_usuario.key..."
dd if=/dev/urandom of=$PATH_CLAVES/$nombre_usuario.key bs=1 count=1 &>/dev/null

echo -n "La creacion de la clave $nombre_usuario.key ha sido: "
if [ -e $PATH_CLAVES/$nombre_usuario.key ]; then
	echo "exitosa."
else
	echo "erronea."
fi
