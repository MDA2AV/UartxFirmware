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
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RX_BUFFER_SIZE 100
#define UART_TIMEOUT 1000 // Timeout for UART receive in milliseconds
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart3;
uint8_t rxBuffer[RX_BUFFER_SIZE];
uint8_t rxChar;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
 * @brief Resets the output state of GPIO pins.
 *
 * This function sets the output state of GPIO pins PC8 and PC9 to LOW.
 * It is typically used to reset or initialize the state of the pins.
 */
void reset_outputs(void)
{
    // Set GPIO pins PC8 and PC9 to LOW
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);
}

/**
 * @brief Handles input from sensors and transmits a corresponding value over UART.
 *
 * This function reads the states of two input pins (PC0 and PC1), representing two sensors.
 * Based on their states, it assigns a specific character (0, 1, 2, or 3) to `output`:
 * - '0': Both sensors are LOW (00).
 * - '1': Sensor 1 is LOW, and Sensor 2 is HIGH (01).
 * - '2': Sensor 1 is HIGH, and Sensor 2 is LOW (10).
 * - '3': Both sensors are HIGH (11).
 *
 * The resulting character, along with a newline, is transmitted over UART.
 */
void handle_inputs(void)
{
    // Read the state of Sensor 1 (PC0) and Sensor 2 (PC1)
    uint8_t sensor1 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0); // Sensor 1 state
    uint8_t sensor2 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1); // Sensor 2 state

    char output; // Variable to store the output character based on sensor states

    // Determine the output value based on the sensor states
    if (sensor1 == GPIO_PIN_RESET && sensor2 == GPIO_PIN_RESET)
    {
        output = '0'; // Both sensors are LOW (00)
    }
    else if (sensor1 == GPIO_PIN_RESET && sensor2 == GPIO_PIN_SET)
    {
        output = '1'; // Sensor 1 is LOW, Sensor 2 is HIGH (01)
    }
    else if (sensor1 == GPIO_PIN_SET && sensor2 == GPIO_PIN_RESET)
    {
        output = '2'; // Sensor 1 is HIGH, Sensor 2 is LOW (10)
    }
    else if (sensor1 == GPIO_PIN_SET && sensor2 == GPIO_PIN_SET)
    {
        output = '3'; // Both sensors are HIGH (11)
    }

    // Prepare the response string with the output character and a newline
    char response[3];
    snprintf(response, sizeof(response), "%c\n", output);

    // Transmit the response over UART
    HAL_UART_Transmit(&huart3, (uint8_t *)response, strlen(response), HAL_MAX_DELAY);
}

/**
 * @brief Controls the output state of specific GPIO pins based on input parameters.
 *
 * The function takes two parameters:
 * - `pin`: Specifies the target pin ('0' or '1').
 * - `state`: Specifies the desired state ('1' for HIGH, '0' for LOW).
 *
 * Depending on the `pin` and `state` values, the function sets or resets the
 * corresponding GPIO pin (PC8 or PC9). This allows controlling the output
 * logic levels of specific GPIOs dynamically.
 *
 * @param pin   The target pin ('0' or '1').
 *              - '0': Refers to GPIO_PIN_8 (PC8).
 *              - '1': Refers to GPIO_PIN_9 (PC9).
 * @param state The desired output state ('1' or '0').
 *              - '1': Sets the pin to HIGH state.
 *              - '0': Resets the pin to LOW state.
 *
 * Example Usage:
 * - `set_output('0', '1');` Sets GPIO_PIN_8 (PC8) to HIGH.
 * - `set_output('1', '0');` Sets GPIO_PIN_9 (PC9) to LOW.
 */
void set_output(char pin, char state) {
    // Check if the target pin is '0' (GPIO_PIN_8)
    if (pin == '0') {
        // Set GPIO_PIN_8 to HIGH if state is '1'
        if (state == '1') {
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
        }
        // Set GPIO_PIN_8 to LOW if state is '0'
        else if (state == '0') {
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
        }
    }
    // Check if the target pin is '1' (GPIO_PIN_9)
    else if (pin == '1') {
        // Set GPIO_PIN_9 to HIGH if state is '1'
        if (state == '1') {
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
        }
        // Set GPIO_PIN_9 to LOW if state is '0'
        else if (state == '0') {
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
        }
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
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // Clear the buffer before receiving new data
	     memset(rxBuffer, 0, RX_BUFFER_SIZE);

	     // Wait for a 5-character command over UART
	     if (HAL_UART_Receive(&huart3, rxBuffer, 5, UART_TIMEOUT) == HAL_OK)
	     {
	         // Check if the received command starts with 'r'
	         if (strncmp((char *)rxBuffer, "r", 1) == 0)
	         {
	             // Reset all outputs (custom function you defined)
	             reset_outputs();
	         }
	         // Check if the command starts with 'd'
	         else if (strncmp((char *)rxBuffer, "d", 1) == 0)
	         {
	             // Handle inputs (custom function you defined)
	             handle_inputs();
	             continue;
	         }
	         // Check if the command starts with 's'
	         else if (strncmp((char *)rxBuffer, "s", 1) == 0)
	         {
	             // Extract the pin and state from the received buffer
	             char pin = rxBuffer[1];   // Second character specifies the pin
	             char state = rxBuffer[2]; // Third character specifies the state

	             // Set the output based on the received pin and state
	             set_output(pin, state);
	         }
	         else
	         {
	             // Transmit an error message if the command is unknown
	             HAL_UART_Transmit(&huart3, "Unknown\n", 8, HAL_MAX_DELAY);
	             continue;
	         }
	     }

	     HAL_UART_Transmit(&huart3, rxBuffer, 5, HAL_MAX_DELAY);
  }
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
}
  /* USER CODE END 3 */

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
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{
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
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /*Configure GPIO pins : PC0 PC1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PC8 PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}


