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
#include "dma.h"
#include "i2c.h"
#include "i2s.h"
#include "spi.h"
#include "usart.h"
#include "usb_host.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OneWire.h"
#include "DallasTemperature.h"
#include <stdio.h>
#include "SX1278.h"

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
OneWire_HandleTypeDef ow1;
DallasTemperature_HandleTypeDef dt1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len)
{
	HAL_UART_Transmit(&huart1, (uint8_t *) ptr, len, HAL_MAX_DELAY);
	return len;
}
void printAddress(CurrentDeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
	  printf("0x%02X ", deviceAddress[i]);
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
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_I2S2_Init();
  MX_SPI1_Init();
  MX_USB_HOST_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  uint8_t uartbuf[] = {"hellow world"};
  HAL_UART_Transmit(&huart1,uartbuf,sizeof(uartbuf),0xFFFF);
  OW_Begin(&ow1, &huart2);
  if(OW_Reset(&ow1) == OW_OK)
  {
    printf("[%8lu] OneWire 1 line devices are present :)\r\n", HAL_GetTick());
  }
  else
  {
    printf("[%8lu] OneWire 1 line no devices :(\r\n", HAL_GetTick());
  }
  DT_SetOneWire(&dt1, &ow1);
  // arrays to hold device address
  CurrentDeviceAddress insideThermometer;
  // locate devices on the bus
  printf("[%8lu] 1-line Locating devices...\r\n", HAL_GetTick());
  DT_Begin(&dt1);
  uint8_t numDevOneLine = DT_GetDeviceCount(&dt1);
  printf("[%8lu] 1-line Found %d devices.\r\n", HAL_GetTick(), numDevOneLine);

  for (int i = 0; i < numDevOneLine; ++i)
  {
  	if (!DT_GetAddress(&dt1, insideThermometer, i))
  		  printf("[%8lu] 1-line: Unable to find address for Device %d\r\n", HAL_GetTick(), i);
  	printf("[%8lu] 1-line: Device %d Address: ", HAL_GetTick(), i);
  	printAddress(insideThermometer);
  	printf("\r\n");
  	// set the resolution to 12 bit (Each Dallas/Maxim device is capable of several different resolutions)
  	DT_SetResolution(&dt1, insideThermometer, 12, true);
  	printf("[%8lu] 1-line: Device %d Resolution: %d\r\n", HAL_GetTick(), i, DT_GetResolution(&dt1, insideThermometer));
  }


  SX1278_hw_t SX1278_hw;
  SX1278_t SX1278;

  int master;
  int ret;

  char buffer[512];

  int message;
  int message_length;

	/* USER CODE BEGIN 2 */
//    master = HAL_GPIO_ReadPin(MODE_GPIO_Port, MODE_Pin); //pin switch to mode salve or master
  	master = 0;
	if (master == 1) {
		printf("Mode: Master\r\n");
		HAL_GPIO_WritePin(MODE_GPIO_Port, MODE_Pin, GPIO_PIN_RESET); //set
	} else {
		printf("Mode: Slave\r\n");
		HAL_GPIO_WritePin(MODE_GPIO_Port, MODE_Pin, GPIO_PIN_SET); //set
	}

	//initialize LoRa module
	SX1278_hw.dio0.port = DIO0_GPIO_Port;
	SX1278_hw.dio0.pin = DIO0_Pin;
	SX1278_hw.nss.port = NSS_GPIO_Port;
	SX1278_hw.nss.pin = NSS_Pin;
	SX1278_hw.reset.port = RESET_GPIO_Port;
	SX1278_hw.reset.pin = RESET_Pin;
	SX1278_hw.spi = &hspi1;

	SX1278.hw = &SX1278_hw;

	printf("Configuring LoRa module\r\n");
	SX1278_init(&SX1278, 434000000, SX1278_POWER_17DBM, SX1278_LORA_SF_7,
	SX1278_LORA_BW_125KHZ, SX1278_LORA_CR_4_5, SX1278_LORA_CRC_EN, 10);
	printf("Done configuring LoRaModule\r\n");

	if (master == 1) {
		ret = SX1278_LoRaEntryTx(&SX1278, 16, 2000);
	} else {
		ret = SX1278_LoRaEntryRx(&SX1278, 16, 2000);
	}
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if (master == 1) {
			printf("Master ...\r\n");
			HAL_Delay(1000);
			printf("Sending package...\r\n");

			message_length = sprintf(buffer, "Hello %d", message);
			ret = SX1278_LoRaEntryTx(&SX1278, message_length, 2000);
			printf("Entry: %d\r\n", ret);

			printf("Sending %s\r\n", buffer);
			ret = SX1278_LoRaTxPacket(&SX1278, (uint8_t*) buffer,
					message_length, 2000);
			message += 1;

			printf("Transmission: %d\r\n", ret);
			printf("Package sent...\r\n");

		} else {
		printf("Slave ...\r\n");
		HAL_Delay(800);
		printf("Receiving package...\r\n");

		ret = SX1278_LoRaRxPacket(&SX1278);
		printf("Received: %d\r\n", ret);
		if (ret > 0) {
			SX1278_read(&SX1278, (uint8_t*) buffer, ret);
			printf("Content (%d): %s\r\n", ret, buffer);
		}
		printf("Package received ...\r\n");

	}

	//change mode
//		if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(MODE_GPIO_Port, MODE_Pin)) {
//			printf("Changing mode\r\n");
//			master = ~master & 0x01;
		if (master == 1) {
				ret = SX1278_LoRaEntryTx(&SX1278, 16, 2000);
			} else {
				ret = SX1278_LoRaEntryRx(&SX1278, 16, 2000);
			}
			HAL_Delay(1000);
			while (GPIO_PIN_RESET == HAL_GPIO_ReadPin(MODE_GPIO_Port, MODE_Pin));
		}


	// call sensors.requestTemperatures() to issue a global temperature
	// request to all devices on the bus
	printf("[%8lu] 1-line: Requesting temperatures...", HAL_GetTick());
	DT_RequestTemperatures(&dt1); // Send the command to get temperatures
	printf("\r\n[%8lu] 1-line: DONE\r\n", HAL_GetTick());
	for (int i = 0; i < numDevOneLine; ++i)
	{
		// After we got the temperatures, we can print them here.
		// We use the function ByIndex, and as an example get the temperature from the first sensor only.
		printf("[%8lu] 1-line: Temperature for the device %d (index %d) is: %.2f\r\n", HAL_GetTick(), i, i, DT_GetTempCByIndex(&dt1, i));
	}

	HAL_Delay(DT_MillisToWaitForConversion(DT_GetAllResolution(&dt1)));

    /* USER CODE END WHILE */
    MX_USB_HOST_Process();

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
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
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
