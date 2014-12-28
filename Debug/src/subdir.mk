################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../src/ParseTorrent.o \
../src/bt_lib.o \
../src/bt_setup.o 

CPP_SRCS += \
../src/BitTorrent.cpp \
../src/ParseTorrent.cpp \
../src/bt_lib.cpp \
../src/bt_setup.cpp 

OBJS += \
./src/BitTorrent.o \
./src/ParseTorrent.o \
./src/bt_lib.o \
./src/bt_setup.o 

CPP_DEPS += \
./src/BitTorrent.d \
./src/ParseTorrent.d \
./src/bt_lib.d \
./src/bt_setup.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


