/**
  ******************************************************************************
  * @File    Src/main.c 
  * @author  Craig
  * @brief   Main program body
  ******************************************************************************

  * @brief  Main program
  * @param  None
  * @retval None
  * a simple interrupt driven button program.  The ISR toggles the LED
  */

#include "main.h"
#include <stdio.h>

//  pins and clock for the LED

#define LED2_PIN                         GPIO_PIN_14
#define LED2_GPIO_PORT                   GPIOB
#define LED2_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED2_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()

//declaration for the GPIO pins for the wakeup Button

#define BLUE_BUTTON_PIN                   GPIO_PIN_13
#define BLUE_BUTTON_GPIO_PORT             GPIOC
#define BLUE_BUTTON_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define BLUE_BUTTON_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOC_CLK_DISABLE()

// pins and clocks for USART

#define DISCOVERY_COM1                          USART1
#define DISCOVERY_COM1_CLK_ENABLE()             __HAL_RCC_USART1_CLK_ENABLE()
#define DISCOVERY_COM1_CLK_DISABLE()            __HAL_RCC_USART1_CLK_DISABLE()

#define DISCOVERY_COM1_TX_PIN                   GPIO_PIN_6
#define DISCOVERY_COM1_TX_GPIO_PORT             GPIOB
#define DISCOVERY_COM1_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()   
#define DISCOVERY_COM1_TX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOB_CLK_DISABLE()  
#define DISCOVERY_COM1_TX_AF                    GPIO_AF7_USART1

#define DISCOVERY_COM1_RX_PIN                   GPIO_PIN_7
#define DISCOVERY_COM1_RX_GPIO_PORT             GPIOB
#define DISCOVERY_COM1_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()   
#define DISCOVERY_COM1_RX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOB_CLK_DISABLE()  
#define DISCOVERY_COM1_RX_AF                    GPIO_AF7_USART1

UART_HandleTypeDef hDiscoUart;
EXTI_HandleTypeDef hexti;
EXTI_ConfigTypeDef pEXTiConfig;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
void LED2_Init(void);
void BSP_COM_Init( UART_HandleTypeDef *);
void LED2_Toggle(void);
int __io_putchar(int);
HAL_StatusTypeDef Blue_PB_EXT_Init(void);

int main(void)
{
  
/* STM32L4xx HAL library initialization:
       - Configure the Flash prefetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user 
             can eventually implement his proper time base source (a general purpose 
             timer for example or other time source), keeping in mind that Time base 
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
             handled in milliseconds basis.
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the System clock to have a frequency of 80 MHz */
  SystemClock_Config();

  
    /* Initialize all configured peripherals */
    hDiscoUart.Instance = DISCOVERY_COM1;
    hDiscoUart.Init.BaudRate = 115200;
    hDiscoUart.Init.WordLength = UART_WORDLENGTH_8B;
    hDiscoUart.Init.StopBits = UART_STOPBITS_1;
    hDiscoUart.Init.Parity = UART_PARITY_NONE;
    hDiscoUart.Init.Mode = UART_MODE_TX_RX;
    hDiscoUart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hDiscoUart.Init.OverSampling = UART_OVERSAMPLING_16;
    hDiscoUart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    hDiscoUart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    BSP_COM_Init(&hDiscoUart);

  /* Configure the User LED */
  LED2_Init();
  printf("Welcome to button interrupts !\n");
  if ( Blue_PB_EXT_Init() == HAL_ERROR )
    printf("set up button interrupt failed\n");
  else
      for ( ; ;)
	{
	}
  printf("Bye Bye \n");
}

static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* MSI is enabled after System reset, activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLP = 7;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}

/*
Inititalise the LED2 GPIO port
*/

void LED2_Init(void)
{

   GPIO_InitTypeDef  gpio_init_structure;
  
  LED2_GPIO_CLK_ENABLE();
  /* Configure the GPIO_LED pin */
  gpio_init_structure.Pin   = LED2_PIN;
  gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull  = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
  
  HAL_GPIO_Init(LED2_GPIO_PORT, &gpio_init_structure);
}

/*
 initialise the COM port
*/

void BSP_COM_Init(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef gpio_init_structure;

  /* Enable GPIO RX/TX clocks */
  DISCOVERY_COM1_TX_GPIO_CLK_ENABLE();
  DISCOVERY_COM1_RX_GPIO_CLK_ENABLE();

  /* Enable USART clock */
  DISCOVERY_COM1_CLK_ENABLE();

  /* Configure USART Tx as alternate function */
  gpio_init_structure.Pin = DISCOVERY_COM1_TX_PIN;
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Alternate = DISCOVERY_COM1_TX_AF;
  HAL_GPIO_Init(DISCOVERY_COM1_TX_GPIO_PORT, &gpio_init_structure);

  /* Configure USART Rx as alternate function */
  gpio_init_structure.Pin = DISCOVERY_COM1_RX_PIN;
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Alternate = DISCOVERY_COM1_RX_AF;
  HAL_GPIO_Init(DISCOVERY_COM1_RX_GPIO_PORT, &gpio_init_structure);

  /* USART configuration */
  huart->Instance = DISCOVERY_COM1;
  HAL_UART_Init(huart);
}
/* set up blue button ISR */
HAL_StatusTypeDef Blue_PB_EXT_Init()
{
  GPIO_InitTypeDef gpio_init_structure;
  
  /* Enable the BUTTON clock */
  BLUE_BUTTON_GPIO_CLK_ENABLE();
  __HAL_RCC_SYSCFG_CLK_ENABLE();
 
  /* Configure Button pin as input */
  gpio_init_structure.Pin = BLUE_BUTTON_PIN;
  gpio_init_structure.Mode = GPIO_MODE_INPUT;
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(BLUE_BUTTON_GPIO_PORT, &gpio_init_structure);
  
  hexti.Line = EXTI_LINE_13;

  pEXTiConfig.Line = EXTI_LINE_13;
  pEXTiConfig.Mode = EXTI_MODE_INTERRUPT;
  pEXTiConfig.Trigger = EXTI_TRIGGER_RISING;
  pEXTiConfig.GPIOSel = EXTI_GPIOC;
  /* Configure interrupts for button */ 
  if ( HAL_EXTI_SetConfigLine(&hexti, &pEXTiConfig) == HAL_ERROR)
    return HAL_ERROR;
  //set ISR priority
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0U);
  //enable IRQ
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  return HAL_OK;
}


//EXTI15_10_IRQHandler ISR
void  EXTI15_10_IRQHandler() 

{ 
    LED2_Toggle();
    //clean up 
    HAL_EXTI_IRQHandler(&hexti);

} 

void LED2_Toggle(void)
{
  HAL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_PIN);
}

int __io_putchar(int ch)
{
  /* write a character to the serial port and Loop until the end of transmission */
  while (HAL_OK != HAL_UART_Transmit(&hDiscoUart, (uint8_t *) &ch, 1, 30000))
  {
    ;
  }
  return ch;
}
