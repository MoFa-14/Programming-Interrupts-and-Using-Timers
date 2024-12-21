/**
  ******************************************************************************
  * @File    Src/button.c 
  * @author  Craig
  * @brief   Main program body with button ISR toggling two LEDs
  ******************************************************************************/

#include "main.h"
#include <stdio.h>

//  Pins and clocks for LED2
#define LED2_PIN                         GPIO_PIN_14
#define LED2_GPIO_PORT                   GPIOB
#define LED2_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()

// Pins and clocks for LED1
#define LED1_PIN  GPIO_PIN_5  
#define LED1_GPIO_PORT                   GPIOA
#define LED1_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()

// Declaration for GPIO pins for the button
#define BLUE_BUTTON_PIN                  GPIO_PIN_13
#define BLUE_BUTTON_GPIO_PORT            GPIOC
#define BLUE_BUTTON_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOC_CLK_ENABLE()

UART_HandleTypeDef hDiscoUart;
EXTI_HandleTypeDef hexti;
EXTI_ConfigTypeDef pEXTiConfig;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
void LED2_Init(void);
void LED1_Init(void);
void BSP_COM_Init(UART_HandleTypeDef *);
void LED2_On(void);
void LED2_Off(void);
void LED1_On(void);
void LED1_Off(void);
int __io_putchar(int);
HAL_StatusTypeDef Blue_PB_EXT_Init(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    /* Initialize LEDs */
    LED2_Init();
    LED1_Init();
    
    

    /* Initialize UART for minicom communication */
    hDiscoUart.Instance = USART1;
    hDiscoUart.Init.BaudRate = 115200;
    hDiscoUart.Init.WordLength = UART_WORDLENGTH_8B;
    hDiscoUart.Init.StopBits = UART_STOPBITS_1;
    hDiscoUart.Init.Parity = UART_PARITY_NONE;
    hDiscoUart.Init.Mode = UART_MODE_TX_RX;
    hDiscoUart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hDiscoUart.Init.OverSampling = UART_OVERSAMPLING_16;
    BSP_COM_Init(&hDiscoUart);

    printf("Welcome to Button Interrupts - Toggle LEDs!\n");

    /* Initialize button interrupt */
    if (Blue_PB_EXT_Init() == HAL_ERROR)
        printf("Button interrupt setup failed!\n");

    while (1)
    {
        // Main loop does nothing - LEDs are toggled in the ISR
    }
}

/* Initialize LED2 */
void LED2_Init(void)
{
    GPIO_InitTypeDef gpio_init_structure;

    LED2_GPIO_CLK_ENABLE();
    gpio_init_structure.Pin = LED2_PIN;
    gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(LED2_GPIO_PORT, &gpio_init_structure);
    LED2_Off(); // Initialize LED2 to OFF
}

/* Initialize LED1 */
void LED1_Init(void)
{
    GPIO_InitTypeDef gpio_init_structure;

    LED1_GPIO_CLK_ENABLE();
    gpio_init_structure.Pin = LED1_PIN;
    gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(LED1_GPIO_PORT, &gpio_init_structure);
    LED1_Off(); // Initialize LED1 to OFF
}

/* Set up button ISR */
HAL_StatusTypeDef Blue_PB_EXT_Init()
{
    GPIO_InitTypeDef gpio_init_structure;

    BLUE_BUTTON_GPIO_CLK_ENABLE();
    __HAL_RCC_SYSCFG_CLK_ENABLE();

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

    if (HAL_EXTI_SetConfigLine(&hexti, &pEXTiConfig) == HAL_ERROR)
        return HAL_ERROR;

    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0U);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

    return HAL_OK;
}

/* Button ISR: Alternate LED1 and LED2 */
void EXTI15_10_IRQHandler()
{
    static uint8_t led_state = 0; // State variable to alternate LEDs

    if (led_state == 0)
    {
        HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_SET);   // Turn LED1 ON
        HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, GPIO_PIN_RESET); // Turn LED2 OFF
        printf("LED1 ON, LED2 OFF\n");
    }
    else
    {
        HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_RESET); // Turn LED1 OFF
        HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, GPIO_PIN_SET);   // Turn LED2 ON
        printf("LED1 OFF, LED2 ON\n");
    }

    led_state = !led_state; // Toggle the state for the next press

    HAL_EXTI_IRQHandler(&hexti); // Clean up the interrupt
}


/* Helper functions to turn LEDs ON/OFF */
void LED2_On(void) { HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, GPIO_PIN_SET); }
void LED2_Off(void) { HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, GPIO_PIN_RESET); }
void LED1_On(void) { HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_SET); }
void LED1_Off(void) { HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_RESET); }

/* USART Initialization */
void BSP_COM_Init(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init_structure;

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();

    gpio_init_structure.Pin = GPIO_PIN_6;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_structure.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOB, &gpio_init_structure);

    gpio_init_structure.Pin = GPIO_PIN_7;
    HAL_GPIO_Init(GPIOB, &gpio_init_structure);

    HAL_UART_Init(huart);
}

int __io_putchar(int ch)
{
    while (HAL_OK != HAL_UART_Transmit(&hDiscoUart, (uint8_t *)&ch, 1, 30000))
    {
        ;
    }
    return ch;
}
/* System Clock Configuration */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Configure the main internal regulator output voltage */
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Initializes the RCC Oscillators according to the specified parameters */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6; // 4 MHz
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 40;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV4;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        while (1); // Error handler
    }

    /* Initializes the CPU, AHB, and APB buses clocks */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        while (1); // Error handler
    }
}
