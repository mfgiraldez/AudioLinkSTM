################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_audio.c \
../Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_lcd.c \
../Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_mod.c \
../Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_sdram.c \
../Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_ts.c 

OBJS += \
./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_audio.o \
./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_lcd.o \
./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_mod.o \
./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_sdram.o \
./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_ts.o 

C_DEPS += \
./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_audio.d \
./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_lcd.d \
./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_mod.d \
./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_sdram.d \
./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_ts.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/STM32F7508-Discovery/%.o Drivers/BSP/STM32F7508-Discovery/%.su Drivers/BSP/STM32F7508-Discovery/%.cyclo: ../Drivers/BSP/STM32F7508-Discovery/%.c Drivers/BSP/STM32F7508-Discovery/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F750xx -DUSE_STM32F7508_DISCO -DUSE_IOEXPANDER -DUSE_USB_FS -c -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/BSP/STM32F7508-Discovery -I../Drivers/BSP/Components/Common -I../Utilities/Log -I../Utilities/Fonts -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -I../Middlewares/Third_Party/FatFs/src -I../Drivers/CMSIS/Include -I../Inc -Os -ffunction-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-STM32F7508-2d-Discovery

clean-Drivers-2f-BSP-2f-STM32F7508-2d-Discovery:
	-$(RM) ./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_audio.cyclo ./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_audio.d ./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_audio.o ./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_audio.su ./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_lcd.cyclo ./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_lcd.d ./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_lcd.o ./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_lcd.su ./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_mod.cyclo ./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_mod.d ./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_mod.o ./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_mod.su ./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_sdram.cyclo ./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_sdram.d ./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_sdram.o ./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_sdram.su ./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_ts.cyclo ./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_ts.d ./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_ts.o ./Drivers/BSP/STM32F7508-Discovery/stm32f7508_discovery_ts.su

.PHONY: clean-Drivers-2f-BSP-2f-STM32F7508-2d-Discovery

