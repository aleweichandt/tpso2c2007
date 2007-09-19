################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../incGeneral/Sockets/conexiones.c \
../incGeneral/Sockets/paquetesGeneral.c 

OBJS += \
./incGeneral/Sockets/conexiones.o \
./incGeneral/Sockets/paquetesGeneral.o 

C_DEPS += \
./incGeneral/Sockets/conexiones.d \
./incGeneral/Sockets/paquetesGeneral.d 


# Each subdirectory must supply rules for building sources it contributes
incGeneral/Sockets/%.o: ../incGeneral/Sockets/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


