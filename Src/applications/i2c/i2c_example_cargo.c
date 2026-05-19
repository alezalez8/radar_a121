// Copyright (c) Acconeer AB, 2025
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <complex.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "acc_definitions_a121.h"
#include "acc_hal_definitions_a121.h"
#include "acc_hal_integration_a121.h"
#include "acc_integration.h"
#include "acc_integration_log.h"
#include "acc_rss_a121.h"
#include "acc_version.h"
#include "example_cargo.h"

#include "acc_reg_protocol.h"
#include "example_cargo_reg_protocol.h"
#include "i2c_application_system.h"
#include "i2c_example_cargo.h"

#define SENSOR_ID         (1U)
#define SENSOR_TIMEOUT_MS (10000U)

static struct
{
	acc_sensor_t                     *sensor;
	cargo_config_t                    config;
	cargo_handle_t                   *handle;
	acc_cal_result_t                  sensor_cal_result;
	acc_detector_cal_result_dynamic_t distance_cal_result_dynamic;
	uint8_t                          *distance_cal_result_static;
	uint32_t                          distance_cal_result_static_size;
	void                             *buffer;
	uint32_t                          buffer_size;

	cargo_result_t result;
	int16_t        most_recent_temperature;
	bool           utilization_result_available;
} g_resources;

static struct
{
	uint32_t i2c_application_command;
	uint32_t i2c_application_status;
	uint32_t measure_counter;

	uint32_t presence_last_tick_ms;
	uint32_t presence_estimated_frame_rate_mhz;

	bool uart_logs_enabed;
} g_application_state;

static struct
{
	bool  presence_valid;
	bool  presence_detected;
	float max_inter_presence_score;
	float max_intra_presence_score;
} g_presence_burst_result;

#define UART_LOG_BUFFER_SIZE 100

/**
 * @brief Get the i2c_detector_command values
 *
 * The i2c_application_command is cleared during this read
 * The read and clear are protected by a critical section
 *
 * @return The command sent from the host
 */
static uint32_t get_command(void);

/**
 * @brief Set bits in the i2c_application_status
 *
 * The i2c_application_status is protected by a critical section
 *
 * @param[in] bit_mask The bit_mask to set
 */
static void application_status_set_bits(uint32_t bit_mask);

/**
 * @brief Clear bits in the i2c_application_status
 *
 * The i2c_application_status is protected by a critical section
 *
 * @param[in] bit_mask The bit_mask to clear
 */
static void application_status_clr_bits(uint32_t bit_mask);

/**
 * @brief Test bits in the i2c_application_status
 *
 * The i2c_application_status is protected by a critical section
 *
 * @param[in] bit_mask The bit_mask to test
 * @return true if all the bits in bit_mask is set in i2c_application_status
 */
static bool application_status_test_bits(uint32_t bit_mask);

/**
 * @brief Create sensor
 */
static void create_sensor(void);

/**
 * @brief Calibrate sensor
 *
 * @return true if successful
 */
static bool calibrate_sensor(void);

/**
 * @brief Performs cargo calibration
 */
static bool do_cargo_calibration(bool full_calibration);

/**
 * @brief Enter sensor hibernation state
 *
 * @ref exit_hibernate needs to be called before the sensor can measure again.
 */
static bool enter_hibernation(void);

/**
 * @brief Exit sensor hibernation state
 */
static bool exit_hibernation(void);

/**
 * @brief Validate Cargo config for this I2C application
 *
 * @return true if config is valid
 */
static bool validate_application_config_for_i2c(void);

/**
 * @brief Apply config
 *
 * This function will create the distance- & presence detector and
 * allocate the needed memory
 */
static void apply_application_config(void);

/**
 * @brief Get next utilization level measurement
 *
 * @return true if successful
 */
static bool utilization_get_next(void);

/**
 * @brief Get next presence measurement
 *
 * @return true if successful
 */
static bool presence_get_next(void);

/**
 * @brief Reset presence burst result
 */
static void presence_reset_burst_result(void);

/**
 * @brief Update presence brust result
 */
static void presence_update_burst_result(const cargo_result_t *cargo_result);

/**
 * @brief Increment measure counter by one
 */
static void increment_measure_counter(void);

/**
 * @brief Update presence frame estimation
 */
static void update_presence_update_rate_estimate(void);

/**
 * @brief Set most recent temperature to the passed temperature
 */
static void update_most_recent_temperature(int16_t temperature);

/**
 * @brief Try to set module in low power mode
 */
static void module_low_power(void);

/**
 * @brief Print the application result
 *
 * Only available when the UART logs have been enabled with ENABLE_UART_LOGS
 *
 * @param[in] result The cargo result
 */
static void log_cargo_result_on_uart(const cargo_result_t *result);

/**
 * @brief UART logging function (can be enabled/disabled by command)
 */
static void uart_log(const char *format, ...);

//
// PUBLIC FUNCTIONS
//

cargo_config_t *i2c_example_cargo_get_config(void)
{
	return &(g_resources.config);
}

bool i2c_example_cargo_command(uint32_t command)
{
	bool status = false;

	/* Make sure we do not have a race for i2c_application_command/i2c_application_status */
	acc_integration_critical_section_enter();

	if (g_application_state.i2c_application_command == 0U)
	{
		/* Set Ready PIN to LOW while processing the command */
		i2c_application_system_set_ready_pin(false);

		/* Set status BUSY bit */
		g_application_state.i2c_application_status  |= EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_BUSY_MASK;
		g_application_state.i2c_application_command  = command;
		status                                       = true;
	}

	acc_integration_critical_section_exit();
	return status;
}

uint32_t i2c_example_cargo_get_status(void)
{
	/* Make sure we do not have a race for i2c_application_status */
	acc_integration_critical_section_enter();

	uint32_t status = g_application_state.i2c_application_status;

	acc_integration_critical_section_exit();

	return status;
}

uint32_t i2c_example_cargo_get_measure_counter(void)
{
	acc_integration_critical_section_enter();

	uint32_t measure_counter = g_application_state.measure_counter;

	acc_integration_critical_section_exit();

	return measure_counter;
}

uint32_t i2c_example_cargo_get_actual_presence_update_rate(void)
{
	acc_integration_critical_section_enter();

	uint32_t estimated_update_rate_mhz = g_application_state.presence_estimated_frame_rate_mhz;

	acc_integration_critical_section_exit();

	return estimated_update_rate_mhz;
}

void i2c_example_cargo_copy_utilization_result_fields(cargo_result_t *dst_result)
{
	acc_integration_critical_section_enter();

	dst_result->presence_valid = false;

	dst_result->utilization_valid = g_resources.result.utilization_valid;
	dst_result->distance          = g_resources.result.distance;
	dst_result->level_m           = g_resources.result.level_m;
	dst_result->level_percent     = g_resources.result.level_percent;

	acc_integration_critical_section_exit();
}

void i2c_example_cargo_copy_presence_burst_result_fields(cargo_result_t *dst_result)
{
	acc_integration_critical_section_enter();

	dst_result->utilization_valid = false;

	dst_result->presence_valid       = g_presence_burst_result.presence_valid;
	dst_result->presence_detected    = g_presence_burst_result.presence_detected;
	dst_result->inter_presence_score = g_presence_burst_result.max_inter_presence_score;
	dst_result->intra_presence_score = g_presence_burst_result.max_intra_presence_score;

	acc_integration_critical_section_exit();
}

int16_t i2c_example_cargo_get_temperature(void)
{
	acc_integration_critical_section_enter();

	int16_t temperature = g_resources.most_recent_temperature;

	acc_integration_critical_section_exit();

	return temperature;
}

//
// MAIN
//

int acconeer_main(int argc, char *argv[]);

int acconeer_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	bool setup_status = true;

	printf("I2C Example Cargo\n");
	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_a121_t *hal = acc_hal_rss_integration_get_implementation();

	if (acc_rss_hal_register(hal))
	{
		application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_RSS_REGISTER_OK_MASK);
	}
	else
	{
		printf("ERROR: acc_rss_hal_register() failed\n\n");
		application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_APPLICATION_ERROR_MASK |
		                            EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_RSS_REGISTER_OK_MASK);
		setup_status = false;
	}

	if (setup_status)
	{
		cargo_config_initialize(&g_resources.config, CARGO_PRESET_NO_LENS);
	}

	/* Turn the sensor on */
	acc_hal_integration_sensor_supply_on(SENSOR_ID);

	if (setup_status)
	{
		/* Create sensor */
		create_sensor();
	}

	i2c_application_system_init();

	/* Setup i2c register protocol */
	example_cargo_reg_protocol_setup();

	while (true)
	{

		/* Handle Commands */
		uint32_t command = get_command();

		if (command == 0) // no command
		{
			if (i2c_application_system_test_wakeup_pin())
			{
				/* If wakeup is HIGH do WFI */
				i2c_application_system_wait_for_interrupt();
			}
			else
			{
				/* If wakeup is LOW do SLEEP */

				/* Set ready pin LOW, we are about to power down */
				i2c_application_system_set_ready_pin(false);

				uart_log("Enter low power state\n");
				i2c_application_enter_low_power_state();

				/* We will restart here when WAKEUP goes HIGH */

				uart_log("Exit low power state\n");
			}
			i2c_application_system_set_ready_pin(true);

			/* No command to process */
			continue;
		}
		/* Special command, always handle reset module command, even if error has occured */
		if (command == EXAMPLE_CARGO_REG_COMMAND_ENUM_RESET_MODULE)
		{
			/* Reset system */
			i2c_application_system_reset();
			continue;
		}

		if (application_status_test_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_APPLICATION_ERROR_MASK))
		{
			/* Do not process commands after error state */
			continue;
		}

		/* Handle command */
		switch (command)
		{
			case EXAMPLE_CARGO_REG_COMMAND_ENUM_APPLY_CONFIGURATION:
				if (!application_status_test_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CONFIG_APPLY_OK_MASK))
				{
					// Apply configuration
					apply_application_config();
				}
				break;
			case EXAMPLE_CARGO_REG_COMMAND_ENUM_MEASURE_UTILIZATION_LEVEL:
				acc_hal_integration_sensor_enable(SENSOR_ID);

				if (utilization_get_next())
				{
					log_cargo_result_on_uart(&g_resources.result);
				}
				else
				{
					printf("ERROR: Could not get next result\n");
					application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_APPLICATION_ERROR_MASK);
				}

				acc_hal_integration_sensor_disable(SENSOR_ID);
				break;
			case EXAMPLE_CARGO_REG_COMMAND_ENUM_MEASURE_PRESENCE:
				acc_hal_integration_sensor_enable(SENSOR_ID);

				if (!cargo_prepare_presence(g_resources.handle,
				                            g_resources.sensor,
				                            &g_resources.sensor_cal_result,
				                            g_resources.buffer,
				                            g_resources.buffer_size))
				{
					printf("ERROR: cargo_prepare_presence() failed\n");
					application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_APPLICATION_ERROR_MASK);
				}
				else
				{
					if (!enter_hibernation())
					{
						application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_APPLICATION_ERROR_MASK);
					}
					else
					{
						uint32_t sleep_time_ms = (uint32_t)(1000.0f / g_resources.config.presence.update_rate);
						i2c_application_set_periodic_wakeup(sleep_time_ms);

						presence_reset_burst_result();

						do
						{
							module_low_power();

							if (i2c_application_is_periodic_wakeup())
							{
								update_presence_update_rate_estimate();

								if (presence_get_next())
								{
									presence_update_burst_result(&g_resources.result);
									log_cargo_result_on_uart(&g_resources.result);
								}
								else
								{
									printf("ERROR: Could not get next result\n");
									application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_APPLICATION_ERROR_MASK);
								}
							}

						} while (!application_status_test_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_APPLICATION_ERROR_MASK) &&
						         (cargo_current_mode_get(g_resources.handle) == CARGO_MODE_PRESENCE));

						exit_hibernation();
					}
				}

				i2c_application_set_periodic_wakeup(0U); // disable periodic wakeup

				acc_hal_integration_sensor_disable(SENSOR_ID);
				break;
			case EXAMPLE_CARGO_REG_COMMAND_ENUM_ENABLE_UART_LOGS:
				g_application_state.uart_logs_enabed = true;
				uart_log("UART logs enabled\n");
				break;
			case EXAMPLE_CARGO_REG_COMMAND_ENUM_DISABLE_UART_LOGS:
				g_application_state.uart_logs_enabed = false;
				uart_log("UART logs disabled\n");
				break;
			case EXAMPLE_CARGO_REG_COMMAND_ENUM_LOG_CONFIGURATION:
				cargo_config_log(&g_resources.config);
				break;
			default:
				printf("ERROR: Unknown command: %" PRIu32 "", command);
				break;
		}

		/* Command handling done, clear busy bit */
		application_status_clr_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_BUSY_MASK);

		/* Set Ready PIN to HIGH when command processing is done */
		i2c_application_system_set_ready_pin(true);
	}

	return EXIT_FAILURE;
}

//
// PRIVATE HELPER FUNCTIONS
//

static uint32_t get_command(void)
{
	/* Make sure we do not have a race for i2c_application_command */
	acc_integration_critical_section_enter();

	uint32_t command = g_application_state.i2c_application_command;

	g_application_state.i2c_application_command = 0U;

	acc_integration_critical_section_exit();

	return command;
}

static void application_status_set_bits(uint32_t bit_mask)
{
	/* Make sure we do not have a race for i2c_application_status */
	acc_integration_critical_section_enter();

	g_application_state.i2c_application_status |= bit_mask;
	uint32_t temp_application_status            = g_application_state.i2c_application_status;

	acc_integration_critical_section_exit();

	uart_log("Application Status = 0x%" PRIx32 "\n", temp_application_status);
}

static void application_status_clr_bits(uint32_t bit_mask)
{
	/* Make sure we do not have a race for i2c_application_status */
	acc_integration_critical_section_enter();

	g_application_state.i2c_application_status &= ~bit_mask;
	uint32_t temp_application_status            = g_application_state.i2c_application_status;

	acc_integration_critical_section_exit();

	uart_log("Application Status = 0x%" PRIx32 "\n", temp_application_status);
}

static bool application_status_test_bits(uint32_t bit_mask)
{
	/* Make sure we do not have a race for i2c_application_status */
	acc_integration_critical_section_enter();

	bool status = (g_application_state.i2c_application_status & bit_mask) == bit_mask;

	acc_integration_critical_section_exit();

	return status;
}

static void create_sensor(void)
{
	acc_hal_integration_sensor_enable(SENSOR_ID);

	g_resources.sensor = acc_sensor_create(SENSOR_ID);

	acc_hal_integration_sensor_disable(SENSOR_ID);

	if (g_resources.sensor != NULL)
	{
		application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_SENSOR_CREATE_OK_MASK);
	}
	else
	{
		application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_APPLICATION_ERROR_MASK |
		                            EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_SENSOR_CREATE_ERROR_MASK);
		printf("ERROR: acc_sensor_create() failed\n");
	}
}

static bool calibrate_sensor(void)
{
	acc_hal_integration_sensor_enable(SENSOR_ID);

	application_status_clr_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_SENSOR_CALIBRATE_OK_MASK |
	                            EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_SENSOR_CALIBRATE_ERROR_MASK);

	bool status;
	bool cal_complete = false;

	do
	{
		status = acc_sensor_calibrate(g_resources.sensor, &cal_complete, &g_resources.sensor_cal_result, g_resources.buffer, g_resources.buffer_size);

		if (cal_complete)
		{
			break;
		}

		if (status)
		{
			status = acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS);
		}
	} while (status);

	if (status)
	{
		application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_SENSOR_CALIBRATE_OK_MASK);
	}
	else
	{
		application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_SENSOR_CALIBRATE_ERROR_MASK);
		printf("ERROR: acc_sensor_calibrate() failed\n");
	}

	/* Reset sensor after calibration by disabling it */
	acc_hal_integration_sensor_disable(SENSOR_ID);

	return status;
}

static bool do_cargo_calibration(bool full_calibration)
{
	bool status           = true;
	bool calibration_done = false;

	application_status_clr_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CARGO_CALIBRATE_OK_MASK |
	                            EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CARGO_CALIBRATE_ERROR_MASK);

	acc_hal_integration_sensor_enable(SENSOR_ID);

	while (status && !calibration_done)
	{
		if (full_calibration)
		{
			status = cargo_calibrate(g_resources.handle,
			                         g_resources.sensor,
			                         &(g_resources.sensor_cal_result),
			                         g_resources.buffer,
			                         g_resources.buffer_size,
			                         g_resources.distance_cal_result_static,
			                         g_resources.distance_cal_result_static_size,
			                         &(g_resources.distance_cal_result_dynamic),
			                         &calibration_done);
		}
		else
		{
			status = cargo_update_calibration(g_resources.handle,
			                                  g_resources.sensor,
			                                  &(g_resources.sensor_cal_result),
			                                  g_resources.buffer,
			                                  g_resources.buffer_size,
			                                  &(g_resources.distance_cal_result_dynamic),
			                                  &calibration_done);
		}

		if (status && !calibration_done)
		{
			status = acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS);
		}
	}

	if (status)
	{
		application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CARGO_CALIBRATE_OK_MASK);
	}
	else
	{
		application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CARGO_CALIBRATE_ERROR_MASK);
	}
	return status;
}

static bool enter_hibernation(void)
{
	bool status = true;

	if (!acc_sensor_hibernate_on(g_resources.sensor))
	{
		printf("ERROR: acc_sensor_hibernate_on failed\n");
		status = false;
	}

	acc_hal_integration_sensor_disable(SENSOR_ID);
	return status;
}

static bool exit_hibernation(void)
{
	bool status = true;

	acc_hal_integration_sensor_enable(SENSOR_ID);
	if (!acc_sensor_hibernate_off(g_resources.sensor))
	{
		printf("ERROR: acc_sensor_hibernate_off failed\n");
		status = false;
	}

	return status;
}

static void apply_application_config(void)
{
	g_resources.config.presence.rate_is_app_driven = true;

	bool status = validate_application_config_for_i2c();

	if (status)
	{
		g_resources.handle = cargo_handle_create(&g_resources.config);
		if (g_resources.handle != NULL)
		{
			application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CARGO_CREATE_OK_MASK);
		}
		else
		{
			application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_APPLICATION_ERROR_MASK |
			                            EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CARGO_CREATE_ERROR_MASK);
			printf("ERROR: cargo_handle_create() failed\n");
			status = false;
		}
	}

	if (status)
	{
		if (cargo_get_buffer_size(g_resources.handle, &g_resources.buffer_size))
		{
			application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_SENSOR_BUFFER_OK_MASK);
		}
		else
		{
			application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_APPLICATION_ERROR_MASK |
			                            EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_SENSOR_BUFFER_ERROR_MASK);
			printf("ERROR: cargo_get_buffer_size() failed\n");
			status = false;
		}
	}

	if (status)
	{
		g_resources.buffer = acc_integration_mem_alloc(g_resources.buffer_size);
		if (g_resources.buffer != NULL)
		{
			application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_SENSOR_BUFFER_OK_MASK);
		}
		else
		{
			application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_APPLICATION_ERROR_MASK |
			                            EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_SENSOR_BUFFER_ERROR_MASK);
			printf("ERROR: sensor buffer allocation failed\n");
			status = false;
		}
	}

	if (status)
	{
		if (cargo_get_distance_cal_result_static_size(g_resources.handle, &(g_resources.distance_cal_result_static_size)))
		{
			application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CARGO_BUFFER_OK_MASK);
		}
		else
		{
			application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_APPLICATION_ERROR_MASK |
			                            EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CARGO_BUFFER_ERROR_MASK);
			printf("ERROR: cargo_get_distance_cal_result_static_size() failed\n");
			status = false;
		}
	}

	if (status && (g_resources.distance_cal_result_static_size > 0U))
	{
		g_resources.distance_cal_result_static = acc_integration_mem_alloc(g_resources.distance_cal_result_static_size);
		if (g_resources.buffer != NULL)
		{
			application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CARGO_BUFFER_OK_MASK);
		}
		else
		{
			application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_APPLICATION_ERROR_MASK |
			                            EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CARGO_BUFFER_ERROR_MASK);
			printf("ERROR: Cargo buffer allocation failed\n");
			status = false;
		}
	}

	if (status)
	{
		status = calibrate_sensor();
	}

	if (status)
	{
		status = do_cargo_calibration(true);
	}

	if (status)
	{
		application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CONFIG_APPLY_OK_MASK);
	}
	else
	{
		printf("ERROR: apply application config failed\n");
		application_status_set_bits(EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_APPLICATION_ERROR_MASK |
		                            EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CONFIG_APPLY_ERROR_MASK);
	}
}

static bool validate_application_config_for_i2c(void)
{
	/* Hibernate is used in this I2C application, which is most optimal <10Hz */
	if (g_resources.config.presence.update_rate > 10.0f)
	{
		printf("Presence update rate should be lower than 10Hz for optimal current consumption.\n");
	}

	return true;
}

static bool utilization_get_next()
{
	bool status = true;

	if (!cargo_prepare_utilization(g_resources.handle,
	                               g_resources.sensor,
	                               &g_resources.sensor_cal_result,
	                               g_resources.buffer,
	                               g_resources.buffer_size))
	{
		status = false;
		printf("ERROR: cargo_prepare_utilization() failed\n");
	}

	if (status)
	{
		if (!acc_sensor_measure(g_resources.sensor))
		{
			printf("ERROR: acc_sensor_measure() failed\n");
			status = false;
		}
	}

	if (status)
	{
		if (!acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS))
		{
			printf("ERROR: Sensor interrupt timeout\n");
			status = false;
		}
	}

	if (status)
	{
		if (!acc_sensor_read(g_resources.sensor, g_resources.buffer, g_resources.buffer_size))
		{
			printf("ERROR: acc_sensor_read() failed\n");
			status = false;
		}
	}

	acc_detector_distance_result_t distance_result = {0};

	if (status)
	{
		bool result_available = false;

		if (!cargo_process_utilization(g_resources.handle,
		                               g_resources.distance_cal_result_static,
		                               &(g_resources.distance_cal_result_dynamic),
		                               g_resources.buffer,
		                               &g_resources.result,
		                               &result_available,
		                               &distance_result))
		{
			printf("ERROR: cargo_process_utilization() failed\n");
			status = false;
		}
		else
		{
			g_resources.utilization_result_available = result_available;
		}
	}

	if (status)
	{
		increment_measure_counter();

		update_most_recent_temperature(distance_result.processing_result->temperature);

		if (g_resources.result.calibration_needed)
		{
			uart_log("Recalibration\n");

			acc_hal_integration_sensor_disable(SENSOR_ID);

			status = calibrate_sensor();

			if (status)
			{
				acc_hal_integration_sensor_enable(SENSOR_ID);
			}

			if (status)
			{
				status = do_cargo_calibration(false); // only a calibration update is needed here.
			}
		}
	}

	return status;
}

static bool presence_get_next(void)
{
	bool status = exit_hibernation();

	if (status)
	{
		if (!acc_sensor_measure(g_resources.sensor))
		{
			printf("ERROR: acc_sensor_measure() failed\n");
			status = false;
		}
	}

	if (status)
	{
		if (!acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS))
		{
			printf("ERROR: Sensor interrupt timeout\n");
			status = false;
		}
	}

	if (status)
	{
		if (!acc_sensor_read(g_resources.sensor, g_resources.buffer, g_resources.buffer_size))
		{
			printf("ERROR: acc_sensor_read() failed\n");
			status = false;
		}
	}

	if (status)
	{
		status = enter_hibernation();
	}

	acc_detector_presence_result_t presence_result = {0};

	if (status)
	{
		if (!cargo_process_presence(g_resources.handle, g_resources.buffer, &g_resources.result, &presence_result))
		{
			printf("ERROR: cargo_process_presence() failed\n");
			status = false;
		}
	}

	if (status)
	{
		increment_measure_counter();

		update_most_recent_temperature(presence_result.processing_result.temperature);

		if (g_resources.result.calibration_needed)
		{
			uart_log("Recalibration\n");

			acc_hal_integration_sensor_disable(SENSOR_ID);

			status = calibrate_sensor();

			if (status)
			{
				acc_hal_integration_sensor_enable(SENSOR_ID);
			}
		}
	}

	return status;
}

static void presence_reset_burst_result(void)
{
	acc_integration_critical_section_enter();

	g_presence_burst_result.presence_valid           = false;
	g_presence_burst_result.presence_detected        = false;
	g_presence_burst_result.max_inter_presence_score = 0.0f;
	g_presence_burst_result.max_intra_presence_score = 0.0f;

	acc_integration_critical_section_exit();
}

static void presence_update_burst_result(const cargo_result_t *cargo_result)
{
	acc_integration_critical_section_enter();

	if (cargo_result->presence_valid)
	{
		g_presence_burst_result.presence_valid = true;

		if (cargo_result->presence_detected)
		{
			g_presence_burst_result.presence_detected = true;
		}

		if (cargo_result->inter_presence_score > g_presence_burst_result.max_inter_presence_score)
		{
			g_presence_burst_result.max_inter_presence_score = cargo_result->inter_presence_score;
		}

		if (cargo_result->intra_presence_score > g_presence_burst_result.max_intra_presence_score)
		{
			g_presence_burst_result.max_intra_presence_score = cargo_result->intra_presence_score;
		}
	}

	acc_integration_critical_section_exit();
}

static void increment_measure_counter(void)
{
	/* Make sure we do not have a race for the measure counter */
	acc_integration_critical_section_enter();

	g_application_state.measure_counter++;

	acc_integration_critical_section_exit();

	printf("measure_counter is now %" PRIu32 "\n", g_application_state.measure_counter);
}

static void update_presence_update_rate_estimate(void)
{
	acc_integration_critical_section_enter();

	uint32_t time_ms                                      = acc_integration_get_time();
	uint32_t diff_ms                                      = time_ms - g_application_state.presence_last_tick_ms;
	g_application_state.presence_last_tick_ms             = time_ms;
	g_application_state.presence_estimated_frame_rate_mhz = (diff_ms > 0) ? (1000000 / diff_ms) : 0;

	acc_integration_critical_section_exit();
}

static void update_most_recent_temperature(int16_t temperature)
{
	acc_integration_critical_section_enter();

	g_resources.most_recent_temperature = temperature;

	acc_integration_critical_section_exit();
}

static void module_low_power(void)
{
	if (i2c_application_system_test_wakeup_pin())
	{
		i2c_application_system_wait_for_interrupt();
	}
	else
	{
		/* Set ready pin LOW, we are about to power down */
		i2c_application_system_set_ready_pin(false);

		uart_log("Enter low power state\n");
		i2c_application_enter_low_power_state();
		uart_log("Exit low power state\n");
	}

	if (i2c_application_system_test_wakeup_pin())
	{
		/* Set ready pin HIGH, we are ready for a command */
		i2c_application_system_set_ready_pin(true);
	}
}

static void log_cargo_result_on_uart(const cargo_result_t *result)
{
	if (g_application_state.uart_logs_enabed)
	{
		if (result->utilization_valid)
		{
			uart_log("Utilization Distance: %" PRIfloat " ", ACC_LOG_FLOAT_TO_INTEGER(result->distance));
			uart_log("Level (m): %" PRIfloat " ", ACC_LOG_FLOAT_TO_INTEGER(result->level_m));
			uart_log("Level (%%): %" PRIfloat " ", ACC_LOG_FLOAT_TO_INTEGER(result->level_percent));
			uart_log("\n");
		}

		if (result->presence_valid)
		{
			uart_log("Presence detected: %s ", result->presence_detected ? "True" : "False");

			if (result->presence_detected)
			{
				uart_log("Inter Presence Score: %" PRIfloat " ", ACC_LOG_FLOAT_TO_INTEGER(result->inter_presence_score));
				uart_log("Intra Presence Score: %" PRIfloat " ", ACC_LOG_FLOAT_TO_INTEGER(result->intra_presence_score));
			}
			uart_log("\n");
		}
	}
}

static void uart_log(const char *format, ...)
{
	char log_buffer[UART_LOG_BUFFER_SIZE];

	va_list ap;

	va_start(ap, format);

	if (g_application_state.uart_logs_enabed)
	{
		int ret = vsnprintf(log_buffer, UART_LOG_BUFFER_SIZE, format, ap);

		if (ret >= UART_LOG_BUFFER_SIZE)
		{
			log_buffer[UART_LOG_BUFFER_SIZE - 4] = '.';
			log_buffer[UART_LOG_BUFFER_SIZE - 3] = '.';
			log_buffer[UART_LOG_BUFFER_SIZE - 2] = '.';
			log_buffer[UART_LOG_BUFFER_SIZE - 1] = 0;
		}

		printf("%s", log_buffer);
	}

	va_end(ap);
}
