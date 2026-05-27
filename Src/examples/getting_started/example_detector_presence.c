// Copyright (c) Acconeer AB, 2022-2026
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "acc_definitions_a121.h"
#include "acc_definitions_common.h"
#include "acc_detector_presence.h"
#include "acc_hal_definitions_a121.h"
#include "acc_hal_integration_a121.h"
#include "acc_integration.h"
#include "acc_rss_a121.h"
#include "acc_sensor.h"

#include "acc_version.h"
#include <string.h>
//=================================================================================
#include "main.h"

//#define LED_PIN   MISC_GPIO1_Pin
//#define LED_PORT  MISC_GPIO1_GPIO_Port
//#define LED_ON_DURATION_MS 2000U
#define INTRA_THRESHOLD 1300  // 1.3 * 1000
// ========================================================================================
#define LED_PIN              MISC_GPIO1_Pin
#define LED_PORT             MISC_GPIO1_GPIO_Port
#define LED_ON_DURATION_MS   2000U
#define CMD_UART_HANDLE      MAIN_UART_HANDLE

extern UART_HandleTypeDef huart2;

static volatile int   g_intra_threshold = 1200;
static volatile float g_frame_rate      = 30.0f;
static volatile bool  g_reconfigure     = false;
static uint32_t       led_on_tick       = 0;
static bool           led_active        = false;

#define CMD_BUF_SIZE 64
static char    g_cmd_buf[CMD_BUF_SIZE];
static uint8_t g_cmd_pos   = 0;
static uint8_t g_uart_byte = 0;

static bool g_monitoring = true;  // true = вывод включён

static void uart_print(const char *msg)
{
    HAL_UART_Transmit(&CMD_UART_HANDLE, (const uint8_t *)msg, strlen(msg), 1000);
}


//=================================================================================




//#define INTER_THRESHOLD 1000  // inter < 1.0 -  не медленный фон. Если inter_score ниже этого значения, значит, движение быстрое
// Переменные для неблокирующего таймера LED
//static uint32_t led_on_tick  = 0;     // момент включения LED в мс
//static bool     led_active   = false; // горит ли LED сейчас
//=================================================================================

/** \example example_detector_presence.c
 * @brief This is an example on how the Detector Presence API can be used
 * @n
 * The example executes as follows:
 *   - Create a presence configuration
 *   - Create a sensor instance
 *   - Create a detector instance
 *   - Calibrate the sensor
 *   - Prepare the detector
 *   - Perform a sensor measurement and read out the data
 *   - Process the measurement and get detector result
 *   - Print the result
 *   - Destroy the configuration
 *   - Destroy the detector instance
 *   - Destroy the sensor instance
 */

typedef enum
{
	PRESENCE_PRESET_CONFIG_NONE = 0,
	PRESENCE_PRESET_CONFIG_SHORT_RANGE,
	PRESENCE_PRESET_CONFIG_MEDIUM_RANGE,
	PRESENCE_PRESET_CONFIG_LONG_RANGE,
	PRESENCE_PRESET_CONFIG_LOW_POWER_WAKEUP,
} presence_preset_config_t;

#define SENSOR_ID         (1U)
#define SENSOR_TIMEOUT_MS (2000U)

#define DEFAULT_PRESET_CONFIG PRESENCE_PRESET_CONFIG_MEDIUM_RANGE
//#define DEFAULT_PRESET_CONFIG PRESENCE_PRESET_CONFIG_LONG_RANGE

static bool do_sensor_calibration(acc_sensor_t *sensor, acc_cal_result_t *cal_result, void *buffer, uint32_t buffer_size);

static void led_update(void);

static void print_result(acc_detector_presence_result_t *result);

static void cleanup(acc_detector_presence_handle_t *presence_handle,
                    acc_detector_presence_config_t *presence_config,
                    acc_sensor_t                   *sensor,
                    void                           *buffer);

static void set_config(acc_detector_presence_config_t *presence_config, presence_preset_config_t preset);
static void uart_cmd_process(const char *line);

int acconeer_main(int argc, char *argv[]);

int acconeer_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	acc_detector_presence_config_t  *presence_config = NULL;
	acc_detector_presence_handle_t  *presence_handle = NULL;
	acc_detector_presence_metadata_t metadata;
	acc_sensor_t                    *sensor      = NULL;
	void                            *buffer      = NULL;
	uint32_t                         buffer_size = 0U;

	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_a121_t *hal = acc_hal_rss_integration_get_implementation();

	if (!acc_rss_hal_register(hal))
	{
		return EXIT_FAILURE;
	}
	//======================================================================
	uart_print("\r\n=== XM125 Drone Detector ===\r\n");
	uart_print("Type HELP for commands.\r\n\r\n");
	//======================================================================
	//HAL_UART_Receive_IT(&CMD_UART_HANDLE, &g_uart_byte, 1);
	//======================================================================

	presence_config = acc_detector_presence_config_create();
	if (presence_config == NULL)
	{
		printf("acc_detector_presence_config_create() failed\n");
		cleanup(presence_handle, presence_config, sensor, buffer);
		return EXIT_FAILURE;
	}

	set_config(presence_config, DEFAULT_PRESET_CONFIG);

	// Print the configuration
	acc_detector_presence_config_log(presence_config);

	presence_handle = acc_detector_presence_create(presence_config, &metadata);
	if (presence_handle == NULL)
	{
		printf("acc_detector_presence_create() failed\n");
		cleanup(presence_handle, presence_config, sensor, buffer);
		return EXIT_FAILURE;
	}

	if (!acc_detector_presence_get_buffer_size(presence_handle, &buffer_size))
	{
		printf("acc_detector_presence_get_buffer_size() failed\n");
		cleanup(presence_handle, presence_config, sensor, buffer);
		return EXIT_FAILURE;
	}

	buffer = acc_integration_mem_alloc(buffer_size);
	if (buffer == NULL)
	{
		printf("buffer allocation failed\n");
		cleanup(presence_handle, presence_config, sensor, buffer);
		return EXIT_FAILURE;
	}

	acc_hal_integration_sensor_supply_on(SENSOR_ID);
	acc_hal_integration_sensor_enable(SENSOR_ID);

	sensor = acc_sensor_create(SENSOR_ID);
	if (sensor == NULL)
	{
		printf("acc_sensor_create() failed\n");
		cleanup(presence_handle, presence_config, sensor, buffer);
		return EXIT_FAILURE;
	}

	acc_cal_result_t cal_result;

	if (!do_sensor_calibration(sensor, &cal_result, buffer, buffer_size))
	{
		printf("do_sensor_calibration() failed\n");
		cleanup(presence_handle, presence_config, sensor, buffer);
		return EXIT_FAILURE;
	}

	if (!acc_detector_presence_prepare(presence_handle, presence_config, sensor, &cal_result, buffer, buffer_size))
	{
		printf("acc_detector_presence_prepare() failed\n");
		cleanup(presence_handle, presence_config, sensor, buffer);
		return EXIT_FAILURE;
	}

	while (true)
	{
		acc_detector_presence_result_t result;

		if (!acc_sensor_measure(sensor))
		{
			printf("acc_sensor_measure failed\n");
			cleanup(presence_handle, presence_config, sensor, buffer);
			return EXIT_FAILURE;
		}

		if (!acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS))
		{
			printf("Sensor interrupt timeout\n");
			cleanup(presence_handle, presence_config, sensor, buffer);
			return EXIT_FAILURE;
		}

		if (!acc_sensor_read(sensor, buffer, buffer_size))
		{
			printf("acc_sensor_read failed\n");
			cleanup(presence_handle, presence_config, sensor, buffer);
			return EXIT_FAILURE;
		}

		if (!acc_detector_presence_process(presence_handle, buffer, &result))
		{
			printf("acc_detector_presence_process failed\n");
			cleanup(presence_handle, presence_config, sensor, buffer);
			return EXIT_FAILURE;
		}

		//==========================================================================================
		// Опрос UART на наличие символа
		if (HAL_UART_Receive(&CMD_UART_HANDLE, &g_uart_byte, 1, 1) == HAL_OK)
		{
		    uint8_t ch = g_uart_byte;
		    HAL_UART_Transmit(&CMD_UART_HANDLE, &ch, 1, 100);
		    if (ch == '\r' || ch == '\n')
		    {
		        if (g_cmd_pos > 0)
		        {
		            g_cmd_buf[g_cmd_pos] = '\0';
		            uart_cmd_process(g_cmd_buf);
		            g_cmd_pos = 0;
		        }
		    }
		    else if (ch == 0x08 || ch == 0x7F) { if (g_cmd_pos > 0) g_cmd_pos--; }
		    else if (g_cmd_pos < CMD_BUF_SIZE - 1) { g_cmd_buf[g_cmd_pos++] = (char)ch; }
		}
		//==========================================================================================
		// Вывод текущих значений каждые 0.2 секунды
		static uint32_t last_print = 0;
		if (g_monitoring && (HAL_GetTick() - last_print >= 50))
		{
		    last_print = HAL_GetTick();
		    char buf[80];
		    int intra = (int)(result.intra_presence_score * 1000.0f);
		    int inter = (int)(result.inter_presence_score * 1000.0f);
		    snprintf(buf, sizeof(buf), "[MON] intra=%d inter=%d\r\n", intra, inter);
		    uart_print(buf);
		}

		led_update();
		print_result(&result);

		if (result.processing_result.data_saturated)
		{
			printf("Data saturated. The detector result is not reliable.\n");
		}

		if (result.processing_result.frame_delayed)
		{
			printf("Frame delayed. Could not read data fast enough.\n");
			printf("Try lowering the frame rate or call 'acc_sensor_read' more frequently.\n");
		}

		/* If "calibration_needed" is indicated, the sensor needs to be recalibrated. */
		if (result.processing_result.calibration_needed)
		{
			printf("Sensor recalibration needed ... \n");

			if (!do_sensor_calibration(sensor, &cal_result, buffer, buffer_size))
			{
				printf("do_sensor_calibration() failed\n");
				cleanup(presence_handle, presence_config, sensor, buffer);
				return EXIT_FAILURE;
			}

			printf("Sensor recalibration done!\n");

			/* Before measuring again, the sensor needs to be prepared through the detector. */
			if (!acc_detector_presence_prepare(presence_handle, presence_config, sensor, &cal_result, buffer, buffer_size))
			{
				printf("acc_detector_presence_prepare() failed\n");
				cleanup(presence_handle, presence_config, sensor, buffer);
				return EXIT_FAILURE;
			}
		}
	}

	cleanup(presence_handle, presence_config, sensor, buffer);

	printf("Application finished OK\n");

	return EXIT_SUCCESS;
}

static void cleanup(acc_detector_presence_handle_t *presence_handle,
                    acc_detector_presence_config_t *presence_config,
                    acc_sensor_t                   *sensor,
                    void                           *buffer)
{
	acc_hal_integration_sensor_disable(SENSOR_ID);
	acc_hal_integration_sensor_supply_off(SENSOR_ID);

	if (presence_config != NULL)
	{
		acc_detector_presence_config_destroy(presence_config);
	}

	if (presence_handle != NULL)
	{
		acc_detector_presence_destroy(presence_handle);
	}

	if (sensor != NULL)
	{
		acc_sensor_destroy(sensor);
	}

	if (buffer != NULL)
	{
		acc_integration_mem_free(buffer);
	}
}

static bool do_sensor_calibration(acc_sensor_t *sensor, acc_cal_result_t *cal_result, void *buffer, uint32_t buffer_size)
{
	bool           status              = false;
	bool           cal_complete        = false;
	const uint16_t calibration_retries = 1U;

	// Random disturbances may cause the calibration to fail. At failure, retry at least once.
	for (uint16_t i = 0; !status && (i <= calibration_retries); i++)
	{
		// Reset sensor before calibration by disabling/enabling it
		acc_hal_integration_sensor_disable(SENSOR_ID);
		acc_hal_integration_sensor_enable(SENSOR_ID);

		do
		{
			status = acc_sensor_calibrate(sensor, &cal_complete, cal_result, buffer, buffer_size);

			if (status && !cal_complete)
			{
				status = acc_hal_integration_wait_for_sensor_interrupt(SENSOR_ID, SENSOR_TIMEOUT_MS);
			}
		} while (status && !cal_complete);
	}

	if (status)
	{
		/* Reset sensor after calibration by disabling/enabling it */
		acc_hal_integration_sensor_disable(SENSOR_ID);
		acc_hal_integration_sensor_enable(SENSOR_ID);
	}

	return status;
}


//static void led_update(void){
//	if (led_active)
//	    {
//	        if ((HAL_GetTick() - led_on_tick) >= LED_ON_DURATION_MS)
//	        {
//	            HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
//	            led_active = false;
//	        }
//	    }
//
//}

static void led_update(void)
{
    if (led_active)
    {
        if ((HAL_GetTick() - led_on_tick) >= LED_ON_DURATION_MS)
        {
            HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
            led_active = false;
        }
    }
}

static void uart_cmd_process(const char *line)
{
    char upper[CMD_BUF_SIZE];
    strncpy(upper, line, CMD_BUF_SIZE - 1);
    upper[CMD_BUF_SIZE - 1] = '\0';
    for (int i = 0; upper[i]; i++)
        if (upper[i] >= 'a' && upper[i] <= 'z') upper[i] -= 32;

    if (strncmp(upper, "HELP", 4) == 0)
    {
        uart_print("Commands:\r\n");
        uart_print("  SET INTRA <val>  threshold (100-10000)\r\n");
        uart_print("  SET RATE <val>   frame rate Hz (5-30)\r\n");
        uart_print("  GET              show parameters\r\n");
        uart_print("  STOP             pause monitoring\r\n");
        uart_print("  START            resume monitoring\r\n");
    }
    else if (strncmp(upper, "GET", 3) == 0)
    {
        char buf[80];
        snprintf(buf, sizeof(buf), "INTRA=%d RATE=%d Hz\r\n",
            g_intra_threshold, (int)g_frame_rate);
        uart_print(buf);
    }
    else if (strncmp(upper, "SET INTRA ", 10) == 0)
    {
        int val = atoi(line + 10);
        if (val >= 100 && val <= 10000)
        {
            g_intra_threshold = val;
            char buf[48];
            snprintf(buf, sizeof(buf), "[OK] INTRA = %d\r\n", val);
            uart_print(buf);
        }
        else uart_print("[ERR] Range: 100-10000\r\n");
    }
    else if (strncmp(upper, "SET RATE ", 9) == 0)
    {
        int val = atoi(line + 9);
        if (val >= 5 && val <= 30)
        {
            g_frame_rate  = (float)val;
            g_reconfigure = true;
            char buf[48];
            snprintf(buf, sizeof(buf), "[OK] RATE = %d Hz\r\n", val);
            uart_print(buf);
        }
        else uart_print("[ERR] Range: 5-30\r\n");
    }
    else if (strncmp(upper, "STOP", 4) == 0)
    {
        g_monitoring = false;
        uart_print("[OK] Monitoring stopped. Type START to resume.\r\n");
    }
    else if (strncmp(upper, "START", 5) == 0)
    {
        g_monitoring = true;
        uart_print("[OK] Monitoring started.\r\n");
    }
    else
    {
        uart_print("[ERR] Unknown. Type HELP.\r\n");
    }
}

//static void print_result(acc_detector_presence_result_t *result)
//{
//    int intra = (int)(result->intra_presence_score * 1000.0f);
//    int inter = (int)(result->inter_presence_score * 1000.0f);
//
//    if (result->presence_detected)
//    {
//        printf("Motion\n");
//    }
//    else
//    {
//        printf("No motion\n");
//    }
//
//    printf("Intra presence score: %d, Inter presence score: %d, Distance (mm): %d\n",
//           intra, inter,
//           (int)(result->presence_distance * 1000.0f));
//
//    // LED: включить на 2 секунды если быстрое движение
////    if (intra > INTRA_THRESHOLD)
////    {
////        HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
////        HAL_Delay(LED_ON_DURATION_MS);
////        HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
////    }
//
//    if (intra > INTRA_THRESHOLD)
//    //if (intra > INTRA_THRESHOLD  )
//    {
//        if (!led_active)  // включаем только если ещё не горит
//        {
//            HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
//            led_active  = true;
//            led_on_tick = HAL_GetTick();
//        }
//    }
//}


static void print_result(acc_detector_presence_result_t *result)
{
    int intra = (int)(result->intra_presence_score * 1000.0f);
    int inter = (int)(result->inter_presence_score * 1000.0f);

    if (intra > g_intra_threshold)
    {
        if (!led_active)
        {
            HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
            led_active  = true;
            led_on_tick = HAL_GetTick();

            char buf[64];
            snprintf(buf, sizeof(buf),
                "[DETECT] intra=%d inter=%d dist=%d mm\r\n",
                intra, inter,
                (int)(result->presence_distance * 1000.0f));
            uart_print(buf);
        }
    }
}


//static void print_result(acc_detector_presence_result_t *result)
//{
//	if (result->presence_detected)
//	{
//		printf("Motion\n");
//	}
//	else
//	{
//		printf("No motion\n");
//	}
//
//	// Score and distance are multiplied by 1000 to avoid printing floats
//	printf("Intra presence score: %d, Inter presence score: %d, Distance (mm): %d\n",
//	       (int)(result->intra_presence_score * 1000.0f),
//	       (int)(result->inter_presence_score * 1000.0f),
//	       (int)(result->presence_distance * 1000.0f));
//}

static void set_config(acc_detector_presence_config_t *presence_config, presence_preset_config_t preset)
{
	switch (preset)
	{
		case PRESENCE_PRESET_CONFIG_NONE:
			// Add configuration of the detector here
			break;

		case PRESENCE_PRESET_CONFIG_SHORT_RANGE:
			acc_detector_presence_config_start_set(presence_config, 0.06f);
			acc_detector_presence_config_end_set(presence_config, 1.0f);
			acc_detector_presence_config_automatic_subsweeps_set(presence_config, true);
			acc_detector_presence_config_signal_quality_set(presence_config, 30.0f);
			acc_detector_presence_config_inter_frame_idle_state_set(presence_config, ACC_CONFIG_IDLE_STATE_DEEP_SLEEP); // speep 1
			acc_detector_presence_config_sweeps_per_frame_set(presence_config, 16);            //  speed 2
			acc_detector_presence_config_frame_rate_set(presence_config, 10.0f);
			acc_detector_presence_config_frame_rate_app_driven_set(presence_config, false);
			acc_detector_presence_config_reset_filters_on_prepare_set(presence_config, true);
			acc_detector_presence_config_intra_detection_set(presence_config, true);
			acc_detector_presence_config_intra_detection_threshold_set(presence_config, 1.4f);
			acc_detector_presence_config_intra_frame_time_const_set(presence_config, 0.15f);
			acc_detector_presence_config_intra_output_time_const_set(presence_config, 0.3f);
			acc_detector_presence_config_inter_detection_set(presence_config, true);
			acc_detector_presence_config_inter_detection_threshold_set(presence_config, 1.0f);
			acc_detector_presence_config_inter_frame_deviation_time_const_set(presence_config, 0.5f);
			acc_detector_presence_config_inter_frame_fast_cutoff_set(presence_config, 5.0f);
			acc_detector_presence_config_inter_frame_slow_cutoff_set(presence_config, 0.20f);
			acc_detector_presence_config_inter_output_time_const_set(presence_config, 2.0f);
			acc_detector_presence_config_inter_frame_presence_timeout_set(presence_config, 3);

			break;

		case PRESENCE_PRESET_CONFIG_MEDIUM_RANGE:
			acc_detector_presence_config_start_set(presence_config, 0.3f);
			acc_detector_presence_config_end_set(presence_config, 2.5f);
			acc_detector_presence_config_automatic_subsweeps_set(presence_config, true);
			acc_detector_presence_config_signal_quality_set(presence_config, 20.0f);
			acc_detector_presence_config_inter_frame_idle_state_set(presence_config, ACC_CONFIG_IDLE_STATE_READY); //=== WAS ACC_CONFIG_IDLE_STATE_DEEP_SLEEP
			acc_detector_presence_config_sweeps_per_frame_set(presence_config, 12);  // was 16
			acc_detector_presence_config_frame_rate_set(presence_config, 20.0f); // was 20  =========================================
			acc_detector_presence_config_frame_rate_app_driven_set(presence_config, true);  // was false
			acc_detector_presence_config_reset_filters_on_prepare_set(presence_config, true);
			acc_detector_presence_config_intra_detection_set(presence_config, true);
			acc_detector_presence_config_intra_detection_threshold_set(presence_config, 1.3f);
			acc_detector_presence_config_intra_frame_time_const_set(presence_config, 0.15f);
			acc_detector_presence_config_intra_output_time_const_set(presence_config, 0.3f);
			acc_detector_presence_config_inter_detection_set(presence_config, true);
			acc_detector_presence_config_inter_detection_threshold_set(presence_config, 1.0f);
			acc_detector_presence_config_inter_frame_deviation_time_const_set(presence_config, 0.5f);
			acc_detector_presence_config_inter_frame_fast_cutoff_set(presence_config, 6.0f);
			acc_detector_presence_config_inter_frame_slow_cutoff_set(presence_config, 0.20f);
			acc_detector_presence_config_inter_output_time_const_set(presence_config, 2.0f);
			acc_detector_presence_config_inter_frame_presence_timeout_set(presence_config, 3);

			break;

		case PRESENCE_PRESET_CONFIG_LONG_RANGE:
			acc_detector_presence_config_start_set(presence_config, 1.0f);  // was 5.0
			acc_detector_presence_config_end_set(presence_config, 7.5f);
			acc_detector_presence_config_automatic_subsweeps_set(presence_config, true);
			acc_detector_presence_config_signal_quality_set(presence_config, 30.0f); // was 10.0
			acc_detector_presence_config_inter_frame_idle_state_set(presence_config, ACC_CONFIG_IDLE_STATE_READY); //=== WAS ACC_CONFIG_IDLE_STATE_DEEP_SLEEP
			acc_detector_presence_config_sweeps_per_frame_set(presence_config, 12); // was 16
			acc_detector_presence_config_frame_rate_set(presence_config, 12.0f);  // was 12.0
			acc_detector_presence_config_frame_rate_app_driven_set(presence_config, true);  //===================================== WAS FALSE
			acc_detector_presence_config_reset_filters_on_prepare_set(presence_config, true);
			acc_detector_presence_config_intra_detection_set(presence_config, true);
			acc_detector_presence_config_intra_detection_threshold_set(presence_config, 1.2f);
			acc_detector_presence_config_intra_frame_time_const_set(presence_config, 0.15f);
			acc_detector_presence_config_intra_output_time_const_set(presence_config, 0.3f);
			acc_detector_presence_config_inter_detection_set(presence_config, true);
			acc_detector_presence_config_inter_detection_threshold_set(presence_config, 0.8f);
			acc_detector_presence_config_inter_frame_deviation_time_const_set(presence_config, 0.5f);
			acc_detector_presence_config_inter_frame_fast_cutoff_set(presence_config, 6.0f);
			acc_detector_presence_config_inter_frame_slow_cutoff_set(presence_config, 0.20f);
			acc_detector_presence_config_inter_output_time_const_set(presence_config, 2.0f);
			acc_detector_presence_config_inter_frame_presence_timeout_set(presence_config, 3);

			break;

		case PRESENCE_PRESET_CONFIG_LOW_POWER_WAKEUP:
			acc_detector_presence_config_start_set(presence_config, 0.38f);
			acc_detector_presence_config_end_set(presence_config, 0.67f);
			acc_detector_presence_config_automatic_subsweeps_set(presence_config, false);
			acc_detector_presence_config_signal_quality_set(presence_config, 20.0f);
			acc_detector_presence_config_auto_step_length_set(presence_config, true);
			acc_detector_presence_config_auto_profile_set(presence_config, false);
			acc_detector_presence_config_profile_set(presence_config, ACC_CONFIG_PROFILE_5);
			acc_detector_presence_config_inter_frame_idle_state_set(presence_config, ACC_CONFIG_IDLE_STATE_DEEP_SLEEP);
			acc_detector_presence_config_hwaas_set(presence_config, 8);
			acc_detector_presence_config_sweeps_per_frame_set(presence_config, 8);
			acc_detector_presence_config_frame_rate_set(presence_config, 0.7f);
			acc_detector_presence_config_frame_rate_app_driven_set(presence_config, false);
			acc_detector_presence_config_reset_filters_on_prepare_set(presence_config, true);
			acc_detector_presence_config_intra_detection_set(presence_config, true);
			acc_detector_presence_config_intra_detection_threshold_set(presence_config, 1.7f);
			acc_detector_presence_config_intra_frame_time_const_set(presence_config, 0.3f);
			acc_detector_presence_config_intra_output_time_const_set(presence_config, 0.3f);
			acc_detector_presence_config_inter_detection_set(presence_config, true);
			acc_detector_presence_config_inter_detection_threshold_set(presence_config, 1.2f);
			acc_detector_presence_config_inter_frame_deviation_time_const_set(presence_config, 0.5f);
			acc_detector_presence_config_inter_frame_fast_cutoff_set(presence_config, 5.0f);
			acc_detector_presence_config_inter_frame_slow_cutoff_set(presence_config, 0.20f);
			acc_detector_presence_config_inter_output_time_const_set(presence_config, 0.5f);
			acc_detector_presence_config_inter_frame_presence_timeout_set(presence_config, 2);

			break;
	}
}
