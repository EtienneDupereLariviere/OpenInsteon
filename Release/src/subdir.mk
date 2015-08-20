################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/InsteonAddress.cpp \
../src/InsteonConnection.cpp \
../src/InsteonController.cpp \
../src/InsteonDevice.cpp \
../src/InsteonDeviceLinkRecord.cpp \
../src/InsteonDeviceList.cpp \
../src/InsteonIdentity.cpp \
../src/InsteonMessage.cpp \
../src/InsteonMessageProcessor.cpp \
../src/InsteonMessenger.cpp \
../src/InsteonNetwork.cpp \
../src/InsteonNetworkBridge.cpp \
../src/openinsteon.cpp 

OBJS += \
./src/InsteonAddress.o \
./src/InsteonConnection.o \
./src/InsteonController.o \
./src/InsteonDevice.o \
./src/InsteonDeviceLinkRecord.o \
./src/InsteonDeviceList.o \
./src/InsteonIdentity.o \
./src/InsteonMessage.o \
./src/InsteonMessageProcessor.o \
./src/InsteonMessenger.o \
./src/InsteonNetwork.o \
./src/InsteonNetworkBridge.o \
./src/openinsteon.o 

CPP_DEPS += \
./src/InsteonAddress.d \
./src/InsteonConnection.d \
./src/InsteonController.d \
./src/InsteonDevice.d \
./src/InsteonDeviceLinkRecord.d \
./src/InsteonDeviceList.d \
./src/InsteonIdentity.d \
./src/InsteonMessage.d \
./src/InsteonMessageProcessor.d \
./src/InsteonMessenger.d \
./src/InsteonNetwork.d \
./src/InsteonNetworkBridge.d \
./src/openinsteon.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -std=c++0x -DBOOST_LOG_DYN_LINK -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


