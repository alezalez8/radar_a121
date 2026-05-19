// Copyright (c) Acconeer AB, 2026
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "acc_reg_protocol.h"
#include "example_cargo_reg_protocol.h"


/**
 * This struct defines the links between a register address and the register read and/or write function.
 */
const acc_reg_protocol_t example_cargo_reg_protocol[EXAMPLE_CARGO_REG_REGISTER_COUNT] = {
    {
        EXAMPLE_CARGO_REG_VERSION_ADDRESS,  // version address
        example_cargo_reg_read_version,     // version read access function
        NULL,
    },
    {
        EXAMPLE_CARGO_REG_PROTOCOL_STATUS_ADDRESS,  // protocol_status address
        example_cargo_reg_read_protocol_status,     // protocol_status read access function
        NULL,
    },
    {
        EXAMPLE_CARGO_REG_MEASURE_COUNTER_ADDRESS,  // measure_counter address
        example_cargo_reg_read_measure_counter,     // measure_counter read access function
        NULL,
    },
    {
        EXAMPLE_CARGO_REG_ACTUAL_PRESENCE_UPDATE_RATE_ADDRESS,  // actual_presence_update_rate address
        example_cargo_reg_read_actual_presence_update_rate,     // actual_presence_update_rate read access function
        NULL,
    },
    {
        EXAMPLE_CARGO_REG_APPLICATION_STATUS_ADDRESS,  // application_status address
        example_cargo_reg_read_application_status,     // application_status read access function
        NULL,
    },
    {
        EXAMPLE_CARGO_REG_CONTAINER_SIZE_ADDRESS,  // container_size address
        example_cargo_reg_read_container_size,     // container_size read access function
        example_cargo_reg_write_container_size,    // container_size write access function
    },
    {
        EXAMPLE_CARGO_REG_ACTIVATE_UTILIZATION_LEVEL_ADDRESS,  // activate_utilization_level address
        example_cargo_reg_read_activate_utilization_level,     // activate_utilization_level read access function
        example_cargo_reg_write_activate_utilization_level,    // activate_utilization_level write access function
    },
    {
        EXAMPLE_CARGO_REG_UTILIZATION_SIGNAL_QUALITY_ADDRESS,  // utilization_signal_quality address
        example_cargo_reg_read_utilization_signal_quality,     // utilization_signal_quality read access function
        example_cargo_reg_write_utilization_signal_quality,    // utilization_signal_quality write access function
    },
    {
        EXAMPLE_CARGO_REG_UTILIZATION_THRESHOLD_SENSITIVITY_ADDRESS,  // utilization_threshold_sensitivity address
        example_cargo_reg_read_utilization_threshold_sensitivity,     // utilization_threshold_sensitivity read access function
        example_cargo_reg_write_utilization_threshold_sensitivity,    // utilization_threshold_sensitivity write access function
    },
    {
        EXAMPLE_CARGO_REG_ACTIVATE_PRESENCE_ADDRESS,  // activate_presence address
        example_cargo_reg_read_activate_presence,     // activate_presence read access function
        example_cargo_reg_write_activate_presence,    // activate_presence write access function
    },
    {
        EXAMPLE_CARGO_REG_PRESENCE_UPDATE_RATE_ADDRESS,  // presence_update_rate address
        example_cargo_reg_read_presence_update_rate,     // presence_update_rate read access function
        example_cargo_reg_write_presence_update_rate,    // presence_update_rate write access function
    },
    {
        EXAMPLE_CARGO_REG_PRESENCE_SWEEPS_PER_FRAME_ADDRESS,  // presence_sweeps_per_frame address
        example_cargo_reg_read_presence_sweeps_per_frame,     // presence_sweeps_per_frame read access function
        example_cargo_reg_write_presence_sweeps_per_frame,    // presence_sweeps_per_frame write access function
    },
    {
        EXAMPLE_CARGO_REG_PRESENCE_SIGNAL_QUALITY_ADDRESS,  // presence_signal_quality address
        example_cargo_reg_read_presence_signal_quality,     // presence_signal_quality read access function
        example_cargo_reg_write_presence_signal_quality,    // presence_signal_quality write access function
    },
    {
        EXAMPLE_CARGO_REG_PRESENCE_INTER_DETECTION_THRESHOLD_ADDRESS,  // presence_inter_detection_threshold address
        example_cargo_reg_read_presence_inter_detection_threshold,     // presence_inter_detection_threshold read access function
        example_cargo_reg_write_presence_inter_detection_threshold,    // presence_inter_detection_threshold write access function
    },
    {
        EXAMPLE_CARGO_REG_PRESENCE_INTRA_DETECTION_THRESHOLD_ADDRESS,  // presence_intra_detection_threshold address
        example_cargo_reg_read_presence_intra_detection_threshold,     // presence_intra_detection_threshold read access function
        example_cargo_reg_write_presence_intra_detection_threshold,    // presence_intra_detection_threshold write access function
    },
    {
        EXAMPLE_CARGO_REG_RESULT_HEADER_ADDRESS,  // result_header address
        example_cargo_reg_read_result_header,     // result_header read access function
        NULL,
    },
    {
        EXAMPLE_CARGO_REG_UTILIZATION_DISTANCE_ADDRESS,  // utilization_distance address
        example_cargo_reg_read_utilization_distance,     // utilization_distance read access function
        NULL,
    },
    {
        EXAMPLE_CARGO_REG_UTILIZATION_LEVEL_MM_ADDRESS,  // utilization_level_mm address
        example_cargo_reg_read_utilization_level_mm,     // utilization_level_mm read access function
        NULL,
    },
    {
        EXAMPLE_CARGO_REG_UTILIZATION_LEVEL_PERCENT_ADDRESS,  // utilization_level_percent address
        example_cargo_reg_read_utilization_level_percent,     // utilization_level_percent read access function
        NULL,
    },
    {
        EXAMPLE_CARGO_REG_PRESENCE_DETECTED_ADDRESS,  // presence_detected address
        example_cargo_reg_read_presence_detected,     // presence_detected read access function
        NULL,
    },
    {
        EXAMPLE_CARGO_REG_MAX_INTER_PRESENCE_SCORE_ADDRESS,  // max_inter_presence_score address
        example_cargo_reg_read_max_inter_presence_score,     // max_inter_presence_score read access function
        NULL,
    },
    {
        EXAMPLE_CARGO_REG_MAX_INTRA_PRESENCE_SCORE_ADDRESS,  // max_intra_presence_score address
        example_cargo_reg_read_max_intra_presence_score,     // max_intra_presence_score read access function
        NULL,
    },
    {
        EXAMPLE_CARGO_REG_COMMAND_ADDRESS,  // command address
        NULL,
        example_cargo_reg_write_command,    // command write access function
    },
    {
        EXAMPLE_CARGO_REG_APPLICATION_ID_ADDRESS,  // application_id address
        example_cargo_reg_read_application_id,     // application_id read access function
        NULL,
    },
};


void example_cargo_reg_protocol_setup(void)
{
    acc_reg_protocol_setup(example_cargo_reg_protocol, EXAMPLE_CARGO_REG_REGISTER_COUNT);
}


void example_cargo_reg_protocol_write_default(void)
{
}
