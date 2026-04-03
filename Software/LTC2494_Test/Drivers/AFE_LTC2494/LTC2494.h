/**
  ******************************************************************************
  * @file           : LTC2494.h
  * @brief          : Driver for communication with Analog Devices LTC2494
  *                   This file contains the common defines of the application.
  ******************************************************************************
  *
  * <h2><center>&copy; Copyright (c) 2025 DONE electronics GmbH.
  * All rights reserved.</center></h2>
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LTC2494_H
#define __LTC2494_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32F1xx_hal.h"
#include "main.h"

#ifdef DEBUG
#include <stdio.h>
#endif

/* Exported types ------------------------------------------------------------*/

extern SPI_HandleTypeDef hspi1;
#define AFE_SPI_PORT hspi1

#define LTC2494


/****** PIN DEFINES ******/
#define AFE_CS_PORT GPIOB
#define AFE_CS_PIN  GPIO_PIN_10
#define AFE_MISO_PORT GPIOB
#define AFE_MISO_PIN  GPIO_PIN_4

/**
 * @enum ADC Channel
 * @brief CMD register bits SGL, ODD, A0 - A2 setting values for ADC Channel selection (Differential only)
 */
typedef enum CHSELECT_t
{
	CH1 = (0x00u << 16u),
	CH2 = (0x01u << 16u),
	CH3 = (0x02u << 16u),
	CH4 = (0x03u << 16u),
	CH5 = (0x04u << 16u),
	CH6 = (0x05u << 16u),
	CH7 = (0x06u << 16u),
	CH8 = (0x07u << 16u),
	CH1i = (0x08u << 16u),
	CH2i = (0x09u << 16u),
	CH3i = (0x0Au << 16u),
	CH4i = (0x0Bu << 16u),
	CH5i = (0x0Cu << 16u),
	CH6i = (0x0Du << 16u),
	CH7i = (0x0Eu << 16u),
	CH8i = (0x0Fu << 16u)
}CHSELECT;

/**
 * @enum ADC Conversion mode
 * @brief CMD register bit IM setting values for ADC conversion mode (ext. Input or internal TempSensor)
 */
typedef enum MODESELECT_t
{
	EXTERN 		= (0x00u << 14u),
	TEMPSENS 	= (0x01u << 14u)
}MODESELECT;

/**
 * @enum ADC Configuration Update
 * @brief CMD register bit EN2 setting value for Keep or Update ADC configuration
 */
typedef enum CONFIGSELECT_t
{
	KEEP 	= (0x00u << 15u),
	UPDATE 	= (0x01u << 15u)
}CONFIGSELECT;

/**
 * @enum Rejection Filter Configuration
 * @brief CMD register bit FA, FB setting value for Line rejection filter Setting
 */
typedef enum FILTERSELECT_t
{
	LINE_5060HZ 	= (0x00u << 11u),
	LINE_50HZ 		= (0x01u << 11u),
	LINE_60HZ 		= (0x02u << 11u)
	
}FILTERSELECT;

/**
 * @enum ADC Speed mode
 * @brief CMD register bit EN2 setting value for "normal" speed (incl. Auto-calibration) or double speed (w/o Auto-calibration)
 */
typedef enum SPEEDSELET_t
{
	SINGLE = (0x00u << 10u),
	DOUBLE = (0x01u << 10u)
}SPEEDSELECT;

/**
 * @enum ADC Gain Selection
 * @brief CMD register bit GS0 - GS2 setting value for ADC PGA Gain selection
 */
typedef enum GAINSELECT_t
{
	GAIN_X1		= (0x00u << 7u),
	GAIN_X4		= (0x01u << 7u),
	GAIN_X8		= (0x02u << 7u),
	GAIN_X16	= (0x03u << 7u),
	GAIN_X32	= (0x04u << 7u),
	GAIN_X64	= (0x05u << 7u),
	GAIN_X128	= (0x06u << 7u)
}GAINSELECT;

/* Exported constants --------------------------------------------------------*/


/* Exported functions prototypes ---------------------------------------------*/

/*Called once on startup*/
HAL_StatusTypeDef LTC2494_Init(uint8_t ChipAdr);

/*Called cyclic*/
/*Called after corresponding interrupt triggered*/
/*Called on APP request*/
uint8_t LTC2494_ReadRDY(uint8_t ChipAdr); /*fertig*/
HAL_StatusTypeDef LTC2494_ReadADCData(uint8_t ChipAdr, CHSELECT Channel, int32_t *adcdata);
void LTC2494_Tare(uint8_t ChipAdr);
HAL_StatusTypeDef LTC2494_CalcWeight(int32_t adcraw, CHSELECT Channel, uint32_t *weight);

/*Internally called*/
HAL_StatusTypeDef LTC2494_Write(uint8_t ChipAdr, uint32_t data);


/* Private defines -----------------------------------------------------------*/

#define TIMEOUT 100u			/*Timeout in ms for SPI communication*/

/****** Bitwidth DEFINES ******/
#ifdef LTC2494
#define BITWIDTH 16u
#endif

#ifdef __cplusplus
}
#endif

#endif /* __LTC2494_H */
