/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

#include "FreeRTOS.h"
#include "task.h"
//#include "timers.h"
//#include "queue.h"
//#include "semphr.h"
#include "event_groups.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* ************************ Event Group Handle ***************************** */
EventGroupHandle_t EventGroup_Handle;

/* ************************ Task Handlers ***************************** */
TaskHandle_t Task01_Handler = NULL;
TaskHandle_t Task02_Handler = NULL;
TaskHandle_t Task03_Handler = NULL;

/* ************************ Event Bits ***************************** */
const EventBits_t task01_bit = (1 << 0); // 0x01 // Bit 0
const EventBits_t task02_bit = (1 << 1); // 0x02 // Bit 1
const EventBits_t task03_bit = (1 << 2); // 0x04 // Bit 2

const EventBits_t all_sync_bits = (task01_bit | task02_bit | task03_bit); // 0x07 // Bit 0, Bit 1, Bit 2)

/* ************************ Task Functions ************************* */
void Task01(void* pvParameters)
{
	for(;;)
	{
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);

		EventBits_t uxReturn = xEventGroupSync(
				EventGroup_Handle,  // The event group
				task01_bit, 		// The bits being set.
				all_sync_bits,		// Wait for all bits
				pdMS_TO_TICKS(2000) // Wait for 2000 ms
		);

		if((uxReturn & all_sync_bits) == all_sync_bits){
			// All three tasks reached the synchronisation point before the call
			// to xEventGroupSync() timed out.
			HAL_UART_Transmit(&huart1, (uint8_t *)"Task01: All tasks synchronized.\n", 32, HAL_MAX_DELAY);
		}else{
			// Timeout occurred before all bits were set.
			HAL_UART_Transmit(&huart1, (uint8_t*) "Task01: Synchronization timeout.\n", 33, HAL_MAX_DELAY);
		}

		vTaskDelay(pdMS_TO_TICKS(500));

	}
}

void Task02(void* pvParameters)
{
	for(;;)
	{
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);

		EventBits_t uxReturn = xEventGroupSync(
				EventGroup_Handle,  // The event group
				task02_bit, 		// The bits being set.
				all_sync_bits,		// Wait for all bits
				pdMS_TO_TICKS(2000) // Wait for 2000 ms
		);


		if((uxReturn & all_sync_bits) == all_sync_bits){
			// All three tasks reached the synchronisation point before the call
			// to xEventGroupSync() timed out.
			HAL_UART_Transmit(&huart1, (uint8_t *)"Task02: All tasks synchronized.\n", 32, HAL_MAX_DELAY);
		}else{
			// Timeout occurred before all bits were set.
			HAL_UART_Transmit(&huart1, (uint8_t*) "Task02: Synchronization timeout.\n", 33, HAL_MAX_DELAY);
		}

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void Task03(void* pvParameters)
{
	static int count = 0;
	for(;;)
	{
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_11);

		 if (count >= 3 && count <= 6) {
			 // Simulate a failure (not calling sync)
			 HAL_UART_Transmit(&huart1, (uint8_t *)"Task03: Missed sync (failure simulation).\n", 43, HAL_MAX_DELAY);
		 }
		 else {

			EventBits_t uxReturn = xEventGroupSync(
					EventGroup_Handle,  // The event group
					task03_bit, 		// The bits being set.
					all_sync_bits,		// Wait for all bits
					pdMS_TO_TICKS(2000) // Wait for 2000 ms
			);


			if((uxReturn & all_sync_bits) == all_sync_bits){
				// All three tasks reached the synchronisation point before the call
				// to xEventGroupSync() timed out.
				HAL_UART_Transmit(&huart1, (uint8_t *)"Task03: All tasks synchronized.\n", 32, HAL_MAX_DELAY);
			}else{
				// Timeout occurred before all bits were set.
				HAL_UART_Transmit(&huart1, (uint8_t*) "Task03: Synchronization timeout.\n", 33, HAL_MAX_DELAY);
			}
		 }

		 count++;
		 if (count > 8) count = 0;

		 vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */


  // Create Event Group
  EventGroup_Handle = xEventGroupCreate();
	if (EventGroup_Handle == NULL) {
		// Event Group creation failed
		HAL_UART_Transmit(&huart1, (uint8_t*) "Event Group Creation Failed!\n", 28, HAL_MAX_DELAY);
	}else {
		HAL_UART_Transmit(&huart1, (uint8_t*) "Event Group Created Successfully.\n", 35, HAL_MAX_DELAY);
	}

	// Create Tasks
	xTaskCreate(Task01, "Task01", 128, NULL, 1, &Task01_Handler);
	xTaskCreate(Task02, "Task02", 128, NULL, 1, &Task02_Handler);
	xTaskCreate(Task03, "Task03", 128, NULL, 1, &Task03_Handler);

	// Start Scheduler
	vTaskStartScheduler();


  /* USER CODE END 2 */


  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pins : PG11 PG13 PG14 */
  GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
#ifdef USE_FULL_ASSERT
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
