/**
  ******************************************************************************
  * @file    stm32f4_discovery.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    28-October-2011
  * @brief   This file provides set of firmware functions to manage Leds and
  *          push-button available on STM32F4-Discovery Kit from STMicroelectronics.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 
  
/* Includes ------------------------------------------------------------------*/
#include "stm32f4_discovery.h"

GPIO_TypeDef* GPIO_PORT[LEDn] = {LED4_GPIO_PORT, LED3_GPIO_PORT, LED5_GPIO_PORT, LED6_GPIO_PORT};
const uint16_t GPIO_PIN[LEDn] = {LED4_PIN, LED3_PIN, LED5_PIN, LED6_PIN};
GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {USER_BUTTON_GPIO_PORT }; 
const uint16_t BUTTON_PIN[BUTTONn] = {USER_BUTTON_PIN };

void STM_EVAL_LEDInit(Led_TypeDef Led)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* Enable the GPIO_LED Clock */
	__GPIOD_CLK_ENABLE();

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.Pin = GPIO_PIN[Led];
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIO_PORT[Led], &GPIO_InitStructure);
}

void STM_EVAL_LEDOn(Led_TypeDef Led)
{
	HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_SET);
}

void STM_EVAL_LEDOff(Led_TypeDef Led)
{
	HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET);
}


void STM_EVAL_LEDToggle(Led_TypeDef Led)
{
	HAL_GPIO_TogglePin(GPIO_PORT[Led], GPIO_PIN[Led]);
}

/**
  * @brief  Configures COM port.
  * @param  COM: Specifies the COM port to be configured.
  *   This parameter can be one of following parameters:
  *     @arg COM1
  *     @arg COM2
  * @param  USART_InitStruct: pointer to a USART_InitTypeDef structure that
  *   contains the configuration information for the specified USART peripheral.
  * @retval None
  */
void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct)
{
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.BaudRate = 115200;
	USART_InitStructure.WordLength = USART_WORDLENGTH_8B;
	USART_InitStructure.StopBits = USART_STOPBITS_1;
	USART_InitStructure.Parity = USART_PARITY_NONE;
	//USART_InitStructure.HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.Mode = USART_MODE_TX_RX;

	/* Enable GPIO clock */
	__GPIOC_CLK_ENABLE();
	__USART3_CLK_ENABLE();
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Alternate = GPIO_AF7_USART3;
  GPIO_InitStructure.Pin = GPIO_PIN_10 | GPIO_PIN_11;

	/* Connect PXx to USARTx_Tx/Rx*/
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* USART configuration */
	USART_HandleTypeDef USART_HandleStructure;
	USART_HandleStructure.Instance = USART3;
	USART_HandleStructure.Init = USART_InitStructure;
	
	HAL_USART_Init(&USART_HandleStructure);

	/* Enable USART */
	__HAL_USART_ENABLE(&USART_HandleStructure);

	/* Enable interrupt */
	__HAL_USART_ENABLE_IT(&USART_HandleStructure, USART_IT_RXNE);
}
