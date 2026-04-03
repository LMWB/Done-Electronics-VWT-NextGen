/**
  ******************************************************************************
  * @file           : LTC2494.c
  * @brief          : Driver for communication with Analog Devices LTC2494
  ******************************************************************************
  *
  * <h2><center>&copy; Copyright (c) 2025 DONE electronics GmbH.
  * All rights reserved.</center></h2>
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <LTC2494.h>

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/


/*@TODO ins EEPROM bei einer finalen SW, da dort beim einschalten kein Tara möglich ist*/
static int32_t TareOffset[8u] = {0x00};

/*Calibration factor for calculation of weight from ADC raw - manually edited after tests*/
static const float CalFactor[8u] =
{
	0.04415,
	0.03140,
	0.03297,
	0.030197,
	0.0330033,
	0.03434,
	0.034358,
	0.0341296
};


/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/

/**
  * @brief  LTC2494 Initialization function
  * @param  Address of the AFE-IC to be initialized
  * @retval none
  */
HAL_StatusTypeDef LTC2494_Init(uint8_t ChipAdr)
{
	HAL_StatusTypeDef ret = HAL_OK;
	uint32_t data = 0xA00000u;	//101xxx
	
	//wait till ready
	while(0x00u != LTC2494_ReadRDY(ChipAdr))
	{
		asm("nop");
	}

	data = data | CH1 | UPDATE | EXTERN | LINE_5060HZ | SINGLE | GAIN_X128;
	ret = LTC2494_Write(ChipAdr, data);
	
	if(HAL_OK != ret)
	{
		ret = HAL_ERROR;
	}
	return ret;
}


/**
  * @brief  LTC2494 Check for new Data Ready
  * @param  Address of the AFE-IC
  * @retval New Data Availability (0 = Yes, FF = No)
  */
uint8_t LTC2494_ReadRDY(uint8_t ChipAdr)
{
	GPIO_PinState RDYpin = GPIO_PIN_SET;
	uint8_t ret = 0x00u;
	
	/*@TODO aufbohren, dass über die adresse mehrere angesprochen werden können*/
	//CS low
	HAL_GPIO_WritePin(AFE_CS_PORT, AFE_CS_PIN, GPIO_PIN_RESET);
	//MISO Pin indicates conversion status when no SPI transfer is running
	RDYpin = HAL_GPIO_ReadPin(AFE_MISO_PORT, AFE_MISO_PIN);
	//CS high
	HAL_GPIO_WritePin(AFE_CS_PORT, AFE_CS_PIN, GPIO_PIN_SET);
	
	if(GPIO_PIN_SET == RDYpin)
	{
		ret = 0xFFu;
	}
	return ret;
}


/**
  * @brief  LTC2494 Write Data to a register
  * @param  Address of the AFE-IC
  * @param	Data to be written
  * @retval Error while data transfer Yes/No
  */
HAL_StatusTypeDef LTC2494_Write(uint8_t ChipAdr, uint32_t data)
{
	HAL_StatusTypeDef ret = HAL_OK;
	uint8_t dat[3u] = {0x00u};

	//Daten MSB First ins Array legen
	dat[0u] = (uint8_t)((data >> 16u) & 0xFFu);
	dat[1u] = (uint8_t)((data >> 8u) & 0xFFu);
	dat[2u] = (uint8_t)(data & 0xFFu);

	//CS low
	HAL_GPIO_WritePin(AFE_CS_PORT, AFE_CS_PIN, GPIO_PIN_RESET);
	ret = HAL_SPI_Transmit(&AFE_SPI_PORT, dat, 3u, TIMEOUT);
	//CS high
	HAL_GPIO_WritePin(AFE_CS_PORT, AFE_CS_PIN, GPIO_PIN_SET);

	return ret;
}


/**
  * @brief  LTC2494 Read ADC Data
  * @param  Address of the AFE-IC
  * @param	ADC raw Data from AFE
  * @retval Error while data transfer Yes/No
  */
HAL_StatusTypeDef LTC2494_ReadADCData(uint8_t ChipAdr, CHSELECT Channel, int32_t *adcdata)
{
	HAL_StatusTypeDef ret = HAL_OK;
	uint8_t adctemp[3u] = { 0x00u };
	uint8_t dat[3u] = { 0x00u };
	uint32_t data = 0xA00000u;	//101xxx;
	uint32_t adcdat = 0x00u;

	data = data | Channel | KEEP;
	dat[0u] = (uint8_t)((data >> 16u) & 0xFFu);
	dat[1u] = (uint8_t)((data >> 8u) & 0xFFu);
	dat[2u] = (uint8_t)(data & 0xFFu);
	
	//CS low
	HAL_GPIO_WritePin(AFE_CS_PORT, AFE_CS_PIN, GPIO_PIN_RESET);
	ret = HAL_SPI_TransmitReceive(&AFE_SPI_PORT, dat, adctemp, 3u, TIMEOUT);
	//CS high
	HAL_GPIO_WritePin(AFE_CS_PORT, AFE_CS_PIN, GPIO_PIN_SET);
	
	if(HAL_OK == ret)
	{
		adcdat = adcdat | (uint32_t)(adctemp[0u] << 16u);
		adcdat = adcdat | (uint32_t)(adctemp[1u] << 8u);
		adcdat = adcdat | (uint32_t)adctemp[2u];
		
		data = 0x00u;
		data = (adcdat & 0xF00000u) >> 20u;

		//Check for conversion end
		if(0x00u == (data & 0x08u))
		{
			//Check for overflow
			if((0x03u == (data & 0x03u)) || (0x00u == (data & 0x03u)))
			{
				ret = HAL_ERROR;
				*adcdata = 0x00;
			}
			else
			{
				data = adcdat & 0x1FFFFu;	//mask status bits - only 17 bit adc raw is necessary
				
				//update sign information into result
				if(0x10000u == (data & 0x10000u))
				{
					//result negative
					*adcdata = 0xFFFE0000; //fill int32 (bit 31 to 17) missing bits with 1
					*adcdata = *adcdata | (int32_t)data;
				}
				else
				{
					*adcdata = (int32_t)(data);
				}
			}
		}
		else
		{
			ret = HAL_ERROR;
			*adcdata = 0x00;			
		}
	}
	else
	{
		*adcdata = 0x00u;
	}

	return ret;
}

/**
  * @brief  LTC2494 Tara all weight scales
  * @param  Address of the AFE-IC
  * @retval none
  */
void LTC2494_Tare(uint8_t ChipAdr)
{
	HAL_StatusTypeDef ret = HAL_OK;
	int32_t data = 0x00;
	int32_t tara = 0x00;

	//Get Tara for all Scales
	for(uint8_t k=0x00u; 8u > k; k++)
	{
		ret = LTC2494_ReadADCData(ChipAdr, (k << 16u), &data);
		if(HAL_OK == ret)
		{
			while(0x00u != LTC2494_ReadRDY(ChipAdr))
			{
				asm("nop");
			}
		}
		else
		{
			ret = HAL_ERROR;
			break;
		}
		
		for(uint8_t i=0x00u; 10u > i; i++)
		{
			ret = LTC2494_ReadADCData(ChipAdr, (k << 16u), &data);
			if(HAL_OK == ret)
			{
				while(0x00u != LTC2494_ReadRDY(ChipAdr))
				{
					asm("nop");
				}
				tara = tara + data;
			}
			else
			{
				ret = HAL_ERROR;
				break;
			}
		}
		TareOffset[k] = tara / 10;
		tara = 0x00;
	}
}

/**
  * @brief  LTC2494 Rescale ADC reading to weight in grams
  * @param  ADC raw value
  * @param	pointer to result scaled to weight in grams
  * @retval Error while data transfer Yes/No
  */
HAL_StatusTypeDef LTC2494_CalcWeight(int32_t adcraw, CHSELECT Channel, uint32_t *weight)
{
	HAL_StatusTypeDef ret = HAL_OK;
	uint8_t idx = 0x00u;
	//adc tar'ed daten: result = adc raw - tareOffset

	switch(Channel)
	{
	case CH1:
		idx = 0x00u;
		break;
	case CH2:
		idx = 0x01u;
		break;
	case CH3:
		idx = 0x02u;
		break;
	case CH4:
		idx = 0x03u;
		break;
	case CH5:
		idx = 0x04u;
		break;
	case CH6:
		idx = 0x05u;
		break;
	case CH7:
		idx = 0x06u;
		break;
	case CH8:
		idx = 0x07u;
		break;		
	default:
		ret = HAL_ERROR;
		break;
	}
	
	if(HAL_OK == ret)
	{
		adcraw = adcraw - TareOffset[idx];
		//Rescale ADC raw value to weight in grams
		*weight = (uint32_t)((float)adcraw * CalFactor[idx]);
	}
	return ret;
}











