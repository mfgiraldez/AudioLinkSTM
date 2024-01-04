################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Utilities/Log/lcd_log.c 

OBJS += \
./Utilities/Log/lcd_log.o 

C_DEPS += \
./Utilities/Log/lcd_log.d 


# Each subdirectory must supply rules for building sources it contributes
Utilities/Log/%.o Utilities/Log/%.su Utilities/Log/%.cyclo: ../Utilities/Log/%.c Utilities/Log/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F750xx -DUSE_STM32F7508_DISCO -DUSE_IOEXPANDER -DUSE_USB_FS -c -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/BSP/STM32F7508-Discovery -I../Drivers/BSP/Components/Common -I../Utilities/Log -I../Utilities/Fonts -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -I../Middlewares/Third_Party/FatFs/src -I../Drivers/CMSIS/Include -I../Inc -Os -ffunction-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Utilities-2f-Log

clean-Utilities-2f-Log:
	-$(RM) ./Utilities/Log/lcd_log.cyclo ./Utilities/Log/lcd_log.d ./Utilities/Log/lcd_log.o ./Utilities/Log/lcd_log.su

.PHONY: clean-Utilities-2f-Log

