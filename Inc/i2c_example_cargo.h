// Copyright (c) Acconeer AB, 2025
// All rights reserved

#ifndef I2C_EXAMPLE_CARGO_H_
#define I2C_EXAMPLE_CARGO_H_

#include <stdbool.h>
#include <stdint.h>

#include "example_cargo.h"

/**
 * @brief Get pointer to internal cargo config struct
 *
 * @return cargo configuration handle
 */
cargo_config_t *i2c_example_cargo_get_config(void);

/**
 * @brief Send command to be executed to cargo i2c app
 *
 * @param[in] command The command to be executed
 * @return true if successful
 */
bool i2c_example_cargo_command(uint32_t command);

/**
 * @brief Get cargo example app status
 *
 * @return status from cargo example app
 */
uint32_t i2c_example_cargo_get_status(void);

/**
 * @brief Get the count of number of measurements performed
 *
 * The counter will be increased every time the Cargo example app completes a measurement
 *
 * @return measure counter from application
 */
uint32_t i2c_example_cargo_get_measure_counter(void);

/**
 * @brief Get the estimated presence update rate of the
 *
 * The application measures time between presence updates with ms accuraccy.
 *
 * @return The estimated update rate in mHz
 */
uint32_t i2c_example_cargo_get_actual_presence_update_rate(void);

/**
 * @brief Copy utilization example app cargo result
 *
 * @return result from example app cargo
 */
void i2c_example_cargo_copy_utilization_result_fields(cargo_result_t *dst_result);

/**
 * @brief Copy presence example app cargo result
 *
 * @return result from example app cargo
 */
void i2c_example_cargo_copy_presence_burst_result_fields(cargo_result_t *dst_result);

/**
 * @brief Get most recently measured temperature
 *
 * @return temperature as defined in @ref acc_processing_result_t
 */
int16_t i2c_example_cargo_get_temperature(void);

#endif
