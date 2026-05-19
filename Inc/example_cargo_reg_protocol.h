// Copyright (c) Acconeer AB, 2026
// All rights reserved

#ifndef EXAMPLE_CARGO_REG_PROTOCOL_H_
#define EXAMPLE_CARGO_REG_PROTOCOL_H_

#include <stdbool.h>
#include <stdint.h>

// The number of registers in the application
#define EXAMPLE_CARGO_REG_REGISTER_COUNT 24U

// The register address defines
#define EXAMPLE_CARGO_REG_VERSION_ADDRESS 0U
#define EXAMPLE_CARGO_REG_PROTOCOL_STATUS_ADDRESS 1U
#define EXAMPLE_CARGO_REG_MEASURE_COUNTER_ADDRESS 2U
#define EXAMPLE_CARGO_REG_ACTUAL_PRESENCE_UPDATE_RATE_ADDRESS 3U
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_ADDRESS 4U
#define EXAMPLE_CARGO_REG_CONTAINER_SIZE_ADDRESS 16U
#define EXAMPLE_CARGO_REG_ACTIVATE_UTILIZATION_LEVEL_ADDRESS 17U
#define EXAMPLE_CARGO_REG_UTILIZATION_SIGNAL_QUALITY_ADDRESS 18U
#define EXAMPLE_CARGO_REG_UTILIZATION_THRESHOLD_SENSITIVITY_ADDRESS 19U
#define EXAMPLE_CARGO_REG_ACTIVATE_PRESENCE_ADDRESS 20U
#define EXAMPLE_CARGO_REG_PRESENCE_UPDATE_RATE_ADDRESS 21U
#define EXAMPLE_CARGO_REG_PRESENCE_SWEEPS_PER_FRAME_ADDRESS 22U
#define EXAMPLE_CARGO_REG_PRESENCE_SIGNAL_QUALITY_ADDRESS 23U
#define EXAMPLE_CARGO_REG_PRESENCE_INTER_DETECTION_THRESHOLD_ADDRESS 24U
#define EXAMPLE_CARGO_REG_PRESENCE_INTRA_DETECTION_THRESHOLD_ADDRESS 25U
#define EXAMPLE_CARGO_REG_RESULT_HEADER_ADDRESS 32U
#define EXAMPLE_CARGO_REG_UTILIZATION_DISTANCE_ADDRESS 33U
#define EXAMPLE_CARGO_REG_UTILIZATION_LEVEL_MM_ADDRESS 34U
#define EXAMPLE_CARGO_REG_UTILIZATION_LEVEL_PERCENT_ADDRESS 35U
#define EXAMPLE_CARGO_REG_PRESENCE_DETECTED_ADDRESS 36U
#define EXAMPLE_CARGO_REG_MAX_INTER_PRESENCE_SCORE_ADDRESS 37U
#define EXAMPLE_CARGO_REG_MAX_INTRA_PRESENCE_SCORE_ADDRESS 38U
#define EXAMPLE_CARGO_REG_COMMAND_ADDRESS 48U
#define EXAMPLE_CARGO_REG_APPLICATION_ID_ADDRESS 65535U

// The defines for version fields (masks and bit-positions)
#define EXAMPLE_CARGO_REG_VERSION_FIELD_MAJOR_POS  16U
#define EXAMPLE_CARGO_REG_VERSION_FIELD_MAJOR_MASK 0xffff0000
#define EXAMPLE_CARGO_REG_VERSION_FIELD_MINOR_POS  8U
#define EXAMPLE_CARGO_REG_VERSION_FIELD_MINOR_MASK 0x0000ff00
#define EXAMPLE_CARGO_REG_VERSION_FIELD_PATCH_POS  0U
#define EXAMPLE_CARGO_REG_VERSION_FIELD_PATCH_MASK 0x000000ff

// The defines for protocol_status fields (masks and bit-positions)
#define EXAMPLE_CARGO_REG_PROTOCOL_STATUS_FIELD_PROTOCOL_STATE_ERROR_POS  0U
#define EXAMPLE_CARGO_REG_PROTOCOL_STATUS_FIELD_PROTOCOL_STATE_ERROR_MASK 0x00000001
#define EXAMPLE_CARGO_REG_PROTOCOL_STATUS_FIELD_PACKET_LENGTH_ERROR_POS  1U
#define EXAMPLE_CARGO_REG_PROTOCOL_STATUS_FIELD_PACKET_LENGTH_ERROR_MASK 0x00000002
#define EXAMPLE_CARGO_REG_PROTOCOL_STATUS_FIELD_ADDRESS_ERROR_POS  2U
#define EXAMPLE_CARGO_REG_PROTOCOL_STATUS_FIELD_ADDRESS_ERROR_MASK 0x00000004
#define EXAMPLE_CARGO_REG_PROTOCOL_STATUS_FIELD_WRITE_FAILED_POS  3U
#define EXAMPLE_CARGO_REG_PROTOCOL_STATUS_FIELD_WRITE_FAILED_MASK 0x00000008
#define EXAMPLE_CARGO_REG_PROTOCOL_STATUS_FIELD_WRITE_TO_READ_ONLY_POS  4U
#define EXAMPLE_CARGO_REG_PROTOCOL_STATUS_FIELD_WRITE_TO_READ_ONLY_MASK 0x00000010

// The defines for application_status fields (masks and bit-positions)
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_RSS_REGISTER_OK_POS  0U
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_RSS_REGISTER_OK_MASK 0x00000001
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_SENSOR_CREATE_OK_POS  1U
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_SENSOR_CREATE_OK_MASK 0x00000002
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_SENSOR_CALIBRATE_OK_POS  2U
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_SENSOR_CALIBRATE_OK_MASK 0x00000004
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CARGO_CREATE_OK_POS  3U
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CARGO_CREATE_OK_MASK 0x00000008
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CARGO_CALIBRATE_OK_POS  4U
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CARGO_CALIBRATE_OK_MASK 0x00000010
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_SENSOR_BUFFER_OK_POS  5U
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_SENSOR_BUFFER_OK_MASK 0x00000020
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CARGO_BUFFER_OK_POS  6U
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CARGO_BUFFER_OK_MASK 0x00000040
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CONFIG_APPLY_OK_POS  7U
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CONFIG_APPLY_OK_MASK 0x00000080
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_RSS_REGISTER_ERROR_POS  8U
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_RSS_REGISTER_ERROR_MASK 0x00000100
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_SENSOR_CREATE_ERROR_POS  10U
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_SENSOR_CREATE_ERROR_MASK 0x00000400
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_SENSOR_CALIBRATE_ERROR_POS  11U
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_SENSOR_CALIBRATE_ERROR_MASK 0x00000800
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CARGO_CREATE_ERROR_POS  12U
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CARGO_CREATE_ERROR_MASK 0x00001000
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CARGO_CALIBRATE_ERROR_POS  13U
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CARGO_CALIBRATE_ERROR_MASK 0x00002000
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_SENSOR_BUFFER_ERROR_POS  14U
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_SENSOR_BUFFER_ERROR_MASK 0x00004000
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CARGO_BUFFER_ERROR_POS  15U
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CARGO_BUFFER_ERROR_MASK 0x00008000
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CONFIG_APPLY_ERROR_POS  16U
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_CONFIG_APPLY_ERROR_MASK 0x00010000
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_APPLICATION_ERROR_POS  17U
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_APPLICATION_ERROR_MASK 0x00020000
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_BUSY_POS  18U
#define EXAMPLE_CARGO_REG_APPLICATION_STATUS_FIELD_BUSY_MASK 0x00040000

// The defines for result_header fields (masks and bit-positions)
#define EXAMPLE_CARGO_REG_RESULT_HEADER_FIELD_TEMPERATURE_POS  0U
#define EXAMPLE_CARGO_REG_RESULT_HEADER_FIELD_TEMPERATURE_MASK 0x0000ffff
#define EXAMPLE_CARGO_REG_RESULT_HEADER_FIELD_UTILIZATION_LEVEL_VALID_POS  17U
#define EXAMPLE_CARGO_REG_RESULT_HEADER_FIELD_UTILIZATION_LEVEL_VALID_MASK 0x00020000
#define EXAMPLE_CARGO_REG_RESULT_HEADER_FIELD_PRESENCE_VALID_POS  18U
#define EXAMPLE_CARGO_REG_RESULT_HEADER_FIELD_PRESENCE_VALID_MASK 0x00040000

// The defines for command enum values
#define EXAMPLE_CARGO_REG_COMMAND_ENUM_APPLY_CONFIGURATION 1U
#define EXAMPLE_CARGO_REG_COMMAND_ENUM_MEASURE_UTILIZATION_LEVEL 4U
#define EXAMPLE_CARGO_REG_COMMAND_ENUM_MEASURE_PRESENCE 5U
#define EXAMPLE_CARGO_REG_COMMAND_ENUM_ENABLE_UART_LOGS 32U
#define EXAMPLE_CARGO_REG_COMMAND_ENUM_DISABLE_UART_LOGS 33U
#define EXAMPLE_CARGO_REG_COMMAND_ENUM_LOG_CONFIGURATION 34U
#define EXAMPLE_CARGO_REG_COMMAND_ENUM_RESET_MODULE 0x52535421

// The defines for application_id enum values
#define EXAMPLE_CARGO_REG_APPLICATION_ID_ENUM_DISTANCE_DETECTOR 1U
#define EXAMPLE_CARGO_REG_APPLICATION_ID_ENUM_PRESENCE_DETECTOR 2U
#define EXAMPLE_CARGO_REG_APPLICATION_ID_ENUM_REF_APP_BREATHING 3U
#define EXAMPLE_CARGO_REG_APPLICATION_ID_ENUM_EXAMPLE_CARGO 4U
#define EXAMPLE_CARGO_REG_APPLICATION_ID_ENUM_EXAMPLE_DOOR_OPEN_DETECTION 5U

// The config default defines


/**
 * The example_cargo protocol setup function.
 */
void example_cargo_reg_protocol_setup(void);


/**
 * The example_cargo protocol write default register value function.
 */
void example_cargo_reg_protocol_write_default(void);


/**
 * Read:
 * Get the RSS version.
 */
void example_cargo_reg_read_version(uint32_t *value);


/**
 * Read:
 * Get protocol error flags.
 */
void example_cargo_reg_read_protocol_status(uint32_t *value);


/**
 * Read:
 * Get the measure counter, the number of measurements performed since
 * restart.
 */
void example_cargo_reg_read_measure_counter(uint32_t *value);


/**
 * Read:
 * Get the actual update rate (frame rate) of presence during a burst
 */
void example_cargo_reg_read_actual_presence_update_rate(uint32_t *value);


/**
 * Read:
 * Get example app status flags.
 */
void example_cargo_reg_read_application_status(uint32_t *value);


/**
 * Read:
 * Size of the container. Valid values to write are 10U, 20U and 40U.
 */
void example_cargo_reg_read_container_size(uint32_t *value);


/**
 * Write:
 * Size of the container. Valid values to write are 10U, 20U and 40U.
 */
bool example_cargo_reg_write_container_size(const uint32_t value);


/**
 * Read:
 * Whether to activate utilization level measurements. The command
 * MEASURE_UTILIZATION_LEVEL cannot succeed if this register if false.
 */
void example_cargo_reg_read_activate_utilization_level(uint32_t *value);


/**
 * Write:
 * Whether to activate utilization level measurements. The command
 * MEASURE_UTILIZATION_LEVEL cannot succeed if this register if false.
 */
bool example_cargo_reg_write_activate_utilization_level(const uint32_t value);


/**
 * Read:
 * Signal quality. This register is x1000 compared to the Cargo
 * Example Application. For more information, see documentation about
 * the Distance Detectors signal quality parameter.
 */
void example_cargo_reg_read_utilization_signal_quality(uint32_t *value);


/**
 * Write:
 * Signal quality. This register is x1000 compared to the Cargo
 * Example Application. For more information, see documentation about
 * the Distance Detectors signal quality parameter.
 */
bool example_cargo_reg_write_utilization_signal_quality(const uint32_t value);


/**
 * Read:
 * Threshold sensitivity. This register is x1000 compared to the Cargo
 * Example Application. For more information, see documentation about
 * the Distance Detectors threshold sensitivity parameter.
 */
void example_cargo_reg_read_utilization_threshold_sensitivity(uint32_t *value);


/**
 * Write:
 * Threshold sensitivity. This register is x1000 compared to the Cargo
 * Example Application. For more information, see documentation about
 * the Distance Detectors threshold sensitivity parameter.
 */
bool example_cargo_reg_write_utilization_threshold_sensitivity(const uint32_t value);


/**
 * Read:
 * Whether to activate presence measurements. The command
 * MEASURE_PRESENCE cannot succeed if this register if false.
 */
void example_cargo_reg_read_activate_presence(uint32_t *value);


/**
 * Write:
 * Whether to activate presence measurements. The command
 * MEASURE_PRESENCE cannot succeed if this register if false.
 */
bool example_cargo_reg_write_activate_presence(const uint32_t value);


/**
 * Read:
 * The presence detector update rate (frame rate). This register is
 * x1000 compared to the Cargo Example Application. For more
 * information, see documentation about the Presence Detectors frame
 * rate parameter.
 */
void example_cargo_reg_read_presence_update_rate(uint32_t *value);


/**
 * Write:
 * The presence detector update rate (frame rate). This register is
 * x1000 compared to the Cargo Example Application. For more
 * information, see documentation about the Presence Detectors frame
 * rate parameter.
 */
bool example_cargo_reg_write_presence_update_rate(const uint32_t value);


/**
 * Read:
 * The number of sweeps that will be captured in each frame
 * (measurement). For more information, see documentation about the
 * Presence Detectors sweeps_per_frame parameter.
 */
void example_cargo_reg_read_presence_sweeps_per_frame(uint32_t *value);


/**
 * Write:
 * The number of sweeps that will be captured in each frame
 * (measurement). For more information, see documentation about the
 * Presence Detectors sweeps_per_frame parameter.
 */
bool example_cargo_reg_write_presence_sweeps_per_frame(const uint32_t value);


/**
 * Read:
 * Signal quality. This register is x1000 compared to the Cargo
 * Example Application. For more information, see documentation about
 * the Presence Detectors signal quality parameter.
 */
void example_cargo_reg_read_presence_signal_quality(uint32_t *value);


/**
 * Write:
 * Signal quality. This register is x1000 compared to the Cargo
 * Example Application. For more information, see documentation about
 * the Presence Detectors signal quality parameter.
 */
bool example_cargo_reg_write_presence_signal_quality(const uint32_t value);


/**
 * Read:
 * This is the threshold for detecting slower movements between
 * frames. This register is x1000 compared to the Cargo Example
 * Application. For more information, see documentation about the
 * Presence Detectors inter detection threshold parameter.
 */
void example_cargo_reg_read_presence_inter_detection_threshold(uint32_t *value);


/**
 * Write:
 * This is the threshold for detecting slower movements between
 * frames. This register is x1000 compared to the Cargo Example
 * Application. For more information, see documentation about the
 * Presence Detectors inter detection threshold parameter.
 */
bool example_cargo_reg_write_presence_inter_detection_threshold(const uint32_t value);


/**
 * Read:
 * This is the threshold for detecting faster movements between
 * frames. This register is x1000 compared to the Cargo Example
 * Application. For more information, see documentation about the
 * Presence Detectors intra detection threshold parameter.
 */
void example_cargo_reg_read_presence_intra_detection_threshold(uint32_t *value);


/**
 * Write:
 * This is the threshold for detecting faster movements between
 * frames. This register is x1000 compared to the Cargo Example
 * Application. For more information, see documentation about the
 * Presence Detectors intra detection threshold parameter.
 */
bool example_cargo_reg_write_presence_intra_detection_threshold(const uint32_t value);


/**
 * Read:
 * The result header for the cargo result.
 */
void example_cargo_reg_read_result_header(uint32_t *value);


/**
 * Read:
 * The distance, in millimeters, to the detection.
 */
void example_cargo_reg_read_utilization_distance(uint32_t *value);


/**
 * Read:
 * The fill level in millimeters. Fill level is the distance from the
 * detection to the back of the container.
 */
void example_cargo_reg_read_utilization_level_mm(uint32_t *value);


/**
 * Read:
 * The fill level in percent. Fill level is the distance from the
 * detection to the back of the container.
 */
void example_cargo_reg_read_utilization_level_percent(uint32_t *value);


/**
 * Read:
 * Whether presence was detected during the 5s presence burst
 */
void example_cargo_reg_read_presence_detected(uint32_t *value);


/**
 * Read:
 * Inter presence score is a measure of the amount of slow motion
 * detected. This register contains the maximum inter presence score
 * during the 5s presence burst.
 */
void example_cargo_reg_read_max_inter_presence_score(uint32_t *value);


/**
 * Read:
 * Intra presence score is measure of the amount of slow motion
 * detected. This register contains the maximum intra presence score
 * during the 5s presence burst.
 */
void example_cargo_reg_read_max_intra_presence_score(uint32_t *value);


/**
 * Write:
 * Execute command.
 */
bool example_cargo_reg_write_command(const uint32_t value);


/**
 * Read:
 * The application id register.
 */
void example_cargo_reg_read_application_id(uint32_t *value);


#endif
