################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ADS/ADS.c \
../ADS/ADSLib.c 

OBJS += \
./ADS/ADS.o \
./ADS/ADSLib.o 

C_DEPS += \
./ADS/ADS.d \
./ADS/ADSLib.d 


# Each subdirectory must supply rules for building sources it contributes
ADS/%.o: ../ADS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


