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

//----- Board Configuration -------------------//
//--- Hardware ------------------//
#define HARDWARE_VERSION_2_0
// #define HARDWARE_VERSION_1_0        //esto seria una placa P1 en realidad
// #define HARDWARE_VERSION_2_1        //esto seria una placa magneto chico
//--- Software ------------------//
// #define SOFTWARE_VERSION_1_0        //habla contra rpi con programa magneto y traduce a micros potencia
// #define SOFTWARE_VERSION_1_1    //programa ernesto manda pulso en OUT4 para mover camilla 21-10-2020
#define SOFTWARE_VERSION_1_2    //it permits the auto up or auto dwn of the stretcher

//-------- Type of Program (depending on software version) ----------------
#define MAGNETO_NORMAL
// #define MAGNETO_ESPECIAL_1CH

//--- Serial Number / Device Id Bytes length ----------
#define USE_DEVICE_ID_4BYTES
// #define USE_DEVICE_ID_12BYTES




//-------- Type of Program and Features ----------------
#define USE_SYNC_PULSES    //manda sync ver abajo

#define USE_NO_TREATMENT_DETECT    //cuando esta en tratamiento revisa si las potencias tambien

#define USE_BUZZER_ON_OUT3

//-------- Kind of Reports Sended ----------------

//-------- Others Configurations depending on the formers ------------
// #define K_200V    0.0957    //con diodo z prot de 3.1V da error
#define K_200V    0.0806    //con diodo z 5.1V
// #define K_15V    0.00804    //con z 3.1V
#define K_15V    0.00619    //con z 5.1V

#ifdef USE_SYNC_PULSES
#define SYNC_CHAR    '*'
#define USE_SYNC_ALL_PLACES    //manda pulsos incluso en los Wait_ms()
// #define USE_SYNC_ONLY_ON_TREATMENT    //pulsos solo en tratamiento
#endif

#ifdef USE_BUZZER_ON_OUT3
#define USE_ONE_BIP_INIT
#endif
//-------- Hysteresis Conf ------------------------

//-------- PWM Conf ------------------------

//-------- Oscillator and Crystal selection (Freq in startup_clocksh) ---
#define HSI_INTERNAL_RC
// #define HSE_CRYSTAL_OSC


//-------- End Of Defines For Configuration ------




//--- Hardware & Software Messages ------------------//
#ifdef HARDWARE_VERSION_2_0
#define HARD "Hardware Version: 2.0"
#endif
#ifdef HARDWARE_VERSION_2_1
#define HARD "Hardware Version: 2.1"
#endif
#ifdef SOFTWARE_VERSION_2_2
#define SOFT "Software Version: 2.2"
#endif
#ifdef SOFTWARE_VERSION_1_0
#define SOFT "Software Version: 1.0"
#endif
#ifdef SOFTWARE_VERSION_1_1
#define SOFT "Software Version: 1.1"
#endif
#ifdef SOFTWARE_VERSION_1_2
#define SOFT "Software Version: 1.2"
#endif
//--- End of Hardware & Software Messages ------------------//



// Exported Types --------------------------------------------------------------
typedef enum {
	resp_ok = 0,
	resp_not_own,
	resp_error

} resp_t;

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

#ifdef MAGNETO_ESPECIAL_1CH
typedef enum {
    SW_NO = 0,
    SW_MIN,
    SW_HALF,
    SW_FULL
    
} resp_sw_t;
#endif


//--- Configuracion de leds ---//
#ifdef HARDWARE_VERSION_2_0
//--- PC0 ---//
#define LED1    ((GPIOC->ODR & 0x0001) == 0)
#define LED1_ON    (GPIOC->BSRR = 0x00000001)
#define LED1_OFF    (GPIOC->BSRR = 0x00010000)

//--- PC1 ---//
#define LED2    ((GPIOC->ODR & 0x0002) == 0)
#define LED2_ON    (GPIOC->BSRR = 0x00000002)
#define LED2_OFF    (GPIOC->BSRR = 0x00020000)

//PA0, PA1 NC

//PA2
//PA3    Usart2

//PA4, PA5, PA6, PA7  NC

//PC4    Analog Channel 14
//PC5    Analog Channel 15

//PB0, PB1, PB2 NC

//PB10
//PB11    Usart3

//PB12    NC

//--- PB13 ---//
#define OUT5    ((GPIOB->ODR & 0x2000) != 0)
#define OUT5_ON    (GPIOB->BSRR = 0x00002000)
#define OUT5_OFF    (GPIOB->BSRR = 0x20000000)

//--- PB14 ---//
#define OUT4    ((GPIOB->ODR & 0x4000) != 0)
#define OUT4_ON    (GPIOB->BSRR = 0x00004000)
#define OUT4_OFF    (GPIOB->BSRR = 0x40000000)

//--- PB15 ---//
#define OUT1    ((GPIOB->ODR & 0x8000) != 0)
#define OUT1_ON    (GPIOB->BSRR = 0x00008000)
#define OUT1_OFF    (GPIOB->BSRR = 0x80000000)

//--- PC6 ---//
#define OUT2    ((GPIOC->ODR & 0x0040) != 0)
#define OUT2_ON    (GPIOC->BSRR = 0x00000040)
#define OUT2_OFF    (GPIOC->BSRR = 0x00400000)

//--- PC7 ---//
#define OUT3    ((GPIOC->ODR & 0x0080) != 0)
#define OUT3_ON    (GPIOC->BSRR = 0x00000080)
#define OUT3_OFF    (GPIOC->BSRR = 0x00800000)

#ifdef USE_BUZZER_ON_OUT3
#define BUZZER    OUT3
#define BUZZER_ON    OUT3_ON
#define BUZZER_OFF    OUT3_OFF
#endif

//--- PC8 ---//
#define IN2 ((GPIOC->IDR & 0x0100) != 0)

//--- PC9 ---//
#define IN1 ((GPIOC->IDR & 0x0200) != 0)

//--- PA8 ---//
#define IN0 ((GPIOA->IDR & 0x0100) == 0)

//PA9
//PA10    Usart1

//PA11, PA12, PA13, PA14, PA15    NC

//PC10
//PC11    Uart4

//PC12
//PD2    Uart5

//PB3, PB4    NC

//--- PB5 ---//
#define SW_RX_TX    ((GPIOB->ODR & 0x0020) != 0)
#define SW_RX_TX_ON    (GPIOB->BSRR = 0x00000020)
#define SW_RX_TX_OFF    (GPIOB->BSRR = 0x00200000)

//PB6, PB7, PB8, PB9    NC

#endif //HARDWARE_VERSION_2_0


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
void HARD_Timers_Update (void);
void HARD_L1_ON (void);
void HARD_L1_OFF (void);

#ifdef MAGNETO_ESPECIAL_1CH
resp_sw_t CheckS1 (void);
void UpdateSwitches (void);
#endif

#endif
