################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../src/Untitled\ Folder/ParseTorrent.o \
../src/Untitled\ Folder/bt_lib.o \
../src/Untitled\ Folder/bt_setup.o 

CPP_SRCS += \
../src/Untitled\ Folder/BitTorrent.cpp \
../src/Untitled\ Folder/ParseTorrent.cpp \
../src/Untitled\ Folder/bt_lib.cpp \
../src/Untitled\ Folder/bt_setup.cpp 

OBJS += \
./src/Untitled\ Folder/BitTorrent.o \
./src/Untitled\ Folder/ParseTorrent.o \
./src/Untitled\ Folder/bt_lib.o \
./src/Untitled\ Folder/bt_setup.o 

CPP_DEPS += \
./src/Untitled\ Folder/BitTorrent.d \
./src/Untitled\ Folder/ParseTorrent.d \
./src/Untitled\ Folder/bt_lib.d \
./src/Untitled\ Folder/bt_setup.d 


# Each subdirectory must supply rules for building sources it contributes
src/Untitled\ Folder/BitTorrent.o: ../src/Untitled\ Folder/BitTorrent.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/Untitled Folder/BitTorrent.d" -MT"src/Untitled\ Folder/BitTorrent.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Untitled\ Folder/ParseTorrent.o: ../src/Untitled\ Folder/ParseTorrent.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/Untitled Folder/ParseTorrent.d" -MT"src/Untitled\ Folder/ParseTorrent.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Untitled\ Folder/bt_lib.o: ../src/Untitled\ Folder/bt_lib.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/Untitled Folder/bt_lib.d" -MT"src/Untitled\ Folder/bt_lib.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Untitled\ Folder/bt_setup.o: ../src/Untitled\ Folder/bt_setup.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/Untitled Folder/bt_setup.d" -MT"src/Untitled\ Folder/bt_setup.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


