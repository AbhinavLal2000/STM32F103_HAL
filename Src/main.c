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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "nokia.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticSemaphore_t osStaticMutexDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t lcd_task_handle;
const osThreadAttr_t lcd_task_handle_attributes = {
  .name = "LCD_Task",
  .stack_size = 128*4,
  .priority = (osPriority_t) osPriorityNormal
};

osThreadId_t button_task_handle;
const osThreadAttr_t button_task_handle_attributes = {
  .name = "Button_Task",
  .stack_size = 128*4,
  .priority = (osPriority_t) osPriorityNormal
};

/* Definitions for SPI_MUTEX */
osMutexId_t SPI_MUTEXHandle;
osStaticMutexDef_t SPI_MUTEXControlBlock;
const osMutexAttr_t SPI_MUTEX_attributes = {
  .name = "SPI_MUTEX",
  .cb_mem = &SPI_MUTEXControlBlock,
  .cb_size = sizeof(SPI_MUTEXControlBlock),
};
/* USER CODE BEGIN PV */
nokia_t n = {
  .h        = &hspi1,
  .DC_port  = N_DC_GPIO_Port,
  .DC_pin   = N_DC_Pin,
  .RST_port = N_RST_GPIO_Port,
  .RST_pin  = N_RST_Pin,
  .CS_port  = SPI1_CS_GPIO_Port,
  .CS_pin   = SPI1_CS_Pin
};

typedef struct
{
  int x;
  int y;

} enemy_sprite_t;

int x_index = 0;
int y_index = 0;
int score   = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
void StartDefaultTask(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint8_t read_btn_ok(void)
{
  HAL_Delay(10);
  return (uint8_t)HAL_GPIO_ReadPin(BTN_OK_GPIO_Port, BTN_OK_Pin);
}
uint8_t read_btn_up(void)
{
  HAL_Delay(10);
  return (uint8_t)HAL_GPIO_ReadPin(BTN_UP_GPIO_Port, BTN_UP_Pin);
}
uint8_t read_btn_dn(void)
{
  HAL_Delay(10);
  return (uint8_t)HAL_GPIO_ReadPin(BTN_DN_GPIO_Port, BTN_DN_Pin);
}
uint8_t read_btn_rt(void)
{
  HAL_Delay(10);
  return (uint8_t)HAL_GPIO_ReadPin(BTN_RT_GPIO_Port, BTN_RT_Pin);
}
uint8_t read_btn_lt(void)
{
  HAL_Delay(10);
  return (uint8_t)HAL_GPIO_ReadPin(BTN_LT_GPIO_Port, BTN_LT_Pin);
}

int get_random_number(void)
{
  return 5;
}

enemy_sprite_t* generate_enemy_sprite(void)
{
  int x = get_random_number();
  enemy_sprite_t *e = (enemy_sprite_t*)malloc(1*sizeof(enemy_sprite_t));
  e->x = x;
  e->y = 1;
  return e;
}

void lcd_task(void* args)
{
  x_index = 38;
  y_index = 1;
  while (1)
  {
    n_print(&n, y_index, 0, " ");
    y_index++;
    if (y_index > 5) y_index = 1;
    n_print(&n, y_index, 0, "#");
    HAL_Delay(500);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
}

void update_score(void)
{
  score++;
  char buffer[10] = {0};
  sprintf(buffer, "%d", score);
  n_print(&n, 0, 60, buffer);
}

void button_task(void* args)
{
  while (1)
  {
    if (read_btn_rt() == 1)
    {
      n_pos(&n, 5, x_index);
      n_raw_print(&n, 0b00000000);
      x_index++;
      if (x_index > 75) x_index = 75;
      n_pos(&n, 5, x_index);
      for (int i = x_index; i < (x_index+8); i++)
      {
        n_raw_print(&n, 0b11000000);
      }
    }
    if (read_btn_lt() == 1)
    {
      n_pos(&n, 5, x_index+7);
      n_raw_print(&n, 0b00000000);
      x_index--;
      if (x_index < 1) x_index = 1;
      n_pos(&n, 5, x_index);
      for (int i = x_index; i < (x_index+8); i++)
      {
        n_raw_print(&n, 0b11000000);
      }
    }
    if ((y_index == 5) && (x_index == 1))
    {
      n_print(&n, y_index, 0, " ");
      y_index = 0;
      n_pos(&n, 5, x_index);
      for (int i = x_index; i < (x_index+8); i++)
      {
        n_raw_print(&n, 0b11000000);
      }
      update_score();
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
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  n_init(&n);
  n_clear(&n);

/*   for (int i = 0; i < 84; i++)
  {
    n_raw_print(&n, 0x01);
  }
  for (int i = 0; i < 6; i++)
  {
    n_pos(&n, i, 0);
    n_raw_print(&n, 0xFF);
  }
  for (int i = 0; i < 83; i++)
  {
    n_raw_print(&n, 0x80);
  }
  for (int i = 0; i < 6; i++)
  {
    n_pos(&n, i, 83);
    n_raw_print(&n, 0xFF);
  }
 */
  n_pos(&n, 5, 38);
  n_raw_print(&n, 0b11000000);
  n_raw_print(&n, 0b11000000);
  n_raw_print(&n, 0b11000000);
  n_raw_print(&n, 0b11000000);
  n_raw_print(&n, 0b11000000);
  n_raw_print(&n, 0b11000000);
  n_raw_print(&n, 0b11000000);
  n_raw_print(&n, 0b11000000);
  update_score();

/*   n_pos(&n, 5, 75);
  n_raw_print(&n, 0b10000011);
  n_raw_print(&n, 0b10000011);
  n_raw_print(&n, 0b10000011);
  n_raw_print(&n, 0b10000011);
  n_raw_print(&n, 0b10000011);
  n_raw_print(&n, 0b10000011);
  n_raw_print(&n, 0b10000011);
  n_raw_print(&n, 0b10000011); */

  //snake_start_screen(&n, &s);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
  /* Create the mutex(es) */
  /* creation of SPI_MUTEX */
  SPI_MUTEXHandle = osMutexNew(&SPI_MUTEX_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
  

  /* USER CODE BEGIN RTOS_THREADS */
  lcd_task_handle    = osThreadNew(lcd_task, NULL, &lcd_task_handle_attributes);
  button_task_handle = osThreadNew(button_task, NULL, &button_task_handle_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  //osKernelStart();

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, N_DC_Pin|N_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI1_CS_Pin */
  GPIO_InitStruct.Pin = SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI1_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : N_DC_Pin N_RST_Pin */
  GPIO_InitStruct.Pin = N_DC_Pin|N_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : BTN_UP_Pin BTN_DN_Pin BTN_LT_Pin BTN_RT_Pin */
  GPIO_InitStruct.Pin = BTN_UP_Pin|BTN_DN_Pin|BTN_LT_Pin|BTN_RT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : BTN_OK_Pin */
  GPIO_InitStruct.Pin = BTN_OK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BTN_OK_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    osDelay(500);
  }
  /* USER CODE END 5 */
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
