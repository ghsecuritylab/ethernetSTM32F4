/**
  ******************************************************************************
  * @file    httpd_cg_ssi.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-October-2011
  * @brief   Webserver SSI and CGI handlers
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


#include "lwip/debug.h"
#include "httpd_def.h"
#include "httpd.h"
#include "lwip/tcp.h"
#include "fs.h"
#include "main.h"

#include <string.h>
#include <stdlib.h>
#include <stdlib.h>

tSSIHandler ADC_Page_SSI_Handler;
uint32_t ADC_not_configured=1;

/* we will use character "t" as tag for CGI */
char const* TAGCHAR="t";
char const* TAGCHAR2="u";
char const ** TAGS; // = { &TAGCHAR, TAGCHAR2 };

const char * LEDS_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);

/* Html request for "/leds.cgi" will start LEDS_CGI_Handler */
const tCGI LEDS_CGI={"/leds.cgi", LEDS_CGI_Handler};

/* Cgi call table, only one CGI used */
tCGI CGI_TAB[1];

/**
  * @brief  Configures the ADC.
  * @param  None
  * @retval None
  */
static void ADC_Configuration(void)
{
	TAGS = malloc(sizeof(char *) *2);
	TAGS[0] = TAGCHAR;
	TAGS[1] = TAGCHAR2;
	
	ADC_HandleTypeDef ADC_HandleStructure;
  ADC_InitTypeDef ADC_InitStructure;
  ADC_MultiModeTypeDef ADC_MultiModeStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable ADC3 clock */
	__ADC3_CLK_ENABLE();

  /* Configure ADC Channel 12 as analog input */
  GPIO_InitStructure.Pin = GPIO_PIN_2;
  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* ADC Common Init */
  ADC_MultiModeStructure.Mode = ADC_MODE_INDEPENDENT;
  ADC_MultiModeStructure.DMAAccessMode = ADC_DMAACCESSMODE_DISABLED;
  ADC_MultiModeStructure.TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_5CYCLES; 
	
	HAL_ADCEx_MultiModeConfigChannel(&ADC_HandleStructure, &ADC_MultiModeStructure);

  /* ADC3 Configuration ------------------------------------------------------*/
	ADC_InitStructure.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV6;
  ADC_InitStructure.Resolution = ADC_RESOLUTION_12B;
  ADC_InitStructure.ScanConvMode = DISABLE;
  ADC_InitStructure.ContinuousConvMode = ENABLE;
  ADC_InitStructure.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE; 
  ADC_InitStructure.DataAlign = ADC_DATAALIGN_RIGHT;
  ADC_InitStructure.NbrOfConversion = 1;
	
	ADC_HandleStructure.Instance = ADC3;
	ADC_HandleStructure.Init = ADC_InitStructure;
  HAL_ADC_Init(&ADC_HandleStructure);

	ADC_ChannelConfTypeDef ADC_ChannelConfStructure;
	// TODO: ADC_ChannelConfStructure.Rank = 1;
	ADC_ChannelConfStructure.SamplingTime = ADC_SAMPLETIME_56CYCLES;
	ADC_ChannelConfStructure.Channel = ADC_CHANNEL_12;
	
  /* ADC3 Regular Channel Config */
	HAL_ADC_ConfigChannel(&ADC_HandleStructure, &ADC_ChannelConfStructure);

//TODO: 
//  /* Enable ADC3 */
//  ADC_Cmd(ADC3, ENABLE);

//  /* ADC3 regular Software Start Conv */ 
//  ADC_SoftwareStartConv(ADC3);
}

/**
  * @brief  ADC_Handler : SSI handler for ADC page 
  */
u16_t ADC_Handler(int iIndex, char *pcInsert, int iInsertLen)
{
  /* We have only one SSI handler iIndex = 0 */
  if (iIndex ==0)
  {  
    char Digit1=0, Digit2=0, Digit3=0, Digit4=0; 
    uint32_t ADCVal = 0;        

     /* configure ADC if not yet configured */
     if (ADC_not_configured ==1)       
     {
        ADC_Configuration();
        ADC_not_configured=0;
     }
     
		 ADC_HandleTypeDef ADC_HandleStructure;
		 ADC_HandleStructure.Instance = ADC3;
     /* get ADC conversion value */
     ADCVal = HAL_ADC_GetValue(&ADC_HandleStructure);
     
     /* convert to Voltage,  step = 0.8 mV */
     ADCVal = (uint32_t)(ADCVal * 0.8);  
     
     /* get digits to display */
     
     Digit1= ADCVal/1000;
     Digit2= (ADCVal-(Digit1*1000))/100 ;
     Digit3= (ADCVal-((Digit1*1000)+(Digit2*100)))/10;
     Digit4= ADCVal -((Digit1*1000)+(Digit2*100)+ (Digit3*10));
        
     /* prepare data to be inserted in html */
     *pcInsert       = (char)(Digit1+0x30);
     *(pcInsert + 1) = (char)(Digit2+0x30);
     *(pcInsert + 2) = (char)(Digit3+0x30);
     *(pcInsert + 3) = (char)(Digit4+0x30);
    
    /* 4 characters need to be inserted in html*/
    return 4;
  } else if (iIndex == 1) { // Test tag "u" value
	    /* prepare data to be inserted in html */
	     *pcInsert       = (char)('1');
	     *(pcInsert + 1) = (char)('2');
	     *(pcInsert + 2) = (char)('3');
	     *(pcInsert + 3) = (char)('4');

	    /* 4 characters need to be inserted in html*/
	    return 4;
  }
  return 0;
}

/**
  * @brief  CGI handler for LEDs control 
  */
const char * LEDS_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
  uint32_t i=0;
  
  /* We have only one SSI handler iIndex = 0 */
  if (iIndex==0)
  {
    /* All leds off */
    STM_EVAL_LEDOff(LED5);
    STM_EVAL_LEDOff(LED6);
    STM_EVAL_LEDOff(LED3);
    STM_EVAL_LEDOff(LED4);
    
    /* Check cgi parameter : example GET /leds.cgi?led=2&led=4 */
    for (i=0; i<iNumParams; i++)
    {
      /* check parameter "led" */
      if (strcmp(pcParam[i] , "led")==0)   
      {
        /* switch led1 ON if 1 */
        if(strcmp(pcValue[i], "1") ==0) 
          STM_EVAL_LEDOn(LED3);
          
        /* switch led2 ON if 2 */
        else if(strcmp(pcValue[i], "2") ==0) 
          STM_EVAL_LEDOn(LED5);
        
        /* switch led3 ON if 3 */
        else if(strcmp(pcValue[i], "3") ==0) 
          STM_EVAL_LEDOn(LED6);
        
        /* switch led4 ON if 4 */
        else if(strcmp(pcValue[i], "4") ==0) 
          STM_EVAL_LEDOn(LED4);
      }
    }
  }
  /* uri to send after cgi call*/
  return "/STM32F4x7LED.html";  
}

void httpd_ssi_init(void)
{  
  /* configure SSI handlers (ADC page SSI) */
  http_set_ssi_handler(ADC_Handler, (char const **)TAGS, 2);
}

/**
 * Initialize CGI handlers
 */
void httpd_cgi_init(void)
{ 
  /* configure CGI handlers (LEDs control CGI) */
  CGI_TAB[0] = LEDS_CGI;
  http_set_cgi_handlers(CGI_TAB, 1);
}

