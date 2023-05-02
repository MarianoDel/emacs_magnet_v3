//------------------------------------------------
// #### PROJECT MAGNET V3 F103 - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### MAIN.C ###################################
//------------------------------------------------

// Includes --------------------------------------------------------------------
#include "stm32f10x.h"
#include "hard.h"

#include "adc.h"
#include "tim.h"
#include "gpio.h"
#include "usart.h"
#include "dma.h"

#include "treatment.h"
#include "comms_from_rasp.h"
#include "comms.h"
#include "test_functions.h"

#include "antennas.h"
#include "comms_channels.h"

#include <stdio.h>
#include <string.h>


// Private Types Constants and Macros ------------------------------------------


// Externals -------------------------------------------------------------------
unsigned short comms_messages_1 = 0;
unsigned short comms_messages_2 = 0;
unsigned short comms_messages_3 = 0;
unsigned short comms_messages_rpi = 0;


char buffSendErr[64];

//--- Externals for keepalives on usarts
#define TIME_RUN_DEF 250
volatile unsigned short timeRun = TIME_RUN_DEF;

//--- Externals de los timers
volatile unsigned short timer_standby = 0;
volatile unsigned short wait_ms_var = 0;
volatile unsigned short comms_timeout = 0;

//--- Externals de los timers
volatile unsigned short adc_ch [ADC_CHANNEL_QUANTITY];


// Globals ---------------------------------------------------------------------
volatile unsigned short timer_sync_xxx_ms = 0;
volatile unsigned short timer_out4 = 0;


// Module Private Functions ----------------------------------------------------
void TimingDelay_Decrement(void);
void SysTickError (void);

#define RPI_Flush_Comms (comms_messages_rpi &= ~COMM_RPI_ALL_MSG_MASK)


// Module Functions ------------------------------------------------------------
int main (void)
{
    // Gpio Configuration.
    GpioInit();
    BUZZER_OFF;
    
    // Systick Timer Activation
    if (SysTick_Config(72000))
        SysTickError();

    // Hardware Tests
    // TF_Hardware_Tests ();

    // --- main program inits. ---
    //-- DMA configuration.
    DMAConfig();
    DMA_ENABLE;
    
    //-- ADC with DMA
    AdcConfig();
    ADC_START;

    //-- Comms with rasp
    Usart1Config ();

    //-- Comms with channels
    Usart2Config ();
    Usart3Config ();
    Uart4Config ();
    Uart5Config ();

    //-- PWM Timers    
    TIM8_Init();    // init timer 8 for ch1 & ch2 pwm output
    TIM4_Init();    // init timer 4 for channel 3 pwm output
    TIM5_Init();    // init timer 5 for channel 4 pwm output

    //-- TIM1 for signals module sequence ready
    TIM1_Init();

    
    //-- Welcome Messages --------------------
    Usart1Send("\r\nInfinity Clinics Magnet Power Board -- powered by: Kirno International Llc\r\n");
    Wait_ms(100);

#ifdef HARD
    Usart1Send(HARD);
    Usart1Send("\r\n");
    Wait_ms(100);    
#else
#error	"No Hardware defined in hard.h file"
#endif

#ifdef SOFT
    Usart1Send(SOFT);
    Usart1Send("\r\n");
    Wait_ms(100);    
#else
#error	"No Soft Version defined in hard.h file"
#endif
    //-- end of Welcome Messages ------------

    ChangeLed(LED_TREATMENT_STANDBY);

    //-- Main Loop --------------------------
    while (1)
    {
        // update the antennas module states
        AntennaUpdateStates ();

        // update the channels comms
        Comms_Channel1 ();
        Comms_Channel2 ();
        Comms_Channel3 ();
        Comms_Channel4 ();

        // update treatment state
        Treatment_Manager();

        // the update of led and buzzer on Treatment_Manager()
        
    }
}

//--- End of Main ---//


// Other Module Functions ------------------------------------------------------
extern void TF_Prot_Int_Handler (unsigned char ch);
void EXTI2_IRQHandler (void)
{
    if(EXTI->PR & EXTI_PR_PR2)    //Line2
    {
        // Overcurrent_Shutdown();
        TF_Prot_Int_Handler (3);    // PROT_CH3
        EXTI->PR |= EXTI_PR_PR2;
    }
}


void EXTI4_IRQHandler (void)
{
    if(EXTI->PR & EXTI_PR_PR4)    //Line4
    {
        // Overcurrent_Shutdown();
        TF_Prot_Int_Handler (4);    // PROT_CH4
        EXTI->PR |= EXTI_PR_PR4;
    }
}


void EXTI15_10_IRQHandler (void)
{
    if(EXTI->PR & EXTI_PR_PR13)    //Line13
    {
        // Overcurrent_Shutdown();
        TF_Prot_Int_Handler (2);    // PROT_CH2        
        EXTI->PR |= EXTI_PR_PR13;
    }
    else if (EXTI->PR & EXTI_PR_PR15)    //Line15
    {
        // Overcurrent_Shutdown();
        TF_Prot_Int_Handler (1);    // PROT_CH1        
        EXTI->PR |= EXTI_PR_PR15;
    }
}


void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    AntennaTimeouts ();

    Treatment_Timeouts ();
    
    HARD_Timeouts();
    
    if (timer_standby)
        timer_standby--;

}


void SysTickError (void)
{
    //Capture systick error...
    while (1)
    {
        if (LED1)
            LED1_OFF;
        else
            LED1_ON;

        for (unsigned char i = 0; i < 255; i++)
        {
            asm ("nop \n\t"
                 "nop \n\t"
                 "nop \n\t" );
        }
    }
}

//--- end of file ---//

