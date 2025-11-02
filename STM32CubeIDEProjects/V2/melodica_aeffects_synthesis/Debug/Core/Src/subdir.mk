################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/ADSR.c \
../Core/Src/audio.c \
../Core/Src/controlstate.c \
../Core/Src/filter.c \
../Core/Src/main.c \
../Core/Src/midi.c \
../Core/Src/oscillator.c \
../Core/Src/signal.c \
../Core/Src/stm32h7xx_hal_msp.c \
../Core/Src/stm32h7xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32h7xx.c \
../Core/Src/voice.c 

OBJS += \
./Core/Src/ADSR.o \
./Core/Src/audio.o \
./Core/Src/controlstate.o \
./Core/Src/filter.o \
./Core/Src/main.o \
./Core/Src/midi.o \
./Core/Src/oscillator.o \
./Core/Src/signal.o \
./Core/Src/stm32h7xx_hal_msp.o \
./Core/Src/stm32h7xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32h7xx.o \
./Core/Src/voice.o 

C_DEPS += \
./Core/Src/ADSR.d \
./Core/Src/audio.d \
./Core/Src/controlstate.d \
./Core/Src/filter.d \
./Core/Src/main.d \
./Core/Src/midi.d \
./Core/Src/oscillator.d \
./Core/Src/signal.d \
./Core/Src/stm32h7xx_hal_msp.d \
./Core/Src/stm32h7xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32h7xx.d \
./Core/Src/voice.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H750xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/ADSR.cyclo ./Core/Src/ADSR.d ./Core/Src/ADSR.o ./Core/Src/ADSR.su ./Core/Src/audio.cyclo ./Core/Src/audio.d ./Core/Src/audio.o ./Core/Src/audio.su ./Core/Src/controlstate.cyclo ./Core/Src/controlstate.d ./Core/Src/controlstate.o ./Core/Src/controlstate.su ./Core/Src/filter.cyclo ./Core/Src/filter.d ./Core/Src/filter.o ./Core/Src/filter.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/midi.cyclo ./Core/Src/midi.d ./Core/Src/midi.o ./Core/Src/midi.su ./Core/Src/oscillator.cyclo ./Core/Src/oscillator.d ./Core/Src/oscillator.o ./Core/Src/oscillator.su ./Core/Src/signal.cyclo ./Core/Src/signal.d ./Core/Src/signal.o ./Core/Src/signal.su ./Core/Src/stm32h7xx_hal_msp.cyclo ./Core/Src/stm32h7xx_hal_msp.d ./Core/Src/stm32h7xx_hal_msp.o ./Core/Src/stm32h7xx_hal_msp.su ./Core/Src/stm32h7xx_it.cyclo ./Core/Src/stm32h7xx_it.d ./Core/Src/stm32h7xx_it.o ./Core/Src/stm32h7xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32h7xx.cyclo ./Core/Src/system_stm32h7xx.d ./Core/Src/system_stm32h7xx.o ./Core/Src/system_stm32h7xx.su ./Core/Src/voice.cyclo ./Core/Src/voice.d ./Core/Src/voice.o ./Core/Src/voice.su

.PHONY: clean-Core-2f-Src

