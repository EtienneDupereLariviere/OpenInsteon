################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/Serial/SerialImp/serial.cc 

CC_DEPS += \
./src/Serial/SerialImp/serial.d 

OBJS += \
./src/Serial/SerialImp/serial.o 


# Each subdirectory must supply rules for building sources it contributes
src/Serial/SerialImp/%.o: ../src/Serial/SerialImp/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -std=c++0x -DBOOST_LOG_DYN_LINK -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


