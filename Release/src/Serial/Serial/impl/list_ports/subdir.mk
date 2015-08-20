################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/Serial/Serial/impl/list_ports/list_ports_linux.cc \
../src/Serial/Serial/impl/list_ports/list_ports_osx.cc \
../src/Serial/Serial/impl/list_ports/list_ports_win.cc 

CC_DEPS += \
./src/Serial/Serial/impl/list_ports/list_ports_linux.d \
./src/Serial/Serial/impl/list_ports/list_ports_osx.d \
./src/Serial/Serial/impl/list_ports/list_ports_win.d 

OBJS += \
./src/Serial/Serial/impl/list_ports/list_ports_linux.o \
./src/Serial/Serial/impl/list_ports/list_ports_osx.o \
./src/Serial/Serial/impl/list_ports/list_ports_win.o 


# Each subdirectory must supply rules for building sources it contributes
src/Serial/Serial/impl/list_ports/%.o: ../src/Serial/Serial/impl/list_ports/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


