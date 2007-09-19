################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../incGeneral/incGeneral.c \
../incGeneral/infoControl.c \
../incGeneral/libConf.c \
../incGeneral/log.c \
../incGeneral/ventanas.c 

OBJS += \
./incGeneral/incGeneral.o \
./incGeneral/infoControl.o \
./incGeneral/libConf.o \
./incGeneral/log.o \
./incGeneral/ventanas.o 

C_DEPS += \
./incGeneral/incGeneral.d \
./incGeneral/infoControl.d \
./incGeneral/libConf.d \
./incGeneral/log.d \
./incGeneral/ventanas.d 


# Each subdirectory must supply rules for building sources it contributes
incGeneral/%.o: ../incGeneral/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


