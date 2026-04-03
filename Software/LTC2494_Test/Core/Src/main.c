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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define S1 0u
#define S2 2u
#define S3 4u
#define S4 6u
#define S5 8u
#define S6 10u
#define S7 12u
#define S8 14u
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

static uint32_t T200_NextExec = 0x00u;
static uint8_t CHsel = 0x01u;
static uint8_t newval = 0x00u;
static int32_t adcraw = 0;
static uint32_t newweight = 0u;
char buf[8] = { '0' };

static const uint8_t Coord_xy_Dist[] =
{
	52, 30,
	52, 47,
	52, 64,
	52, 81,
	52, 98,
	52, 115,
	52, 132,
	52, 149
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void UpdateDisplay(uint32_t value);
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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

	ST7735_Init(2u);
	fillScreen(WHITE);
	ST7735_WriteString(2,7,"Weight Scale DEMO",Font_7x10 , BLUE, WHITE);
	drawFastHLine(0, 23, 180, GREEN);
	drawFastHLine(0, 24, 180, GREEN);
	drawFastHLine(0, 25, 180, GREEN);

	ST7735_WriteString(10,35,"BOOTING...",Font_11x18 , MAGENTA, WHITE);

	/*error in case of wrong Code Pin Level*/
	if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(CODE_GPIO_Port, CODE_Pin))
	{
		ST7735_WriteString(10,35,"          ",Font_11x18 , WHITE, WHITE);
		ST7735_WriteString(15,35,"ERROR",Font_11x18 , RED, WHITE);
		ST7735_WriteString(5,65,"wrong AFE mounted",Font_7x10 , RED, WHITE);
		while(1)
		{
			asm("nop");
		}
	}

	if(HAL_ERROR == LTC2494_Init(0u))
	{	//entweder Communication error oder falscher AFE-Chip
		ST7735_WriteString(10,35,"          ",Font_11x18 , WHITE, WHITE);
		ST7735_WriteString(15,35,"ERROR",Font_11x18 , RED, WHITE);
		ST7735_WriteString(5,65,"AFE Init Error",Font_7x10 , RED, WHITE);
		while(1)
		{
			asm("nop");
		}
	}

	HAL_Delay(300u);

	LTC2494_Tare(0x00u);

	ST7735_WriteString(10,35,"          ",Font_11x18 , WHITE, WHITE);
	ST7735_WriteString(2,30,"CELL 1:",Font_7x10 , BLACK, WHITE);
	drawFastHLine(0, 42, 180, BLACK);
	ST7735_WriteString(2,47,"CELL 2:",Font_7x10 , BLACK, WHITE);
	drawFastHLine(0, 59, 180, BLACK);
	ST7735_WriteString(2,64,"CELL 3:",Font_7x10 , BLACK, WHITE);
	drawFastHLine(0, 76, 180, BLACK);
	ST7735_WriteString(2,81,"CELL 4:",Font_7x10 , BLACK, WHITE);
	drawFastHLine(0, 93, 180, BLACK);
	ST7735_WriteString(2,98,"CELL 5:",Font_7x10 , BLACK, WHITE);
	drawFastHLine(0, 110, 180, BLACK);
	ST7735_WriteString(2,115,"CELL 6:",Font_7x10 , BLACK, WHITE);
	drawFastHLine(0, 127, 180, BLACK);
	ST7735_WriteString(2,132,"CELL 7:",Font_7x10 , BLACK, WHITE);
	drawFastHLine(0, 144, 180, BLACK);
	ST7735_WriteString(2,149,"CELL 8:",Font_7x10 , BLACK, WHITE);

	ST7735_WriteString(Coord_xy_Dist[S1], Coord_xy_Dist[S1+1u], "999999", Font_7x10 , RED, WHITE);
	ST7735_WriteString(Coord_xy_Dist[S2], Coord_xy_Dist[S2+1u], "999999", Font_7x10 , RED, WHITE);
	ST7735_WriteString(Coord_xy_Dist[S3], Coord_xy_Dist[S3+1u], "999999", Font_7x10 , RED, WHITE);
	ST7735_WriteString(Coord_xy_Dist[S4], Coord_xy_Dist[S4+1u], "999999", Font_7x10 , RED, WHITE);
	ST7735_WriteString(Coord_xy_Dist[S5], Coord_xy_Dist[S5+1u], "999999", Font_7x10 , RED, WHITE);
	ST7735_WriteString(Coord_xy_Dist[S6], Coord_xy_Dist[S6+1u], "999999", Font_7x10 , RED, WHITE);
	ST7735_WriteString(Coord_xy_Dist[S7], Coord_xy_Dist[S7+1u], "999999", Font_7x10 , RED, WHITE);
	ST7735_WriteString(Coord_xy_Dist[S8], Coord_xy_Dist[S8+1u], "999999", Font_7x10 , RED, WHITE);

	//Start first Ch1 conversion before first task execution
	LTC2494_ReadADCData(0, CH1, &adcraw);

	T200_NextExec = HAL_GetTick() + 200u;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  if(HAL_GetTick() >= T200_NextExec)
	  {	/*Get new sensor data and evaluate them*/
		  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

		  switch(CHsel)
		  {
			  case 1u:
				  if(0xFFu == newval)
				  {
					  CHsel = 2u;
					  LTC2494_ReadADCData(0, CH2, &adcraw);
					  //adcraw contains now CH1 result
					  LTC2494_CalcWeight(adcraw, CH1, &newweight);
					  UpdateDisplay(newweight);
					  ST7735_WriteString(Coord_xy_Dist[S1], Coord_xy_Dist[S1+1u], buf, Font_7x10 , RED, WHITE);
				  }
				  break;
			  case 2u:
				  if(0xFFu == newval)
				  {
					  CHsel = 3u;
					  LTC2494_ReadADCData(0, CH3, &adcraw);
					  //adcraw contains now CH2 result
					  LTC2494_CalcWeight(adcraw, CH2, &newweight);
					  UpdateDisplay(newweight);
					  ST7735_WriteString(Coord_xy_Dist[S2], Coord_xy_Dist[S2+1u], buf, Font_7x10 , RED, WHITE);
				  }
				  break;
			  case 3u:
				  if(0xFFu == newval)
				  {
					  CHsel = 4u;
					  LTC2494_ReadADCData(0, CH4, &adcraw);
					  //adcraw contains now CH3 result
					  LTC2494_CalcWeight(adcraw, CH3, &newweight);
					  UpdateDisplay(newweight);
					  ST7735_WriteString(Coord_xy_Dist[S3], Coord_xy_Dist[S3+1u], buf, Font_7x10 , RED, WHITE);
				  }
				  break;
			  case 4u:
				  if(0xFFu == newval)
				  {
					  CHsel = 5u;
					  LTC2494_ReadADCData(0, CH5, &adcraw);
					  //adcraw contains now CH4 result
					  LTC2494_CalcWeight(adcraw, CH4, &newweight);
					  UpdateDisplay(newweight);
					  ST7735_WriteString(Coord_xy_Dist[S4], Coord_xy_Dist[S4+1u], buf, Font_7x10 , RED, WHITE);
				  }
				  break;
			  case 5u:
				  if(0xFFu == newval)
				  {
					  CHsel = 6u;
					  LTC2494_ReadADCData(0, CH6, &adcraw);
					  //adcraw contains now CH5 result
					  LTC2494_CalcWeight(adcraw, CH5, &newweight);
					  UpdateDisplay(newweight);
					  ST7735_WriteString(Coord_xy_Dist[S5], Coord_xy_Dist[S5+1u], buf, Font_7x10 , RED, WHITE);
				  }
				  break;
			  case 6u:
				  if(0xFFu == newval)
				  {
					  CHsel = 7u;
					  LTC2494_ReadADCData(0, CH7, &adcraw);
					  //adcraw contains now CH6 result
					  LTC2494_CalcWeight(adcraw, CH6, &newweight);
					  UpdateDisplay(newweight);
					  ST7735_WriteString(Coord_xy_Dist[S6], Coord_xy_Dist[S6+1u], buf, Font_7x10 , RED, WHITE);
				  }
				  break;
			  case 7u:
				  if(0xFFu == newval)
				  {
					  CHsel = 8u;
					  LTC2494_ReadADCData(0, CH8, &adcraw);
					  //adcraw contains now CH7 result
					  LTC2494_CalcWeight(adcraw, CH7, &newweight);
					  UpdateDisplay(newweight);
					  ST7735_WriteString(Coord_xy_Dist[S7], Coord_xy_Dist[S7+1u], buf, Font_7x10 , RED, WHITE);
				  }
				  break;
			  case 8u:
				  if(0xFFu == newval)
				  {
					  CHsel = 1u;
					  LTC2494_ReadADCData(0, CH1, &adcraw);
					  //adcraw contains now CH8 result
					  LTC2494_CalcWeight(adcraw, CH8, &newweight);
					  UpdateDisplay(newweight);
					  ST7735_WriteString(Coord_xy_Dist[S8], Coord_xy_Dist[S8+1u], buf, Font_7x10 , RED, WHITE);
				  }
				  break;
			  default:
				  CHsel = 1u;
				  break;
		  }

		  T200_NextExec = HAL_GetTick() + 200u;
	  }

	  if(0x00u == LTC2494_ReadRDY(0u))
	  {
		  newval = 0xFFu;
	  }
	  else
	  {
		  newval = 0x00u;
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
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
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
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

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
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DIS_CS_GPIO_Port, DIS_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, DIS_Reset_Pin|DIS_DC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CS_Pin DIS_CS_Pin */
  GPIO_InitStruct.Pin = CS_Pin|DIS_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : DIS_Reset_Pin DIS_DC_Pin */
  GPIO_InitStruct.Pin = DIS_Reset_Pin|DIS_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : MISO_RB_Pin */
  GPIO_InitStruct.Pin = MISO_RB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MISO_RB_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CODE_Pin */
  GPIO_InitStruct.Pin = CODE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(CODE_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void UpdateDisplay(uint32_t value)
{
	//ST7735_WriteString(20, 92, "              ", Font_7x10 , RED, WHITE);
	//umwandeln in String und auf Display ausgeben
	itoa(value, buf, 10u);
	if(10u > value)
	{
		buf[1u] = 0x20u; /*0x20 = Leerzeichen*/
		buf[2u] = 0x20u; /*0x20 = Leerzeichen*/
		buf[3u] = 0x20u; /*0x20 = Leerzeichen*/
		buf[4u] = 0x20u; /*0x20 = Leerzeichen*/
		buf[5u] = 0x20u; /*0x20 = Leerzeichen*/
	}
	else if(100u > value)
	{
		buf[2u] = 0x20u; /*0x20 = Leerzeichen*/
		buf[3u] = 0x20u; /*0x20 = Leerzeichen*/
		buf[4u] = 0x20u; /*0x20 = Leerzeichen*/
		buf[5u] = 0x20u; /*0x20 = Leerzeichen*/
	}
	else if(1000u > value)
	{
		buf[3u] = 0x20u; /*0x20 = Leerzeichen*/
		buf[4u] = 0x20u; /*0x20 = Leerzeichen*/
		buf[5u] = 0x20u; /*0x20 = Leerzeichen*/
	}
	else if(10000u > value)
	{
		buf[4u] = 0x20u; /*0x20 = Leerzeichen*/
		buf[5u] = 0x20u; /*0x20 = Leerzeichen*/
	}
}
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
