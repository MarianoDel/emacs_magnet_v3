//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### HARD.H #################################
//---------------------------------------------

#ifndef HARD_H_
#define HARD_H_


//----------- Defines For Configuration -------------

//-------- Type of Board and Program  ---------------
// #define MAGNET_INFINITY    // with sync and tamper
// #define MAGNET_GAUSSTEK    // basic board
#define MAGNET_GAUSSTEK_MT250    // small 2ch less power board

//----- Board Configuration -------------------//
//--- Hardware ------------------//
// #define HARDWARE_VERSION_3_0    // this board gets the better of stretcher and magnet
#define HARDWARE_VERSION_1_0    // small 2ch less power board


//--- Software ------------------//
#define SOFTWARE_VERSION_1_1    // fixt 50% on square signal, fast up on antennas
// #define SOFTWARE_VERSION_1_0    // init version


//--- Serial Number / Device Id Bytes length ----------
#define USE_DEVICE_ID_4BYTES
// #define USE_DEVICE_ID_12BYTES



//-------- Type of Program and Features ----------------
#define USE_NO_TREATMENT_DETECT    //cuando esta en tratamiento revisa si las potencias tambien
#define USE_BUZZER_ON_START


//-------- Kind of Reports Sended ----------------

//-------- Others Configurations depending on the formers ------------
#ifdef HARDWARE_VERSION_3_0
// #define K_200V    0.0957    //con diodo z prot de 3.1V da error
#define K_200V    0.0806    //con diodo z 5.1V
// #define K_15V    0.00804    //con z 3.1V
#define K_15V    0.00619    //con z 5.1V
#endif

#ifdef HARDWARE_VERSION_1_0
#define K_200V    0.0396    //con diodo z 5.1V
#define K_15V    0.00619    //con z 5.1V
#endif



//-------- Oscillator and Crystal selection (Freq in startup_clocks.h) ---
#ifdef MAGNET_GAUSSTEK_MT250
#define HSI_INTERNAL_RC
// #define HSE_CRYSTAL_OSC

#ifdef HSE_CRYSTAL_OSC
// #define CRYSTAL_8MHZ
#define CRYSTAL_12MHZ
#endif

// #define SYSCLK_FREQ_72MHz
#define SYSCLK_FREQ_64MHz
// #define SYSCLK_FREQ_8MHz
#endif

#if (defined MAGNET_GAUSSTEK) ||\
    (defined MAGNET_INFINITY)
// #define HSI_INTERNAL_RC
#define HSE_CRYSTAL_OSC

#ifdef HSE_CRYSTAL_OSC
// #define CRYSTAL_8MHZ
#define CRYSTAL_12MHZ
#endif

#define SYSCLK_FREQ_72MHz
// #define SYSCLK_FREQ_64MHz
// #define SYSCLK_FREQ_8MHz
#endif



//-------- End Of Defines For Configuration ------


//--- Hardware & Software Messages ------------------//
#ifdef HARDWARE_VERSION_3_0
#define HARD "Hardware Version: 3.0"
#endif
#ifdef HARDWARE_VERSION_1_0
#define HARD "Hardware Version: 1.0"
#endif
#ifdef SOFTWARE_VERSION_1_1
#define SOFT "Software Version: 1.1"
#endif
#ifdef SOFTWARE_VERSION_1_0
#define SOFT "Software Version: 1.0"
#endif
//--- End of Hardware & Software Messages ------------------//

//-------- Sanity Checks -------------------------
#if ((!defined MAGNET_INFINITY) && \
     (!defined MAGNET_GAUSSTEK) && \
     (!defined MAGNET_GAUSSTEK_MT250))
#error "Must define type of board in hard.h"
#endif

#if ((defined MAGNET_INFINITY) || \
     (defined MAGNET_GAUSSTEK))
#ifndef HARDWARE_VERSION_3_0
#error "For this config min hardware is 3.0"
#endif
#endif

#if (defined MAGNET_GAUSSTEK_MT250)
#ifndef HARDWARE_VERSION_1_0
#error "For this config hardware must be 1.0"
#endif
#endif



//-------- End of Sanity Checks ------------------

// Exported Types --------------------------------------------------------------
enum resultados
{
	NO_INIT = 0,
	TRABAJANDO,
	FIN_OK,
	FIN_ERROR,
	FIN_TIMEOUT
};

enum bool
{
	FALSE = 0,
	TRUE = !FALSE
};


//--- Gpios Config -------------------------------
#ifdef HARDWARE_VERSION_3_0

// PA defines ----
// PA0 PA1 Alternative TIM5_CH1 TIM5_CH2

// PA2 PA3 Alternative Usart2 Tx Rx

// PA4 Input
#define PROT_CH4    ((GPIOA->IDR & 0x0010) != 0)

// PA5 Analog Channel 5 (IS_CH4)
// PA6 Analog Channel 6 (IS_CH3)

// PA7 NC

// PA8 
#define BUZZER    ((GPIOA->ODR & 0x0100) != 0)
#define BUZZER_ON    (GPIOA->BSRR = 0x00000100)
#define BUZZER_OFF    (GPIOA->BSRR = 0x01000000)

// PA9 PA10 Alternative Usart1 Tx Rx

// PA11 Alternative TIM1_CH4 for SYNC_IN

// PA12 PA13 PA14 PA15 NC

// PB defines ----
// PB0 Analog Channel 8 (IS_CH2)
// PB1 Analog Channel 9 (IS_CH1)

// PB2 Input
#define PROT_CH3    ((GPIOB->IDR & 0x0004) != 0)

// PB3 PB4 PB5 PB6 NC

// PB7 PB8 Alternative TIM4_CH2 TIM4_CH3

// PB9 NC

// PB10 PB11 Alternative Usart3 Tx Rx

// PB12 NC

// PB13 Input
#define PROT_CH2    ((GPIOB->IDR & 0x2000) != 0)

// PB14 if output, check tamper_funcs module
#ifdef MAGNET_INFINITY
#define PB14    ((GPIOB->ODR & 0x4000) != 0)
#define PB14_ON    (GPIOB->BSRR = 0x00004000)
#define PB14_OFF    (GPIOB->BSRR = 0x40000000)
#endif

// PB15 Input
#define PROT_CH1    ((GPIOB->IDR & 0x8000) != 0)

// PC defines ----
// PC0 
#define LED1    ((GPIOC->ODR & 0x0001) != 0)
#define LED1_ON    (GPIOC->BSRR = 0x00000001)
#define LED1_OFF    (GPIOC->BSRR = 0x00010000)

// PC1
#define LED2    ((GPIOC->ODR & 0x0002) != 0)
#define LED2_ON    (GPIOC->BSRR = 0x00000002)
#define LED2_OFF    (GPIOC->BSRR = 0x00020000)

// PC2 NC

// PC3 Analog Channel 13 (Sense_12V)
// PC4 Analog Channel 14 (Sense_200V)
// PC5 Analog Channel 15 (Sense_15V)

// PC6 PC7 PC8 PC9 Alternative TIM8_CH1 TIM8_CH2 TIM8_CH3 TIM8_CH4

// PC10 PC11 Alternative Uart4 Tx Rx

// PC12 PD2 Alternative Uart5 Tx Rx

// PC13 Input
#ifdef MAGNET_INFINITY
#define TAMPER_PIN    ((GPIOC->IDR & 0x2000) != 0)
#endif

#endif //HARDWARE_VERSION_3_0

#ifdef HARDWARE_VERSION_1_0    // must be MT250
// PA defines ----
// PA0 PA1 Alternative TIM5_CH1 TIM5_CH2

// PA2 PA3 Alternative Usart2 Tx Rx

// PA4 Input
#define PROT_CH2    ((GPIOA->IDR & 0x0010) != 0)

// PA5 Analog Channel 5 (IS_CH2)
// PA6 Analog Channel 6 (IS_CH1)

// PA7 NC

// PA8 
#define BUZZER    ((GPIOA->ODR & 0x0100) != 0)
#define BUZZER_ON    (GPIOA->BSRR = 0x00000100)
#define BUZZER_OFF    (GPIOA->BSRR = 0x01000000)

// PA9 PA10 Alternative Usart1 Tx Rx

// PA11 Alternative TIM1_CH4 for SYNC_IN

// PA12 PA13 PA14 PA15 NC

// PB defines ----
// PB0 
// PB1 NC

// PB2 Input
#define PROT_CH1    ((GPIOB->IDR & 0x0004) != 0)

// PB3 PB4 PB5 PB6 NC

// PB7 PB8 Alternative TIM4_CH2 TIM4_CH3

// PB9 NC

// PB10 PB11 Alternative Usart3 Tx Rx

// PB12 PB14 PB15 NC

// PC defines ----
// PC0 
#define LED1    ((GPIOC->ODR & 0x0001) != 0)
#define LED1_ON    (GPIOC->BSRR = 0x00000001)
#define LED1_OFF    (GPIOC->BSRR = 0x00010000)

// PC1
#define LED2    ((GPIOC->ODR & 0x0002) != 0)
#define LED2_ON    (GPIOC->BSRR = 0x00000002)
#define LED2_OFF    (GPIOC->BSRR = 0x00020000)

// PC2 NC

// PC3 Analog Channel 13 (Sense_12V)
// PC4 Analog Channel 14 (Sense_100V)
// PC5 Analog Channel 15 (Sense_15V)

// PC6 PC7 PC8 PC9 NC

// PC10 PC11 NC

// PC12 PD2 NC

// PC13 PC14 PC15 NC

// channels protection for backward compatibility
#define PROT_CH3    (1 == 0)
#define PROT_CH4    (1 == 0)
#endif //HARDWARE_VERSION_1_0 (must be MT250)

//--- End of Gpios Config ------------------------

//ESTADOS DEL BUZZER
typedef enum
{    
    BUZZER_INIT = 0,
    BUZZER_TO_STOP,

    BUZZER_MULTIPLE_LONG,
    BUZZER_MULTIPLE_LONGA,
    BUZZER_MULTIPLE_LONGB,

    BUZZER_MULTIPLE_HALF,
    BUZZER_MULTIPLE_HALFA,
    BUZZER_MULTIPLE_HALFB,

    BUZZER_MULTIPLE_SHORT,
    BUZZER_MULTIPLE_SHORTA,
    BUZZER_MULTIPLE_SHORTB
    
} buzzer_state_t;

//COMANDOS DEL BUZZER	(tienen que ser los del estado de arriba)
#define BUZZER_STOP_CMD		BUZZER_TO_STOP
#define BUZZER_LONG_CMD		BUZZER_MULTIPLE_LONG
#define BUZZER_HALF_CMD		BUZZER_MULTIPLE_HALF
#define BUZZER_SHORT_CMD	BUZZER_MULTIPLE_SHORT

// tiempos de light treatment
// #define TIM_BIP_SHORT		50
// #define TIM_BIP_SHORT_WAIT	100
// #define TIM_BIP_HALF		200
// #define TIM_BIP_HALF_WAIT	500
// #define TIM_BIP_LONG		1200
// #define TIM_BIP_LONG_WAIT	1500

// tiempos de magneto
#define TIM_BIP_SHORT		300
#define TIM_BIP_SHORT_WAIT	500
#define TIM_BIP_HALF		600
#define TIM_BIP_HALF_WAIT	800
#define TIM_BIP_LONG		2000
#define TIM_BIP_LONG_WAIT	2000

//--- Tiempos (TIMEOUT) de salidas
#define TIMER_OUT4_IN_ON       100

//--- ESTADOS DEL LED -----------
typedef enum
{    
    START_BLINKING = 0,
    WAIT_TO_OFF,
    WAIT_TO_ON,
    WAIT_NEW_CYCLE
} led_state_t;

//Estados Externos de LED BLINKING
#define LED_NO_BLINKING    0     
#define LED_TREATMENT_STANDBY    1
#define LED_TREATMENT_GENERATING    2
#define LED_TREATMENT_PAUSED    3
#define LED_TREATMENT_BRIDGE_MODE    5



//--- Exported Module Functions ----
void ChangeLed (unsigned char);
void UpdateLed (void);
void UpdateBuzzer (void);
void BuzzerCommands(unsigned char , unsigned char );
void HARD_Timeouts (void);
void HARD_L1_ON (void);
void HARD_L1_OFF (void);

unsigned char Led1_Is_On (void);
void Led1_On (void);
void Led1_Off (void);

void Pb14_To_Output (void);
void Pb14_Off (void);
void Pb14_On (void);
void Pb14_To_Input (void);
unsigned char Tamper_Pin (void);

#endif
