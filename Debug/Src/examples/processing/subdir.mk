################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/examples/processing/example_processing_amplitude.c \
../Src/examples/processing/example_processing_coherent_mean.c \
../Src/examples/processing/example_processing_noncoherent_mean.c \
../Src/examples/processing/example_processing_peak_interpolation.c \
../Src/examples/processing/example_processing_static_presence.c \
../Src/examples/processing/example_processing_subtract_adaptive_bg.c 

OBJS += \
./Src/examples/processing/example_processing_amplitude.o \
./Src/examples/processing/example_processing_coherent_mean.o \
./Src/examples/processing/example_processing_noncoherent_mean.o \
./Src/examples/processing/example_processing_peak_interpolation.o \
./Src/examples/processing/example_processing_static_presence.o \
./Src/examples/processing/example_processing_subtract_adaptive_bg.o 

C_DEPS += \
./Src/examples/processing/example_processing_amplitude.d \
./Src/examples/processing/example_processing_coherent_mean.d \
./Src/examples/processing/example_processing_noncoherent_mean.d \
./Src/examples/processing/example_processing_peak_interpolation.d \
./Src/examples/processing/example_processing_static_presence.d \
./Src/examples/processing/example_processing_subtract_adaptive_bg.d 


# Each subdirectory must supply rules for building sources it contributes
Src/examples/processing/%.o Src/examples/processing/%.su Src/examples/processing/%.cyclo: ../Src/examples/processing/%.c Src/examples/processing/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu99 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L431xx -c -I../Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-examples-2f-processing

clean-Src-2f-examples-2f-processing:
	-$(RM) ./Src/examples/processing/example_processing_amplitude.cyclo ./Src/examples/processing/example_processing_amplitude.d ./Src/examples/processing/example_processing_amplitude.o ./Src/examples/processing/example_processing_amplitude.su ./Src/examples/processing/example_processing_coherent_mean.cyclo ./Src/examples/processing/example_processing_coherent_mean.d ./Src/examples/processing/example_processing_coherent_mean.o ./Src/examples/processing/example_processing_coherent_mean.su ./Src/examples/processing/example_processing_noncoherent_mean.cyclo ./Src/examples/processing/example_processing_noncoherent_mean.d ./Src/examples/processing/example_processing_noncoherent_mean.o ./Src/examples/processing/example_processing_noncoherent_mean.su ./Src/examples/processing/example_processing_peak_interpolation.cyclo ./Src/examples/processing/example_processing_peak_interpolation.d ./Src/examples/processing/example_processing_peak_interpolation.o ./Src/examples/processing/example_processing_peak_interpolation.su ./Src/examples/processing/example_processing_static_presence.cyclo ./Src/examples/processing/example_processing_static_presence.d ./Src/examples/processing/example_processing_static_presence.o ./Src/examples/processing/example_processing_static_presence.su ./Src/examples/processing/example_processing_subtract_adaptive_bg.cyclo ./Src/examples/processing/example_processing_subtract_adaptive_bg.d ./Src/examples/processing/example_processing_subtract_adaptive_bg.o ./Src/examples/processing/example_processing_subtract_adaptive_bg.su

.PHONY: clean-Src-2f-examples-2f-processing

