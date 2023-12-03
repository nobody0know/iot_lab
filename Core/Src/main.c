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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t rx2_buffer[256];
uint8_t rx1_buffer[256];
uint8_t trans_mode = 0;
uint8_t config_flag = 0;
uint8_t config_ab = 0;
char at_test[20] = {"AT\r\n"};
char at_restart[20] = {"AT+RESET\r\n"};
char at_open_showback[20]={"ATE1\r\n"};
char at_close_showback[20]={"ATE0\r\n"};

char at_search_cwmode[20] = {"AT+CWMODE?\r\n"};//閺屻儴顕楀銉ょ稊濡?崇础
char at_set_cwmode_nomal[20] = {"AT+CWMODE=0\r\n"};
char at_set_cwmode_lowpower[20] = {"AT+CWMODE=2\r\n"};
char at_set_cwmode_weakup[20] = {"AT+CWMODE=1\r\n"};

char at_search_tmode[20] = {"AT+TMODE?\r\n"};//閺屻儴顕楅崣鎴??浣哄Ц閹??
char at_set_tmode_transparent[20] = {"AT+TMODE=0\r\n"};//闁繑妲戞导鐘虹翻
char at_set_tmode_orientation[20] = {"AT+TMODE=1\r\n"};//鐎规氨鍋ｆ导鐘虹翻

char at_search_wlrate[20] = {"AT+WLRATE?\r\n"};//閺屻儴顕楅弮鐘靛殠闁喓宸奸崪灞间繆闁??
char at_set_wlrate_a[30] = {"AT+WLRATE=11,4\r\n"};//娣囷繝浜?11 闁喓宸?9.6kbps
char at_set_wlrate_b[30] = {"AT+WLRATE=12,4\r\n"};//娣囷繝浜?11 闁喓宸?9.6kbps

char at_search_address[20] = {"AT+ADDR?\r\n"};//閺屻儴顕楃拋鎯ь槵閸︽澘娼?
char at_set_address_a[20] = {"AT+ADDR=11,45\r\n"};
char at_set_address_b[20] = {"AT+ADDR=11,46\r\n"};

char at_set_uart_baudrate_115200[20] = {"AT+UART=7,0\r\n"};

char choose_mode[100] = {"\nplease choose transport mode:\n 1.transparent mode\n 2.orientation mode\r\n"};
char choose_work[100] = {"\nPlease select whether you want to configure it:\n 1.need\n 2.no\r\n"};
char choose_board[100] = {"\nPlease select which board you are on:\n 1.A board\n 2.B board\r\n"};

char error[30] = {"something was wrong!\r\n"};
void AT_lora_mode_set();
void AT_lora_inquire();
void AT_lora_init()
{
    HAL_Delay(100);
    HAL_UART_Transmit(&huart2,(uint8_t *)at_test,strlen(at_test),1000);
    HAL_Delay(100);
    AT_lora_inquire();
    HAL_Delay(100);
    HAL_UART_Transmit(&huart1,(uint8_t *)choose_work,strlen(choose_work),1000);
    while (config_flag==0)
    {
    }
    if(config_flag==1)
    {
        AT_lora_mode_set();
    }
    else
    {
        AT_lora_inquire();
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET);
        HAL_UART_Transmit(&huart2,(uint8_t *)at_restart,strlen(at_restart),1000);
        HAL_Delay(100);
    }

}
void AT_lora_mode_set()
{
    HAL_UART_Transmit(&huart1,(uint8_t *)choose_mode,strlen(choose_mode),1000);
    while(trans_mode==0)
    {
    }
    if(trans_mode==1)//瀹氬悜浼犺緭
    {
        HAL_UART_Transmit(&huart2,(uint8_t *)at_open_showback,strlen(at_open_showback),1000);
        HAL_Delay(100);
        HAL_UART_Transmit(&huart2,(uint8_t *)at_set_tmode_transparent,strlen(at_set_tmode_transparent),1000);
        HAL_Delay(100);

        HAL_UART_Transmit(&huart2,(uint8_t *)at_set_wlrate_a,strlen(at_set_wlrate_a),1000);
        HAL_Delay(100);

        HAL_UART_Transmit(&huart2,(uint8_t *)at_set_address_a,strlen(at_set_address_a),1000);
        HAL_Delay(100);

        HAL_UART_Transmit(&huart2,(uint8_t *)at_set_uart_baudrate_115200,strlen(at_set_uart_baudrate_115200),1000);
        HAL_Delay(100);
        AT_lora_inquire();
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET);
        HAL_UART_Transmit(&huart2,(uint8_t *)at_restart,strlen(at_restart),1000);
        HAL_Delay(100);

    }
    else if(trans_mode == 2)
    {
        HAL_UART_Transmit(&huart2,(uint8_t *)at_open_showback,strlen(at_open_showback),1000);
        HAL_Delay(100);
        HAL_UART_Transmit(&huart2,(uint8_t *)at_set_tmode_orientation,strlen(at_set_tmode_orientation),1000);
        HAL_Delay(100);
        HAL_UART_Transmit(&huart1,(uint8_t *)choose_board,strlen(choose_board),1000);
        while (config_ab==0)
        {

        }
        if(config_ab==1)
        {
            HAL_UART_Transmit(&huart2,(uint8_t *)at_set_wlrate_a,strlen(at_set_wlrate_a),1000);
            HAL_Delay(100);

            HAL_UART_Transmit(&huart2,(uint8_t *)at_set_address_a,strlen(at_set_address_a),1000);
            HAL_Delay(100);
        }
        else if(config_ab == 2)
        {
            HAL_UART_Transmit(&huart2,(uint8_t *)at_set_wlrate_b,strlen(at_set_wlrate_b),1000);
            HAL_Delay(100);

            HAL_UART_Transmit(&huart2,(uint8_t *)at_set_address_b,strlen(at_set_address_b),1000);
            HAL_Delay(100);
        }

        HAL_UART_Transmit(&huart2,(uint8_t *)at_set_uart_baudrate_115200,strlen(at_set_uart_baudrate_115200),1000);
        HAL_Delay(100);
        AT_lora_inquire();
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET);
        HAL_UART_Transmit(&huart2,(uint8_t *)at_restart,strlen(at_restart),1000);
        HAL_Delay(100);
    }
    else
    {
        HAL_UART_Transmit(&huart1,(uint8_t *)error,strlen(error),1000);

    }

}
void AT_lora_inquire()
{
//    HAL_UART_Transmit(&huart2,(uint8_t *)at_close_showback,strlen(at_close_showback),1000);
//    HAL_Delay(100);
    HAL_UART_Transmit(&huart2,(uint8_t *)at_search_address,strlen(at_search_address),1000);
    HAL_Delay(100);
    HAL_UART_Transmit(&huart2,(uint8_t *)at_search_wlrate,strlen(at_search_wlrate),1000);
    HAL_Delay(100);
    HAL_UART_Transmit(&huart2,(uint8_t *)at_search_cwmode,strlen(at_search_cwmode),1000);
    HAL_Delay(100);
    HAL_UART_Transmit(&huart2,(uint8_t *)at_search_tmode,strlen(at_search_tmode),1000);
    HAL_Delay(100);
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
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

    __HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);
    __HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);
    AT_lora_init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


      HAL_Delay(10);
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
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
