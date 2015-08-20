################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Util/Log.cpp \
../src/Util/Timer.cpp \
../src/Util/Utilities.cpp 

OBJS += \
./src/Util/Log.o \
./src/Util/Timer.o \
./src/Util/Utilities.o 

CPP_DEPS += \
./src/Util/Log.d \
./src/Util/Timer.d \
./src/Util/Utilities.d 


# Each subdirectory must supply rules for building sources it contributes
src/Util/%.o: ../src/Util/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -std=c++0x -DBOOST_LOG_DYN_LINK -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


