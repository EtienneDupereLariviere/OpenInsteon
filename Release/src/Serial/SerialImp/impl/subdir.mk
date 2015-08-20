################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/Serial/SerialImp/impl/unix.cc \
../src/Serial/SerialImp/impl/win.cc 

CC_DEPS += \
./src/Serial/SerialImp/impl/unix.d \
./src/Serial/SerialImp/impl/win.d 

OBJS += \
./src/Serial/SerialImp/impl/unix.o \
./src/Serial/SerialImp/impl/win.o 


# Each subdirectory must supply rules for building sources it contributes
src/Serial/SerialImp/impl/%.o: ../src/Serial/SerialImp/impl/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -std=c++0x -DBOOST_LOG_DYN_LINK -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


