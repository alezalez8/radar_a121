// Copyright (c) Acconeer AB, 2023-2025
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stdint.h>

#include "acc_integration.h"
#include "acc_version.h"
#include "example_cargo.h"

#include "acc_reg_protocol.h"
#include "example_cargo_reg_protocol.h"
#include "i2c_example_cargo.h"

/**
 * @brief Get the protocol error status flags
 *
 * @return error flags as uint32 register value
 */
static uint32_t get_protocol_error_flags(void);

//
// REGISTER READ/WRITE ACCESS FUNCTIONS
//

void example_cargo_reg_read_application_id(uint32_t *value)
{
	*value = EXAMPLE_CARGO_REG_APPLICATION_ID_ENUM_EXAMPLE_CARGO;
}

void example_cargo_reg_read_version(uint32_t *value)
{
	*value = acc_version_get_hex();
}

void example_cargo_reg_read_protocol_status(uint32_t *value)
{
	uint32_t error_flags     = get_protocol_error_flags();
	uint32_t protocol_status = 0;

	if ((error_flags & ACC_REG_ERROR_FLAG_PROTOCOL_STATE_ERROR) != 0)
	{
		protocol_status |= EXAMPLE_CARGO_REG_PROTOCOL_STATUS_FIELD_PROTOCOL_STATE_ERROR_MASK;
	}

	if ((error_flags & ACC_REG_ERROR_FLAG_PACKET_LENGTH_ERROR) != 0)
	{
		protocol_status |= EXAMPLE_CARGO_REG_PROTOCOL_STATUS_FIELD_PACKET_LENGTH_ERROR_MASK;
	}

	if ((error_flags & ACC_REG_ERROR_FLAG_ADDRESS_ERROR) != 0)
	{
		protocol_status |= EXAMPLE_CARGO_REG_PROTOCOL_STATUS_FIELD_ADDRESS_ERROR_MASK;
	}

	if ((error_flags & ACC_REG_ERROR_FLAG_WRITE_FAILED) != 0)
	{
		protocol_status |= EXAMPLE_CARGO_REG_PROTOCOL_STATUS_FIELD_WRITE_FAILED_MASK;
	}

	if ((error_flags & ACC_REG_ERROR_FLAG_WRITE_TO_READ_ONLY) != 0)
	{
		protocol_status |= EXAMPLE_CARGO_REG_PROTOCOL_STATUS_FIELD_WRITE_TO_READ_ONLY_MASK;
	}

	*value = protocol_status;
}

void example_cargo_reg_read_measure_counter(uint32_t *value)
{
	*value = i2c_example_cargo_get_measure_counter();
}

void example_cargo_reg_read_actual_presence_update_rate(uint32_t *value)
{
	*value = i2c_example_cargo_get_actual_presence_update_rate();
}

bool example_cargo_reg_write_command(const uint32_t value)
{
	return i2c_example_cargo_command(value);
}

void example_cargo_reg_read_application_status(uint32_t *value)
{
	*value = i2c_example_cargo_get_status();
}

void example_cargo_reg_read_container_size(uint32_t *value)
{
	cargo_config_t *config = i2c_example_cargo_get_config();

	switch (config->container_size)
	{
		case CARGO_CONTAINER_SIZE_10_FT:
			*value = 10U;
			break;
		case CARGO_CONTAINER_SIZE_20_FT:
			*value = 20U;
			break;
		case CARGO_CONTAINER_SIZE_40_FT:
			*value = 40U;
			break;
		default:
			*value = 0U;
			break;
	}
}

bool example_cargo_reg_write_container_size(uint32_t value)
{
	bool status = true;

	cargo_config_t *config = i2c_example_cargo_get_config();

	switch (value)
	{
		case 10U:
			config->container_size = CARGO_CONTAINER_SIZE_10_FT;
			break;
		case 20U:
			config->container_size = CARGO_CONTAINER_SIZE_20_FT;
			break;
		case 40U:
			config->container_size = CARGO_CONTAINER_SIZE_40_FT;
			break;
		default:
			status = false;
			break;
	}
	return status;
}

void example_cargo_reg_read_activate_utilization_level(uint32_t *value)
{
	cargo_config_t *config = i2c_example_cargo_get_config();

	*value = (uint32_t)config->activate_utilization_level;
}

bool example_cargo_reg_write_activate_utilization_level(uint32_t value)
{
	cargo_config_t *config = i2c_example_cargo_get_config();

	config->activate_utilization_level = (bool)value;

	return true;
}

void example_cargo_reg_read_utilization_signal_quality(uint32_t *value)
{
	cargo_config_t *config = i2c_example_cargo_get_config();

	*value = acc_reg_protocol_float_to_uint32_milli(config->utilization.signal_quality);
}

bool example_cargo_reg_write_utilization_signal_quality(uint32_t value)
{
	cargo_config_t *config = i2c_example_cargo_get_config();

	config->utilization.signal_quality = acc_reg_protocol_uint32_milli_to_float(value);

	return true;
}

void example_cargo_reg_read_utilization_threshold_sensitivity(uint32_t *value)
{
	cargo_config_t *config = i2c_example_cargo_get_config();

	*value = acc_reg_protocol_float_to_uint32_milli(config->utilization.threshold_sensitivity);
}

bool example_cargo_reg_write_utilization_threshold_sensitivity(uint32_t value)
{
	cargo_config_t *config = i2c_example_cargo_get_config();

	config->utilization.threshold_sensitivity = acc_reg_protocol_uint32_milli_to_float(value);

	return true;
}

void example_cargo_reg_read_activate_presence(uint32_t *value)
{
	cargo_config_t *config = i2c_example_cargo_get_config();

	*value = (bool)config->activate_presence;
}

bool example_cargo_reg_write_activate_presence(uint32_t value)
{
	cargo_config_t *config = i2c_example_cargo_get_config();

	config->activate_presence = (bool)value;

	return true;
}

void example_cargo_reg_read_presence_update_rate(uint32_t *value)
{
	cargo_config_t *config = i2c_example_cargo_get_config();

	*value = acc_reg_protocol_float_to_uint32_milli(config->presence.update_rate);
}

bool example_cargo_reg_write_presence_update_rate(uint32_t value)
{
	cargo_config_t *config = i2c_example_cargo_get_config();

	config->presence.update_rate = acc_reg_protocol_uint32_milli_to_float(value);

	return true;
}

void example_cargo_reg_read_presence_sweeps_per_frame(uint32_t *value)
{
	cargo_config_t *config = i2c_example_cargo_get_config();

	*value = (uint32_t)config->presence.sweeps_per_frame;
}

bool example_cargo_reg_write_presence_sweeps_per_frame(uint32_t value)
{
	cargo_config_t *config = i2c_example_cargo_get_config();

	config->presence.sweeps_per_frame = (uint16_t)value;

	return true;
}

void example_cargo_reg_read_presence_signal_quality(uint32_t *value)
{
	cargo_config_t *config = i2c_example_cargo_get_config();

	*value = acc_reg_protocol_float_to_uint32_milli(config->presence.signal_quality);
}

bool example_cargo_reg_write_presence_signal_quality(uint32_t value)
{
	cargo_config_t *config = i2c_example_cargo_get_config();

	config->presence.signal_quality = acc_reg_protocol_uint32_milli_to_float(value);

	return true;
}

void example_cargo_reg_read_presence_inter_detection_threshold(uint32_t *value)
{
	cargo_config_t *config = i2c_example_cargo_get_config();

	*value = acc_reg_protocol_float_to_uint32_milli(config->presence.inter_detection_threshold);
}

bool example_cargo_reg_write_presence_inter_detection_threshold(uint32_t value)
{
	cargo_config_t *config = i2c_example_cargo_get_config();

	config->presence.inter_detection_threshold = acc_reg_protocol_uint32_milli_to_float(value);

	return true;
}

void example_cargo_reg_read_presence_intra_detection_threshold(uint32_t *value)
{
	cargo_config_t *config = i2c_example_cargo_get_config();

	*value = acc_reg_protocol_float_to_uint32_milli(config->presence.intra_detection_threshold);
}

bool example_cargo_reg_write_presence_intra_detection_threshold(uint32_t value)
{
	cargo_config_t *config = i2c_example_cargo_get_config();

	config->presence.intra_detection_threshold = acc_reg_protocol_uint32_milli_to_float(value);

	return true;
}

void example_cargo_reg_read_result_header(uint32_t *value)
{
	cargo_result_t result      = {0};
	int16_t        temperature = i2c_example_cargo_get_temperature();

	uint32_t temperature_bits =
	    (((uint32_t)temperature) << EXAMPLE_CARGO_REG_RESULT_HEADER_FIELD_TEMPERATURE_POS) & EXAMPLE_CARGO_REG_RESULT_HEADER_FIELD_TEMPERATURE_MASK;

	i2c_example_cargo_copy_utilization_result_fields(&result);

	uint32_t utilization_valid_bit = (((uint32_t)result.utilization_valid) << EXAMPLE_CARGO_REG_RESULT_HEADER_FIELD_UTILIZATION_LEVEL_VALID_POS) &
	                                 EXAMPLE_CARGO_REG_RESULT_HEADER_FIELD_UTILIZATION_LEVEL_VALID_MASK;

	i2c_example_cargo_copy_presence_burst_result_fields(&result);

	uint32_t presence_valid_bit = (((uint32_t)result.presence_valid) << EXAMPLE_CARGO_REG_RESULT_HEADER_FIELD_PRESENCE_VALID_POS) &
	                              EXAMPLE_CARGO_REG_RESULT_HEADER_FIELD_PRESENCE_VALID_MASK;

	uint32_t packed = temperature_bits | utilization_valid_bit | presence_valid_bit;
	*value          = packed;
}

void example_cargo_reg_read_utilization_distance(uint32_t *value)
{
	cargo_result_t result = {0};

	i2c_example_cargo_copy_utilization_result_fields(&result);

	*value = acc_reg_protocol_float_to_uint32_milli(result.distance);
}

void example_cargo_reg_read_utilization_level_mm(uint32_t *value)
{
	cargo_result_t result = {0};

	i2c_example_cargo_copy_utilization_result_fields(&result);

	*value = acc_reg_protocol_float_to_uint32_milli(result.level_m);
}

void example_cargo_reg_read_utilization_level_percent(uint32_t *value)
{
	cargo_result_t result = {0};

	i2c_example_cargo_copy_utilization_result_fields(&result);

	*value = (uint32_t)result.level_percent;
}

void example_cargo_reg_read_presence_detected(uint32_t *value)
{
	cargo_result_t result = {0};

	i2c_example_cargo_copy_presence_burst_result_fields(&result);

	*value = (uint32_t)result.presence_detected;
}

void example_cargo_reg_read_max_inter_presence_score(uint32_t *value)
{
	cargo_result_t result = {0};

	i2c_example_cargo_copy_presence_burst_result_fields(&result);

	*value = acc_reg_protocol_float_to_uint32_milli(result.inter_presence_score);
}

void example_cargo_reg_read_max_intra_presence_score(uint32_t *value)
{
	cargo_result_t result = {0};

	i2c_example_cargo_copy_presence_burst_result_fields(&result);

	*value = acc_reg_protocol_float_to_uint32_milli(result.intra_presence_score);
}

//
// PRIVATE HELPER FUNCTIONS
//

static uint32_t get_protocol_error_flags(void)
{
	/* Make sure we do not have a race for  error flags */
	acc_integration_critical_section_enter();

	uint32_t error_flags = acc_reg_protocol_get_error_flags();

	acc_integration_critical_section_exit();

	return error_flags;
}
