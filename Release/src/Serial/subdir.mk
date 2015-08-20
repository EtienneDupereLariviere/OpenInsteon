################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Serial/AutoResetEvent.cpp \
../src/Serial/SerialPortCreator.cpp \
../src/Serial/SerialPortDriver.cpp 

OBJS += \
./src/Serial/AutoResetEvent.o \
./src/Serial/SerialPortCreator.o \
./src/Serial/SerialPortDriver.o 

CPP_DEPS += \
./src/Serial/AutoResetEvent.d \
./src/Serial/SerialPortCreator.d \
./src/Serial/SerialPortDriver.d 


# Each subdirectory must supply rules for building sources it contributes
src/Serial/%.o: ../src/Serial/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -std=c++0x -DBOOST_LOG_DYN_LINK -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


