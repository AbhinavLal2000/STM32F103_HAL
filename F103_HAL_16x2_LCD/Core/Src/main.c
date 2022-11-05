/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BTN_DELAY HAL_Delay(100);
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
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void LCD_send(char data, uint8_t rs)
{
	HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, rs);

	HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, ((data >> 3) & 0x01));
	HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, ((data >> 2) & 0x01));
	HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, ((data >> 1) & 0x01));
	HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, ((data >> 0) & 0x01));

	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 1);
	HAL_Delay(1);
	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, 0);
	HAL_Delay(1);
}

void LCD_cmd(char cmd)
{
	char data;
	data = ((cmd >> 4) & 0x0F);
	LCD_send(data, 0);
	data = (cmd & 0x0F);
	LCD_send(data, 0);
}

void LCD_data(char cmd)
{
	char bits;
	bits = ((cmd >> 4) & 0x0F);
	LCD_send(bits, 1);
	bits = (cmd & 0x0F);
	LCD_send(bits, 1);
}

void LCD_string(char* str)
{
	int i = 0;
	while(str[i] != 0)
	{
		LCD_data(str[i]);
		i++;
	}
}

void LCD_string_pos(int row, int col, char* str)
{
	switch (row)
	{
		case 0:
			col |= 0x80;
			break;

		case 1:
			col |= 0xC0;
			break;
	}
	LCD_cmd(col);
	LCD_string(str);
}

void LCD_clear(void)
{
	LCD_cmd(0x01);
	LCD_cmd(0x80);
}

void LCD_fill(int row, int col, char data)
{
	int i;
	if(row == 0)
	{
		LCD_cmd(0x80);
		for(i = 0; i < (16 - col); i++)
		{
			LCD_data(data);
		}
	}
	else if (row == 1)
	{
		LCD_cmd(0xC0);
		for(i = 0; i < (16 - col); i++)
		{
			LCD_data(data);
		}
	}
}

void LCD_init(void)
{
	char command[6] = {0x02, 0x28, 0x0C, 0x06, 0x01, 0x80};
	for(int i = 0; i < 6; i++)
	{
		LCD_cmd(command[i]);
	}
}

void LCD_timer(void)
{
	LCD_clear();
	LCD_string_pos(0, 5, "Timer");
	LCD_string_pos(1, 4, "00:00:00");
	LCD_cmd(0x0F);
	LCD_string_pos(1, 10, "");
	uint8_t s = 0, m = 0, h = 0, run = 0;
	int cursor_pos = 0;
	char buffer[3] = {0,0,0};
	char hb[3] = {0}; char mb[3] = {0}; char sb[3] = {0};
	while(1)
	{
		if(HAL_GPIO_ReadPin(BTN_SLCT_GPIO_Port, BTN_SLCT_Pin))
		{
			cursor_pos += 3;
			if(cursor_pos > 6)
			{
				cursor_pos = 0;
			}
			BTN_DELAY
			LCD_string_pos(1, 10-cursor_pos, "");
		}
		if(HAL_GPIO_ReadPin(BTN_UP_GPIO_Port, BTN_UP_Pin))
		{
			buffer[0] = 0; buffer[1] = 0; buffer[2] = 0;
			switch(cursor_pos)
			{
				case 0:
					s++;
					sprintf(buffer, "%d", s);
					if(s < 10)
					{
						LCD_string_pos(1, 11, buffer);
						LCD_string_pos(1, 10, "0");
						LCD_string_pos(1, 10, "");
					}
					else
					{
						LCD_string_pos(1, 10, buffer);
						LCD_string_pos(1, 10, "");
					}
					break;
				case 3:
					m++;
					sprintf(buffer, "%d", m);
					if(m < 10)
					{
						LCD_string_pos(1, 8, buffer);
						LCD_string_pos(1, 7, "0");
						LCD_string_pos(1, 7, "");
					}
					else
					{
						LCD_string_pos(1, 7, buffer);
						LCD_string_pos(1, 7, "");
					}
					break;
				case 6:
					h++;
					sprintf(buffer, "%d", h);
					if(h < 10)
					{
						LCD_string_pos(1, 5, buffer);
						LCD_string_pos(1, 4, "0");
						LCD_string_pos(1, 4, "");
					}
					else
					{
						LCD_string_pos(1, 4, buffer);
						LCD_string_pos(1, 4, "");
					}
					break;
			}
			BTN_DELAY
		}
		if(HAL_GPIO_ReadPin(BTN_DN_GPIO_Port, BTN_DN_Pin))
		{
			buffer[0] = 0; buffer[1] = 0; buffer[2] = 0;
			switch(cursor_pos)
			{
				case 0:
					s--;
					sprintf(buffer, "%d", s);
					if(s < 10)
					{
						LCD_string_pos(1, 11, buffer);
						LCD_string_pos(1, 10, "0");
						LCD_string_pos(1, 10, "");
					}
					else
					{
						LCD_string_pos(1, 10, buffer);
						LCD_string_pos(1, 10, "");
					}
					break;
				case 3:
					m--;
					sprintf(buffer, "%d", m);
					if(m < 10)
					{
						LCD_string_pos(1, 8, buffer);
						LCD_string_pos(1, 7, "0");
						LCD_string_pos(1, 7, "");
					}
					else
					{
						LCD_string_pos(1, 7, buffer);
						LCD_string_pos(1, 7, "");
					}
					break;
				case 6:
					h--;
					sprintf(buffer, "%d", h);
					if(h < 10)
					{
						LCD_string_pos(1, 5, buffer);
						LCD_string_pos(1, 4, "0");
						LCD_string_pos(1, 4, "");
					}
					else
					{
						LCD_string_pos(1, 4, buffer);
						LCD_string_pos(1, 4, "");
					}
					break;
			}
			BTN_DELAY
		}
		if(HAL_GPIO_ReadPin(BTN_OK_GPIO_Port, BTN_OK_Pin))
		{
			run = 1;
			LCD_cmd(0x0C);
			BTN_DELAY
		}
		if(run == 1)
		{
			if (s != 0)
			{
				s--;
				HAL_Delay(1000-2);
			}
			if(s == 0)
			{
				if (m != 0)
				{
					m--;
					s = 59;
				}
				if(m == 0)
				{
					if (h != 0)
					{
						h--;
						m = 60;
					}
				}
			}
			sprintf(hb, "%d", h);
			sprintf(mb, "%d", m);
			sprintf(sb, "%d", s);
			if(s < 10)
			{
				LCD_string_pos(1, 11, sb);
				LCD_string_pos(1, 10, "0");
				LCD_string_pos(1, 10, "");
			}
			else
			{
				LCD_string_pos(1, 10, sb);
				LCD_string_pos(1, 10, "");
			}
			if(m < 10)
			{
				LCD_string_pos(1, 8, mb);
				LCD_string_pos(1, 7, "0");
				LCD_string_pos(1, 7, "");
			}
			else
			{
				LCD_string_pos(1, 7, mb);
				LCD_string_pos(1, 7, "");
			}
			if(h < 10)
			{
				LCD_string_pos(1, 5, hb);
				LCD_string_pos(1, 4, "0");
				LCD_string_pos(1, 4, "");
			}
			else
			{
				LCD_string_pos(1, 4, hb);
				LCD_string_pos(1, 4, "");
			}
		}
	}
}

void LCD_stopwatch(void)
{
	LCD_clear();
	LCD_string_pos(0, 3, "Stopwatch");
	LCD_string_pos(1, 4, "00:00:00");
	uint8_t h = 0, m = 0, s = 0, run = 0;
	char hb[3] = {0}; char mb[3] = {0}; char sb[3] = {0};
	while(1)
	{
		if(HAL_GPIO_ReadPin(BTN_OK_GPIO_Port, BTN_OK_Pin))
		{
			run = 1;
			LCD_cmd(0x0C);
			BTN_DELAY
		}
		while(run)
		{
			s++;
			HAL_Delay(1000-2);
			if(s > 59)
			{
				s = 0;
				m++;
				if(m > 59)
				{
					m = 0;
					h++;
				}
			}

			sprintf(sb, "%d", s);
			sprintf(mb, "%d", m);
			sprintf(hb, "%d", h);
			if(s < 10)
			{
				LCD_string_pos(1, 11, sb);
				LCD_string_pos(1, 10, "0");
				LCD_string_pos(1, 10, "");
			}
			else
			{
				LCD_string_pos(1, 10, sb);
				LCD_string_pos(1, 10, "");
			}
			if(m < 10)
			{
				LCD_string_pos(1, 8, mb);
				LCD_string_pos(1, 7, "0");
				LCD_string_pos(1, 7, "");
			}
			else
			{
				LCD_string_pos(1, 7, mb);
				LCD_string_pos(1, 7, "");
			}
			if(h < 10)
			{
				LCD_string_pos(1, 5, hb);
				LCD_string_pos(1, 4, "0");
				LCD_string_pos(1, 4, "");
			}
			else
			{
				LCD_string_pos(1, 4, hb);
				LCD_string_pos(1, 4, "");
			}
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
  /* USER CODE BEGIN 2 */

  char *funcs[4] = {"> ", "Timer", "Stopwatch", "  "};
  int selector_pos = 0;

  LCD_init();
  LCD_string_pos(selector_pos, 0, funcs[0]);
  LCD_string_pos(0, 2, funcs[1]);
  LCD_string_pos(1, 2, funcs[2]);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(HAL_GPIO_ReadPin(BTN_SLCT_GPIO_Port, BTN_SLCT_Pin))
	  {
		  if(selector_pos == 0)
		  {
			  LCD_string_pos(selector_pos, 0, funcs[3]);
			  selector_pos = 1;
			  LCD_string_pos(selector_pos, 0, funcs[0]);
		  }
		  else if(selector_pos == 1)
		  {
			  LCD_string_pos(selector_pos, 0, funcs[3]);
			  selector_pos = 0;
			  LCD_string_pos(selector_pos, 0, funcs[0]);
		  }
		  BTN_DELAY
	  }

	  if(HAL_GPIO_ReadPin(BTN_OK_GPIO_Port, BTN_OK_Pin))
	  {
		  if(selector_pos == 0)
		  {
			  LCD_timer();
		  }
		  else if(selector_pos == 1)
		  {
			  LCD_stopwatch();
		  }
		  BTN_DELAY
	  }
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, D4_Pin|D5_Pin|D6_Pin|D7_Pin
                          |RS_Pin|RW_Pin|EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : D4_Pin D5_Pin D6_Pin D7_Pin
                           RS_Pin RW_Pin EN_Pin */
  GPIO_InitStruct.Pin = D4_Pin|D5_Pin|D6_Pin|D7_Pin
                          |RS_Pin|RW_Pin|EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : BTN_SLCT_Pin */
  GPIO_InitStruct.Pin = BTN_SLCT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BTN_SLCT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : BTN_OK_Pin BTN_UP_Pin BTN_DN_Pin */
  GPIO_InitStruct.Pin = BTN_OK_Pin|BTN_UP_Pin|BTN_DN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
