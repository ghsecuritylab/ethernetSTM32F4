/**
  ******************************************************************************
  * @file    stm32f4x7_eth_bsp.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-October-2011 
  * @brief   STM32F4x7 Ethernet hardware configuration.
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
#include "stm32f4xx_hal.h"
#include "stm32f4x7_eth_bsp.h"
#include "main.h"

uint32_t Eth_Link_PHYITConfig(uint16_t PHYAddress);
void Eth_Link_EXTIConfig(void);

void ETH_BSP_Config(void)
{
  /* Configure the PHY to generate an interrupt on change of link status */
  Eth_Link_PHYITConfig(DP83848_PHY_ADDRESS);

  /* Configure the EXTI for Ethernet link status. */
  Eth_Link_EXTIConfig();
}

/**
  * @brief  Configure the PHY to generate an interrupt on change of link status.
  * @param PHYAddress: external PHY address  
  * @retval None
  */
uint32_t Eth_Link_PHYITConfig(uint16_t PHYAddress)
{
	// TODO:
//  uint32_t tmpreg = 0;

//  /* Read MICR register */
//  tmpreg = HAL_ETH_ReadPHYRegister(PHYAddress, PHY_MICR);

//  /* Enable output interrupt events to signal via the INT pin */
//  tmpreg |= (uint32_t)PHY_MICR_INT_EN | PHY_MICR_INT_OE;
//  if(!(HAL_ETH_WritePHYRegister(PHYAddress, PHY_MICR, tmpreg)))
//  {
//    /* Return ERROR in case of write timeout */
//    return ETH_ERROR;
//  }

//  /* Read MISR register */
//  tmpreg = HAL_ETH_ReadPHYRegister(PHYAddress, PHY_MISR);

//  /* Enable Interrupt on change of link status */
//  tmpreg |= (uint32_t)PHY_MISR_LINK_INT_EN;
//  if(!(HAL_ETH_WritePHYRegister(PHYAddress, PHY_MISR, tmpreg)))
//  {
//    /* Return ERROR in case of write timeout */
//    return ETH_ERROR;
//  }
//  /* Return SUCCESS */
  return ETH_SUCCESS;   
}

/**
  * @brief  EXTI configuration for Ethernet link status.
  * @param PHYAddress: external PHY address  
  * @retval None
  */
void Eth_Link_EXTIConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  //EXTI_InitTypeDef EXTI_InitStructure;

  /* Configure INT pin as input */
  GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Pin = ETH_LINK_PIN;
  HAL_GPIO_Init(ETH_LINK_GPIO_PORT, &GPIO_InitStructure);

	//TODO:
//  /* Connect EXTI Line to INT Pin */
//  SYSCFG_EXTILineConfig(ETH_LINK_EXTI_PORT_SOURCE, ETH_LINK_EXTI_PIN_SOURCE);

//  /* Configure EXTI line */
//  EXTI_InitStructure.EXTI_Line = ETH_LINK_EXTI_LINE;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set the EXTI interrupt to the highest priority */
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

uint32_t ETH_CheckFrameReceived(void)
{
  /* check if last segment */
  if(((heth.RxDesc->Status & ETH_DMARXDESC_OWN) == (uint32_t)RESET) &&
     ((heth.RxDesc->Status & ETH_DMARXDESC_LS) != (uint32_t)RESET)) 
  {
    heth.RxFrameInfos.SegCount++;
    if (heth.RxFrameInfos.SegCount == 1)
    {
      heth.RxFrameInfos.FSRxDesc = heth.RxDesc;
    }
    heth.RxFrameInfos.LSRxDesc = heth.RxDesc;
    return 1;
  }

  /* check if first segment */
  else if(((heth.RxDesc->Status & ETH_DMARXDESC_OWN) == (uint32_t)RESET) &&
          ((heth.RxDesc->Status & ETH_DMARXDESC_FS) != (uint32_t)RESET)&&
            ((heth.RxDesc->Status & ETH_DMARXDESC_LS) == (uint32_t)RESET))
  {
    heth.RxFrameInfos.FSRxDesc = heth.RxDesc;
    heth.RxFrameInfos.LSRxDesc = NULL;
    heth.RxFrameInfos.SegCount = 1;   
    heth.RxDesc = (ETH_DMADescTypeDef*) (heth.RxDesc->Buffer2NextDescAddr);
  }

  /* check if intermediate segment */ 
  else if(((heth.RxDesc->Status & ETH_DMARXDESC_OWN) == (uint32_t)RESET) &&
          ((heth.RxDesc->Status & ETH_DMARXDESC_FS) == (uint32_t)RESET)&&
            ((heth.RxDesc->Status & ETH_DMARXDESC_LS) == (uint32_t)RESET))
  {
    (heth.RxFrameInfos.SegCount) ++;
    heth.RxDesc = (ETH_DMADescTypeDef*) (heth.RxDesc->Buffer2NextDescAddr);
  } 
  return 0;
}