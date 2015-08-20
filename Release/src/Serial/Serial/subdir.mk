################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/Serial/Serial/serial.cc 

CC_DEPS += \
./src/Serial/Serial/serial.d 

OBJS += \
./src/Serial/Serial/serial.o 


# Each subdirectory must supply rules for building sources it contributes
src/Serial/Serial/%.o: ../src/Serial/Serial/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


