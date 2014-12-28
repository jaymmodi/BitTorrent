################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../NetworkSocket.o \
../ParseTorrent.o \
../bt_lib.o \
../bt_setup.o 

CPP_SRCS += \
../BitTorrent.cpp \
../NetworkSocket.cpp \
../ParseTorrent.cpp \
../bt_lib.cpp \
../bt_setup.cpp 

OBJS += \
./BitTorrent.o \
./NetworkSocket.o \
./ParseTorrent.o \
./bt_lib.o \
./bt_setup.o 

CPP_DEPS += \
./BitTorrent.d \
./NetworkSocket.d \
./ParseTorrent.d \
./bt_lib.d \
./bt_setup.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


