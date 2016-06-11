#include "stm32f10x_lib.h"
#include"RCC_Configuration.h"

ErrorStatus HSEStartUpStatus;

void RCC_Configuration(void)
{   
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();	//ϵͳʱ�Ӹ�λ

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);   //���ⲿʱ��

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();	 //����ⲿʱ���Ƿ��ȶ�

  if(HSEStartUpStatus == SUCCESS)			 //����ɹ�
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);  //������ر�Ԥȡ������

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);	//���ô�����ʱֵ
 	
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
   RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }
    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
}



