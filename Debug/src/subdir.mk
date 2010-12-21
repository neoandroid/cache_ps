################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../src/cache.o 

CPP_SRCS += \
../src/cache.cpp \
../src/cache_FIFO.cpp \
../src/cache_LRU.cpp \
../src/driver_cache.cpp \
../src/file.cpp 

OBJS += \
./src/cache.o \
./src/cache_FIFO.o \
./src/cache_LRU.o \
./src/driver_cache.o \
./src/file.o 

CPP_DEPS += \
./src/cache.d \
./src/cache_FIFO.d \
./src/cache_LRU.d \
./src/driver_cache.d \
./src/file.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/neo/workspace/cache/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


