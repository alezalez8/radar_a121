################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/examples/troubleshooting/example_diagnostic_test.c 

OBJS += \
./Src/examples/troubleshooting/example_diagnostic_test.o 

C_DEPS += \
./Src/examples/troubleshooting/example_diagnostic_test.d 


# Each subdirectory must supply rules for building sources it contributes
Src/examples/troubleshooting/%.o Src/examples/troubleshooting/%.su Src/examples/troubleshooting/%.cyclo: ../Src/examples/troubleshooting/%.c Src/examples/troubleshooting/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu99 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L431xx -c -I../Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-examples-2f-troubleshooting

clean-Src-2f-examples-2f-troubleshooting:
	-$(RM) ./Src/examples/troubleshooting/example_diagnostic_test.cyclo ./Src/examples/troubleshooting/example_diagnostic_test.d ./Src/examples/troubleshooting/example_diagnostic_test.o ./Src/examples/troubleshooting/example_diagnostic_test.su

.PHONY: clean-Src-2f-examples-2f-troubleshooting

