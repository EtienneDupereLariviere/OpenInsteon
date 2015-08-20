################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/Serial/Serial/impl/unix.cc \
../src/Serial/Serial/impl/win.cc 

CC_DEPS += \
./src/Serial/Serial/impl/unix.d \
./src/Serial/Serial/impl/win.d 

OBJS += \
./src/Serial/Serial/impl/unix.o \
./src/Serial/Serial/impl/win.o 


# Each subdirectory must supply rules for building sources it contributes
src/Serial/Serial/impl/%.o: ../src/Serial/Serial/impl/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


