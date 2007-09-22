################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../incGeneral/Estructuras/estructuras.c 

OBJS += \
./incGeneral/Estructuras/estructuras.o 

C_DEPS += \
./incGeneral/Estructuras/estructuras.d 


# Each subdirectory must supply rules for building sources it contributes
incGeneral/Estructuras/%.o: ../incGeneral/Estructuras/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


