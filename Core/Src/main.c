/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Hal/clock.h"
#include "Hal/gpio.h"
#include "Hal/timer.h"
#include "Hal/i2c.h"
#include "Hal/uart.h"
#include "Hal/spi.h"
#include "Hal/watchdog.h"
#include "Device/eeprom.h"
#include "Device/placedpoint.h"
#include "Device/rfid.h"
#include "Device/sound.h"
#include "Device/waterflow.h"
#include "Device/led.h"
#include "DeviceManager/vanmanager.h"
#include "App/protocol.h"
#include "App/commandhandler.h"
#include "App/schedulerport.h"
#include "App/statusreporter.h"
#include "App/statemachine.h"
#include "Lib/scheduler/scheduler.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  // Hal init
  CLOCK_init();
  GPIO_init();
  I2C_init();
  TIMER_init();
  UART_init();
  SPI_init();

  // Device Init
  EEPROM_init();
  PLACEDPOINT_init();
  RFID_init();
  SOUND_init();
  WATERFLOW_init();
  SOLENOID_init();
  LED_init();
//  // Device Manager Init
  VANMANAGER_init();
//
//  // App Init
  STATEMACHINE_init();
  PROTOCOL_init();
  SCHEDULERPORT_init();
  COMMANDHANDLER_init();
  SCHEDULERPORT_init();
  STATUSREPORTER_init();
  /* USER CODE END Init */
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//  SPI_test();
//  UART_test();
//  RTC_test();
//  EEPROM_test();
//  CONFIG_test();
//  JSMNG_test();
//  CONFIG_clear();
//  WATCHDOG_test();
//  TIMER_test();
//  RFID_test();
//	WATERFLOW_test();
//  SOUND_test();
  while (1)
  {
	// App
	STATEMACHINE_run();
	PROTOCOL_run();
	STATUSREPORTER_run();
	COMMANDHANDLER_run();
	SCH_Dispatch_Tasks();
	VANMANAGER_run();
	// Device
	RFID_run();
	SOUND_run();

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
