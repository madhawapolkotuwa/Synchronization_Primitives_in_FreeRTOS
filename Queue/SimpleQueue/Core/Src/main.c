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
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "string.h"
#include "stdio.h"
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

uint8_t rx_data = 0;

/* ******************* TASK HANDLERS ******************* */
xTaskHandle Task01_Handle;
xTaskHandle Task02_Handle;
xTaskHandle Task03_Handle;

/* ******************* QUEUE HANDLER ******************* */
xQueueHandle Queue_Handle;

/* ******************* TASK FUNCTIONS ******************* */
void Task01_Producer(void* argument)
{
	int t1Value = 100;
	uint32_t TickDelay = pdMS_TO_TICKS(4000); // convert ms to ticks
	while(1){

		char* str = (char*) pvPortMalloc(100 * sizeof(char)); // Allocate memory from the heap

		if(xQueueSend(Queue_Handle, &t1Value, portMAX_DELAY) == pdPASS){

			HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);

			sprintf(str, "T1 Successfully sent count to the queue: %d\n", t1Value);
			HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);

		}

		vPortFree(str); // Free the allocated memory

		vTaskDelay(TickDelay);
	}
}

void Task02_Producer(void* argument)
{
	int t2Value = 202;
	uint32_t TickDelay = pdMS_TO_TICKS(2000); // convert ms to ticks
	while(1){

		char* str = (char*) pvPortMalloc(100 * sizeof(char)); // Allocate memory from the heap

		if(xQueueSend(Queue_Handle, &t2Value, portMAX_DELAY) == pdPASS){

			HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);

			sprintf(str, "T2 Successfully sent count to the queue: %d\n", t2Value);
			HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);

		}

		vPortFree(str); // Free the allocated memory

		vTaskDelay(TickDelay);
	}
}

void Task03_Consumer(void* argument)
{
	int received = 0;
	uint32_t TickDelay = pdMS_TO_TICKS(5000); // convert ms to ticks
	while(1){

		char* str = (char*) pvPortMalloc(100 * sizeof(char)); // Allocate memory from the heap

		if(xQueueReceive(Queue_Handle, &received, portMAX_DELAY) != pdTRUE) // Wait indefinitely until something arrives
		{
			HAL_UART_Transmit(&huart1, (uint8_t*) "Error in receiving from Queue\n", 31, HAL_MAX_DELAY);
		}else {

			HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_11);

			sprintf(str, "Successfully received value from the queue: %d  \n\n", received);
			HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
		}

		vPortFree(str); // Free the allocated memory

		vTaskDelay(TickDelay);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{

	int ToSend = 123456789;
	if(rx_data == 'r')
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;

		if (xQueueSendToFrontFromISR(Queue_Handle, &ToSend, &xHigherPriorityTaskWoken) == pdPASS) // if queue is full, it will block.
		{
			HAL_UART_Transmit(huart, (uint8_t*) "\nSent from ISR\n\n", 17, HAL_MAX_DELAY);
		}else {
			HAL_UART_Transmit(huart, (uint8_t*) "\n\nCould not send from ISR Queue Full\n\n", 38, HAL_MAX_DELAY); // queue full
		}

		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	}

	// Restart UART Reception in Interrupt mode
	HAL_UART_Receive_IT(huart, &rx_data, 1);
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

  /* USER CODE END 2 */

  /* ********************* Create integer QUEUE ********************* */
  Queue_Handle = xQueueCreate(5, sizeof(int));
  if (Queue_Handle == NULL) {
	// Queue was not created and must not be used.
	HAL_UART_Transmit(&huart1, (uint8_t *)"Queue was not created and must not be used.\n", 43, HAL_MAX_DELAY);
  }else{
	HAL_UART_Transmit(&huart1, (uint8_t*) "Queue created successfully.\n", 30, HAL_MAX_DELAY);
  }

  /* ********************* Create Tasks ********************* */
  xTaskCreate(Task01_Producer, "T1", 128, NULL, 3, &Task01_Handle);
  xTaskCreate(Task02_Producer, "T2", 128, NULL, 2, &Task02_Handle);
  xTaskCreate(Task03_Consumer, "T3", 128, NULL, 1, &Task03_Handle);

  /* Start UART Reception in Interrupt mode */
  HAL_UART_Receive_IT(&huart1, &rx_data, 1);

  vTaskStartScheduler(); // This function will never return unless RTOS scheduler stops

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
