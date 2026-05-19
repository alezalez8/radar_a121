################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/examples/advanced_control/example_detector_distance_calibration_caching.c \
../Src/examples/advanced_control/example_detector_distance_low_power_hibernate.c \
../Src/examples/advanced_control/example_detector_distance_low_power_off.c \
../Src/examples/advanced_control/example_detector_distance_with_iq_data_print.c \
../Src/examples/advanced_control/example_detector_presence_low_power_hibernate.c \
../Src/examples/advanced_control/example_detector_presence_low_power_off.c \
../Src/examples/advanced_control/example_detector_presence_multiple_configurations.c \
../Src/examples/advanced_control/example_detector_presence_with_iq_data_print.c \
../Src/examples/advanced_control/example_service_calibration_caching.c \
../Src/examples/advanced_control/example_service_low_power_sensor_disable.c \
../Src/examples/advanced_control/example_service_low_power_sensor_hibernate.c \
../Src/examples/advanced_control/example_service_multiple_configurations.c \
../Src/examples/advanced_control/example_service_sensor_disable.c \
../Src/examples/advanced_control/example_service_sensor_hibernate.c \
../Src/examples/advanced_control/example_service_sensor_off.c \
../Src/examples/advanced_control/example_service_subsweeps.c 

OBJS += \
./Src/examples/advanced_control/example_detector_distance_calibration_caching.o \
./Src/examples/advanced_control/example_detector_distance_low_power_hibernate.o \
./Src/examples/advanced_control/example_detector_distance_low_power_off.o \
./Src/examples/advanced_control/example_detector_distance_with_iq_data_print.o \
./Src/examples/advanced_control/example_detector_presence_low_power_hibernate.o \
./Src/examples/advanced_control/example_detector_presence_low_power_off.o \
./Src/examples/advanced_control/example_detector_presence_multiple_configurations.o \
./Src/examples/advanced_control/example_detector_presence_with_iq_data_print.o \
./Src/examples/advanced_control/example_service_calibration_caching.o \
./Src/examples/advanced_control/example_service_low_power_sensor_disable.o \
./Src/examples/advanced_control/example_service_low_power_sensor_hibernate.o \
./Src/examples/advanced_control/example_service_multiple_configurations.o \
./Src/examples/advanced_control/example_service_sensor_disable.o \
./Src/examples/advanced_control/example_service_sensor_hibernate.o \
./Src/examples/advanced_control/example_service_sensor_off.o \
./Src/examples/advanced_control/example_service_subsweeps.o 

C_DEPS += \
./Src/examples/advanced_control/example_detector_distance_calibration_caching.d \
./Src/examples/advanced_control/example_detector_distance_low_power_hibernate.d \
./Src/examples/advanced_control/example_detector_distance_low_power_off.d \
./Src/examples/advanced_control/example_detector_distance_with_iq_data_print.d \
./Src/examples/advanced_control/example_detector_presence_low_power_hibernate.d \
./Src/examples/advanced_control/example_detector_presence_low_power_off.d \
./Src/examples/advanced_control/example_detector_presence_multiple_configurations.d \
./Src/examples/advanced_control/example_detector_presence_with_iq_data_print.d \
./Src/examples/advanced_control/example_service_calibration_caching.d \
./Src/examples/advanced_control/example_service_low_power_sensor_disable.d \
./Src/examples/advanced_control/example_service_low_power_sensor_hibernate.d \
./Src/examples/advanced_control/example_service_multiple_configurations.d \
./Src/examples/advanced_control/example_service_sensor_disable.d \
./Src/examples/advanced_control/example_service_sensor_hibernate.d \
./Src/examples/advanced_control/example_service_sensor_off.d \
./Src/examples/advanced_control/example_service_subsweeps.d 


# Each subdirectory must supply rules for building sources it contributes
Src/examples/advanced_control/%.o Src/examples/advanced_control/%.su Src/examples/advanced_control/%.cyclo: ../Src/examples/advanced_control/%.c Src/examples/advanced_control/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu99 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L431xx -c -I../Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-examples-2f-advanced_control

clean-Src-2f-examples-2f-advanced_control:
	-$(RM) ./Src/examples/advanced_control/example_detector_distance_calibration_caching.cyclo ./Src/examples/advanced_control/example_detector_distance_calibration_caching.d ./Src/examples/advanced_control/example_detector_distance_calibration_caching.o ./Src/examples/advanced_control/example_detector_distance_calibration_caching.su ./Src/examples/advanced_control/example_detector_distance_low_power_hibernate.cyclo ./Src/examples/advanced_control/example_detector_distance_low_power_hibernate.d ./Src/examples/advanced_control/example_detector_distance_low_power_hibernate.o ./Src/examples/advanced_control/example_detector_distance_low_power_hibernate.su ./Src/examples/advanced_control/example_detector_distance_low_power_off.cyclo ./Src/examples/advanced_control/example_detector_distance_low_power_off.d ./Src/examples/advanced_control/example_detector_distance_low_power_off.o ./Src/examples/advanced_control/example_detector_distance_low_power_off.su ./Src/examples/advanced_control/example_detector_distance_with_iq_data_print.cyclo ./Src/examples/advanced_control/example_detector_distance_with_iq_data_print.d ./Src/examples/advanced_control/example_detector_distance_with_iq_data_print.o ./Src/examples/advanced_control/example_detector_distance_with_iq_data_print.su ./Src/examples/advanced_control/example_detector_presence_low_power_hibernate.cyclo ./Src/examples/advanced_control/example_detector_presence_low_power_hibernate.d ./Src/examples/advanced_control/example_detector_presence_low_power_hibernate.o ./Src/examples/advanced_control/example_detector_presence_low_power_hibernate.su ./Src/examples/advanced_control/example_detector_presence_low_power_off.cyclo ./Src/examples/advanced_control/example_detector_presence_low_power_off.d ./Src/examples/advanced_control/example_detector_presence_low_power_off.o ./Src/examples/advanced_control/example_detector_presence_low_power_off.su ./Src/examples/advanced_control/example_detector_presence_multiple_configurations.cyclo ./Src/examples/advanced_control/example_detector_presence_multiple_configurations.d ./Src/examples/advanced_control/example_detector_presence_multiple_configurations.o ./Src/examples/advanced_control/example_detector_presence_multiple_configurations.su ./Src/examples/advanced_control/example_detector_presence_with_iq_data_print.cyclo ./Src/examples/advanced_control/example_detector_presence_with_iq_data_print.d ./Src/examples/advanced_control/example_detector_presence_with_iq_data_print.o ./Src/examples/advanced_control/example_detector_presence_with_iq_data_print.su ./Src/examples/advanced_control/example_service_calibration_caching.cyclo ./Src/examples/advanced_control/example_service_calibration_caching.d ./Src/examples/advanced_control/example_service_calibration_caching.o ./Src/examples/advanced_control/example_service_calibration_caching.su ./Src/examples/advanced_control/example_service_low_power_sensor_disable.cyclo ./Src/examples/advanced_control/example_service_low_power_sensor_disable.d ./Src/examples/advanced_control/example_service_low_power_sensor_disable.o ./Src/examples/advanced_control/example_service_low_power_sensor_disable.su ./Src/examples/advanced_control/example_service_low_power_sensor_hibernate.cyclo ./Src/examples/advanced_control/example_service_low_power_sensor_hibernate.d ./Src/examples/advanced_control/example_service_low_power_sensor_hibernate.o ./Src/examples/advanced_control/example_service_low_power_sensor_hibernate.su ./Src/examples/advanced_control/example_service_multiple_configurations.cyclo ./Src/examples/advanced_control/example_service_multiple_configurations.d ./Src/examples/advanced_control/example_service_multiple_configurations.o ./Src/examples/advanced_control/example_service_multiple_configurations.su ./Src/examples/advanced_control/example_service_sensor_disable.cyclo ./Src/examples/advanced_control/example_service_sensor_disable.d ./Src/examples/advanced_control/example_service_sensor_disable.o ./Src/examples/advanced_control/example_service_sensor_disable.su ./Src/examples/advanced_control/example_service_sensor_hibernate.cyclo ./Src/examples/advanced_control/example_service_sensor_hibernate.d ./Src/examples/advanced_control/example_service_sensor_hibernate.o ./Src/examples/advanced_control/example_service_sensor_hibernate.su ./Src/examples/advanced_control/example_service_sensor_off.cyclo ./Src/examples/advanced_control/example_service_sensor_off.d ./Src/examples/advanced_control/example_service_sensor_off.o ./Src/examples/advanced_control/example_service_sensor_off.su ./Src/examples/advanced_control/example_service_subsweeps.cyclo ./Src/examples/advanced_control/example_service_subsweeps.d ./Src/examples/advanced_control/example_service_subsweeps.o ./Src/examples/advanced_control/example_service_subsweeps.su

.PHONY: clean-Src-2f-examples-2f-advanced_control

