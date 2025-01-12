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
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart3;
char rx_buffer[1];                  // Temporary buffer for single-byte UART reception
char command_buffer[RX_BUFFER_SIZE]; // Buffer to store received command
volatile uint8_t command_ready_flag = 0;  // Flag indicating a command is ready

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);

/* USER CODE BEGIN PFP */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
//void reset_outputs(void);
//void handle_inputs(void);
//void set_output(char pin, char state);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
 * @brief UART receive complete callback.
 *
 * This callback is triggered when a UART receive interrupt occurs for USART3. It processes incoming
 * characters and builds a command string. When a newline ('\n') or carriage return ('\r') is received,
 * the command is considered complete, and the `command_ready_flag` is set.
 *
 * Function behavior:
 * - Characters are appended to `command_buffer` until a newline ('\n') or carriage return ('\r') is received.
 * - If the received character completes a command, the buffer is null-terminated, and the `command_ready_flag` is set.
 * - If the buffer overflows, the index is reset to prevent overwriting memory.
 * - After processing the character, UART interrupt reception is re-enabled.
 *
 * @param huart Pointer to the UART handle that triggered the callback.
 *
 * @note This function assumes that `rx_buffer`, `command_buffer`, `command_ready_flag`, and `RX_BUFFER_SIZE`
 *       are properly defined and initialized elsewhere in the program.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    static uint8_t rx_index = 0; // Index for tracking position in the command buffer.

    if (huart->Instance == USART3) { // Ensure the callback is for USART3.
        char received_char = rx_buffer[0]; // Retrieve the received character.

        if (received_char == '\n' || received_char == '\r') {
            command_buffer[rx_index] = '\0'; // Null-terminate the command string.
            command_ready_flag = 1;         // Signal that a complete command is ready to process.
            rx_index = 0;                   // Reset the index for the next command.
        } else if (rx_index < RX_BUFFER_SIZE - 1) {
            command_buffer[rx_index++] = received_char; // Append the character to the command buffer.
        } else {
            rx_index = 0; // Reset the index if the buffer overflows.
        }

        // Re-enable UART interrupt reception for the next character.
        HAL_UART_Receive_IT(&huart3, (uint8_t *)rx_buffer, 1);
    }
}

/**
 * @brief Resets the output state of GPIO pins.
 */
void reset_outputs(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);
}

/**
 * @brief Handles input from sensors and transmits a corresponding value over UART.
 *
 * This function reads the state of two sensors connected to GPIO pins, determines a specific output
 * value based on their states, and sends this value as a character over UART. The output is encoded
 * as follows:
 *
 * - '0': Both sensors are in the GPIO_PIN_RESET state.
 * - '1': Sensor 1 is GPIO_PIN_RESET and Sensor 2 is GPIO_PIN_SET.
 * - '2': Sensor 1 is GPIO_PIN_SET and Sensor 2 is GPIO_PIN_RESET.
 * - '3': Both sensors are in the GPIO_PIN_SET state.
 *
 * The transmitted character is followed by a newline character for readability.
 *
 * @note The function assumes that HAL_UART_Transmit and HAL_GPIO_ReadPin are properly initialized and functional.
 */
void handle_inputs(void)
{
    uint8_t sensor1 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0); // Read the state of Sensor 1.
    uint8_t sensor2 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1); // Read the state of Sensor 2.

    char output;
    if (sensor1 == GPIO_PIN_RESET && sensor2 == GPIO_PIN_RESET)
        output = '0'; // Both sensors are inactive.
    else if (sensor1 == GPIO_PIN_RESET && sensor2 == GPIO_PIN_SET)
        output = '1'; // Sensor 1 inactive, Sensor 2 active.
    else if (sensor1 == GPIO_PIN_SET && sensor2 == GPIO_PIN_RESET)
        output = '2'; // Sensor 1 active, Sensor 2 inactive.
    else
        output = '3'; // Both sensors are active.

    char response[3];
    snprintf(response, sizeof(response), "%c\n", output); // Format the output with a newline.
    HAL_UART_Transmit(&huart3, (uint8_t *)response, strlen(response), HAL_MAX_DELAY); // Transmit the response over UART.
}

/**
 * @brief Controls the output state of specific GPIO pins based on input parameters.
 *
 * This function sets the state of GPIO pins based on the provided `pin` and `state` parameters.
 * The function supports two pins ('0' and '1') and two states ('1' for GPIO_PIN_SET and '0' for GPIO_PIN_RESET).
 *
 * Function behavior:
 * - If `pin` is '0' and `state` is '1', GPIO_PIN_8 is set to GPIO_PIN_SET.
 * - If `pin` is '0' and `state` is '0', GPIO_PIN_8 is set to GPIO_PIN_RESET.
 * - If `pin` is '1' and `state` is '1', GPIO_PIN_9 is set to GPIO_PIN_SET.
 * - If `pin` is '1' and `state` is '0', GPIO_PIN_9 is set to GPIO_PIN_RESET.
 *
 * @param pin The pin identifier ('0' or '1').
 * @param state The desired state ('1' for GPIO_PIN_SET, '0' for GPIO_PIN_RESET).
 *
 * @note This function assumes that HAL_GPIO_WritePin is properly initialized and functional, and
 *       the GPIO pins GPIO_PIN_8 and GPIO_PIN_9 are configured as output pins.
 */
void set_output(char pin, char state)
{
    if (pin == '0') {
        if (state == '1')
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET); // Set GPIO_PIN_8 to high.
        else if (state == '0')
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET); // Set GPIO_PIN_8 to low.
    } else if (pin == '1') {
        if (state == '1')
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET); // Set GPIO_PIN_9 to high.
        else if (state == '0')
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET); // Set GPIO_PIN_9 to low.
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * This function initializes the system and enters an infinite loop to handle UART commands.
  * The application supports the following commands:
  *
  * - "r": Resets all outputs to their default state and sends a confirmation message.
  * - "d": Reads inputs from sensors and transmits their corresponding state via UART.
  * - "sXY": Sets the output state of a specific pin.
  *     - X specifies the pin ('0' or '1').
  *     - Y specifies the state ('0' for GPIO_PIN_RESET, '1' for GPIO_PIN_SET).
  *     - Example: "s01" sets pin '0' to GPIO_PIN_SET.
  *
  * Invalid or unrecognized commands return an "Unknown Command" message.
  *
  * @retval int Always returns 0 (application does not terminate).
  */
int main(void)
{
    HAL_Init();                           // Initialize the HAL library.
    SystemClock_Config();                 // Configure the system clock.
    MX_GPIO_Init();                       // Initialize GPIO pins.
    MX_USART3_UART_Init();                // Initialize UART3 for communication.

    HAL_UART_Receive_IT(&huart3, (uint8_t *)rx_buffer, 1); // Enable UART reception in interrupt mode.

    while (1) {
        if (command_ready_flag) {         // Check if a command has been received and is ready to process.
            if (strcmp(command_buffer, "r") == 0) {
                reset_outputs();          // Reset all outputs to their default state.
                HAL_UART_Transmit(&huart3, (uint8_t *)"Outputs Reset\n", 15, HAL_MAX_DELAY); // Confirm reset.
            } else if (strcmp(command_buffer, "d") == 0) {
                handle_inputs();          // Handle sensor inputs and transmit their state via UART.
            } else if (strncmp(command_buffer, "s", 1) == 0 && strlen(command_buffer) == 3) {
                char pin = command_buffer[1];   // Extract the pin identifier from the command.
                char state = command_buffer[2]; // Extract the state from the command.

                // Validate pin and state.
                if ((pin == '0' || pin == '1') && (state == '0' || state == '1')) {
                    set_output(pin, state);     // Set the specified output pin to the desired state.
                    HAL_UART_Transmit(&huart3, (uint8_t *)"Output Set\n", 11, HAL_MAX_DELAY); // Confirm action.
                } else {
                    HAL_UART_Transmit(&huart3, (uint8_t *)"Invalid sXY Format\n", 20, HAL_MAX_DELAY); // Invalid format.
                }
            } else {
                HAL_UART_Transmit(&huart3, (uint8_t *)"Unknown Command\n", 17, HAL_MAX_DELAY); // Unrecognized command.
            }
            command_ready_flag = 0;       // Reset the command ready flag after processing.
        }
    }
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

/*
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */ /*
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
/*
}
*/

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

  /*Configure GPIO pins : PC1 PC2 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
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
