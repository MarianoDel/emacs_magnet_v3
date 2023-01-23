//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### GPIO.C ################################
//---------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "gpio.h"
#include "hard.h"


/* Externals ------------------------------------------------------------------*/


/* Globals ------------------------------------------------------------------*/


/* Module Functions -----------------------------------------------------------*/


//--- Tamper config ---//
void Tamper_Config(void)
{
	unsigned long temp;

	//--- GPIOB ---//
	//--- Clock ---//
	if (!(RCC->APB2ENR & 0x00000008))
		RCC->APB2ENR |= 0x00000008;

	//--- Config pines ---//
	temp = GPIOB->CRH;
	temp &= 0xFF0FFFFF;
	temp |= 0x00800000;
	GPIOB->CRH = temp;
	GPIOB->BSRR = 0x00002000;
}





//------- GPIO REGISTERS ----------//
//
//	GPIOx->CRL	pin 7 - 0
//	3  2  1  0
//	CNF0  MODE0
//	GPIOx->CRH	pin 15 - 8
//
//      En hexa me queda cada nibble es igual a la configuracion de ese pin
//      nibble0 configura completo pin0
//
//	MODEx 00 Input (reset)
//	MODEx 01 Output 10MHz
//	MODEx 10 Output 2MHz
//	MODEx 11 Output 50MHz
//
//	Input Mode
//      las funciones alternativas de los pines estan directamente conectadad al
//      periferico en el modo input
//      CNFx 00 Analog
//      CNFx 01 Floating (reset)
//      CNFx 10 Input (pull up / dwn)
//      CNFx 11 Reserved
//
//	Output Mode
//      CNFx 00 Push Pull
//      CNFx 01 Open Drain
//      CNFx 10 Alternate func Push Pull
//      CNFx 11 Alternate func Open Drain
//
//      Pull-Up Pull-Dwn si el pin es Input el registro ODR coloca pull-up (1) o pull-dwn (0)
//      GPIOx->ODR 0xNNNN, 1 bit por pin
//

//-- GPIO Configuration --------------------
void GpioInit (void)
{
    unsigned long temp;

    //--- GPIO Clocks ---//
    if (!RCC_GPIOA_CLK)
        RCC_GPIOA_CLKEN;

    if (!RCC_GPIOB_CLK)
        RCC_GPIOB_CLKEN;

    if (!RCC_GPIOC_CLK)
        RCC_GPIOC_CLKEN;

    if (!RCC_GPIOD_CLK)
        RCC_GPIOD_CLKEN;

    //--- GPIOA Low Side ------------------//
    //PA0 NC
    //PA1 NC
    //PA2 alternative Tx Usart2
    //PA3 alternative Rx Usart2
    //PA4 NC
    //PA5 NC
    //PA6 NC
    //PA7 NC
    temp = GPIOA->CRL;
    temp &= 0xFFFF00FF;
    temp |= 0x00008A00;
    GPIOA->CRL = temp;

    //--- GPIOA High Side ------------------//
    //PA8 IN0
    //PA9 alternative Tx Usart1
    //PA10 alternative Rx Usart1
    //PA11 NC
    //PA12 NC
    //PA13 NC
    //PA14 NC
    //PA15 NC
    temp = GPIOA->CRH;
    temp &= 0xFFFFF000;
    temp |= 0x000008A8;
    GPIOA->CRH = temp;

    //--- GPIOA Pull-Up Pull-Dwn ------------------//
    temp = GPIOA->ODR;    //PA3 PA8 pull-up
    temp &= 0xFEF7;
    temp |= 0x0108;
    GPIOA->ODR = temp;

    //--- GPIOB Low Side -------------------//
    //PB0 NC
    //PB1 NC
    //PB2 NC
    //PB3 NC
    //PB4 NC
    //PB5 SW_RX_TX    RS485
    //PB6 
    //PB7 
    temp = GPIOB->CRL;
    temp &= 0xFF0FFFFF;
    temp |= 0x00200000;
    GPIOB->CRL = temp;

    //--- GPIOB High Side -------------------//
    //PB8 NC
    //PB9 NC
    //PB10 alternative Tx Usart3
    //PB11 alternative Rx Usart3
    //PB12 NC
    //PB13 OUT5
    //PB14 OUT4
    //PB15 OUT1  
    temp = GPIOB->CRH;
    temp &= 0x000F00FF;
    temp |= 0x22208A00;
    GPIOB->CRH = temp;    
    
    //--- GPIOC Low Side -------------------//
    //PC0 LED1
    //PC1 LED2
    //PC2 NC
    //PC3 NC
    //PC4 Sense 200V ADC Channel 14
    //PC5 Sense 15V ADC Channel 15
    //PC6 OUT2
    //PC7 OUT3
    temp = GPIOC->CRL;
    temp &= 0x0000FF00;
    temp |= 0x22000022;
    GPIOC->CRL = temp;

    //--- GPIOC High Side -------------------//
    //PC8 IN2
    //PC9 IN1
    //PC10 alternative Tx Uart4
    //PC11 alternativo Rx Uart4
    //PC12 alternativo Tx Uart5
    //PC13 NC
    //PC14 NC    oscillator
    //PC15 NC    oscillator
    temp = GPIOC->CRH;   
    temp &= 0xFFF00000;
    temp |= 0x000A8A88;
    GPIOC->CRH = temp;

    //--- GPIOD Low Side -------------------//
    //PD0 NC
    //PD1 NC
    //PD2 alternative Rx Uart5
    //PD3 No implemented
    //PD4 No implemented
    //PD5 No implemented
    //PD6 No implemented
    //PD7 No implemented    
    temp = GPIOD->CRL;   
    temp &= 0xFFFFF0FF;    
    temp |= 0x00000A00;
    GPIOD->CRL = temp;

}
