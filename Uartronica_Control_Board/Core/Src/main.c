/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define RX_BUFFER_SIZE 10

uint8_t rxBuffer[RX_BUFFER_SIZE]; // Buffer to accumulate received message
uint8_t rxChar;                  // Buffer for a single received character
// uint8_t txMessage[] = "ok\n";    // Response message
// uint8_t index = 0;               // Index to track the buffer position

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
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
  HAL_Delay(1000);
  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  /* USER CODE BEGIN 3 */
	  while (1)
	  {
	      // Receive a command from the UART (sent by the Python program)
	      if (HAL_UART_Receive(&huart3, &rxChar, 1, HAL_MAX_DELAY) == HAL_OK)
	      {
	          // Interpret the received command
	          switch (rxChar)
	          {
	          case 'R': // Read GPIO states and send detailed output
	          {
	              // Read GPIO input statesif ((rxBuffer[0] == 'd') && (rxBuffer[1] == '\n'))
	        	    {
	        	        // Handle the 'd\n' command or input here
	        	    }
	              uint8_t sensor1 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0);
	              uint8_t sensor2 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1);


			  // Check for floating pins
				  char warning[50];
				  if (sensor1 == GPIO_PIN_RESET && sensor2 == GPIO_PIN_RESET)
				  {
					  strcpy(warning, "Pins might be floating!\n");
				  }
				  else if (sensor1 != GPIO_PIN_RESET && sensor2 != GPIO_PIN_RESET)
				  {
					  strcpy(warning, "  ");
				  }

	              // Determine the logic state
	              char state[3];
	              if (sensor1 == GPIO_PIN_RESET && sensor2 == GPIO_PIN_RESET)
	              {
	                  strcpy(state, "00");
	              }
	              else if (sensor1 == GPIO_PIN_RESET && sensor2 == GPIO_PIN_SET)
	              {
	                  strcpy(state, "01");
	              }
	              else if (sensor1 == GPIO_PIN_SET && sensor2 == GPIO_PIN_RESET)
	              {
	                  strcpy(state, "10");
	              }
	              else if (sensor1 == GPIO_PIN_SET && sensor2 == GPIO_PIN_SET)
	              {
	                  strcpy(state, "11");
	              }

	              // Determine the voltage levels for detailed output
	              char sensor1_voltage[4];
	              char sensor2_voltage[4];

	              strcpy(sensor1_voltage, (sensor1 == GPIO_PIN_SET) ? "5V" : "0V");
	              strcpy(sensor2_voltage, (sensor2 == GPIO_PIN_SET) ? "5V" : "0V");

	              // Construct the detailed message
	              char response[100];
	              snprintf(response, sizeof(response),
	                       "State - %s\nSensor 1 - %s\nSensor 2 - %s\nWarning - %s\n",
	                       state, sensor1_voltage, sensor2_voltage, warning);

	              // Send the message via UART
	              HAL_UART_Transmit(&huart3, (uint8_t *)response, strlen(response), HAL_MAX_DELAY);

	              break;
	          }


	          case 'C': // Control GPIO outputs
	          {
	              // Receive additional control data
	              uint8_t controlData[2];
	              HAL_UART_Receive(&huart3, controlData, 2, HAL_MAX_DELAY);

	              // Set GPIO outputs based on received control data
	              HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, (controlData[0] == '1') ? GPIO_PIN_SET : GPIO_PIN_RESET);
	              HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, (controlData[1] == '1') ? GPIO_PIN_SET : GPIO_PIN_RESET);

	              // Acknowledge the command
	              char ack[] = "OK\n";
	              HAL_UART_Transmit(&huart3, (uint8_t *)ack, strlen(ack), HAL_MAX_DELAY);
	              break;
	          }

	          case 'T': // Test GPIO connections
	                      test_connection();
	                      char ack[] = "Test Completed\n";
	                      HAL_UART_Transmit(&huart3, (uint8_t *)ack, strlen(ack), HAL_MAX_DELAY);
	                      break;

	          default: // Invalid command
	          {
	              char error[] = "ERR\n";
	              HAL_UART_Transmit(&huart3, (uint8_t *)error, strlen(error), HAL_MAX_DELAY);
	              break;
	          }
	          }
	      }
	  }
	  /* USER CODE END 3 */

  }
  /* USER CODE END 3 */
}

void test_connection(void)
{
    char message[100];

    // Set PC8 to HIGH and PC9 to LOW
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);    // Set PC8 HIGH
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);  // Set PC9 LOW

    HAL_Delay(10);  // Delay to allow signals to stabilize

    // Read PC0 and PC1
    GPIO_PinState pc0_state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0);
    GPIO_PinState pc1_state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1);

    // Verify the connection
    if (pc0_state == GPIO_PIN_SET && pc1_state == GPIO_PIN_RESET)
    {
        snprintf(message, sizeof(message), "Connection OK: PC0 is HIGH (PC8), PC1 is LOW (PC9)\n");
    }
    else
    {
        snprintf(message, sizeof(message), "Connection Failed: PC0 or PC1 state mismatch (Test 1)\n");
    }

    HAL_UART_Transmit(&huart3, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);

    // Reverse states for further testing
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);  // Set PC8 LOW
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);    // Set PC9 HIGH

    HAL_Delay(10);  // Delay to allow signals to stabilize

    // Read PC0 and PC1 again
    pc0_state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0);
    pc1_state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1);

    if (pc0_state == GPIO_PIN_RESET && pc1_state == GPIO_PIN_SET)
    {
        snprintf(message, sizeof(message), "Connection OK: PC0 is LOW (PC8), PC1 is HIGH (PC9)\n");
    }
    else
    {
        snprintf(message, sizeof(message), "Connection Failed: PC0 or PC1 state mismatch (Test 2)\n");
    }

    HAL_UART_Transmit(&huart3, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PC0 PC1 PC2 PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PC8 PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
