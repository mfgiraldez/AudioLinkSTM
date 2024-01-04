################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Src/startup_stm32f750xx.s 

C_SRCS += \
../Src/FirFilter.c \
../Src/explorer.c \
../Src/main.c \
../Src/menu.c \
../Src/stm32f7xx_it.c \
../Src/system_stm32f7xx.c \
../Src/transmitter.c \
../Src/usbh_conf.c \
../Src/usbh_diskio.c \
../Src/waverecorder.c 

OBJS += \
./Src/FirFilter.o \
./Src/explorer.o \
./Src/main.o \
./Src/menu.o \
./Src/startup_stm32f750xx.o \
./Src/stm32f7xx_it.o \
./Src/system_stm32f7xx.o \
./Src/transmitter.o \
./Src/usbh_conf.o \
./Src/usbh_diskio.o \
./Src/waverecorder.o 

S_DEPS += \
./Src/startup_stm32f750xx.d 

C_DEPS += \
./Src/FirFilter.d \
./Src/explorer.d \
./Src/main.d \
./Src/menu.d \
./Src/stm32f7xx_it.d \
./Src/system_stm32f7xx.d \
./Src/transmitter.d \
./Src/usbh_conf.d \
./Src/usbh_diskio.d \
./Src/waverecorder.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F750xx -DUSE_STM32F7508_DISCO -DUSE_IOEXPANDER -DUSE_USB_FS -c -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/BSP/STM32F7508-Discovery -I../Drivers/BSP/Components/Common -I../Utilities/Log -I../Utilities/Fonts -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -I../Middlewares/Third_Party/FatFs/src -I../Drivers/CMSIS/Include -I../Inc -Os -ffunction-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/%.o: ../Src/%.s Src/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m7 -g3 -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/FirFilter.cyclo ./Src/FirFilter.d ./Src/FirFilter.o ./Src/FirFilter.su ./Src/explorer.cyclo ./Src/explorer.d ./Src/explorer.o ./Src/explorer.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/menu.cyclo ./Src/menu.d ./Src/menu.o ./Src/menu.su ./Src/startup_stm32f750xx.d ./Src/startup_stm32f750xx.o ./Src/stm32f7xx_it.cyclo ./Src/stm32f7xx_it.d ./Src/stm32f7xx_it.o ./Src/stm32f7xx_it.su ./Src/system_stm32f7xx.cyclo ./Src/system_stm32f7xx.d ./Src/system_stm32f7xx.o ./Src/system_stm32f7xx.su ./Src/transmitter.cyclo ./Src/transmitter.d ./Src/transmitter.o ./Src/transmitter.su ./Src/usbh_conf.cyclo ./Src/usbh_conf.d ./Src/usbh_conf.o ./Src/usbh_conf.su ./Src/usbh_diskio.cyclo ./Src/usbh_diskio.d ./Src/usbh_diskio.o ./Src/usbh_diskio.su ./Src/waverecorder.cyclo ./Src/waverecorder.d ./Src/waverecorder.o ./Src/waverecorder.su

.PHONY: clean-Src

