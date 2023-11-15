//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### HARD.C #################################
//---------------------------------------------
#include "hard.h"
#include "stm32f10x.h"


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
// for the LEDs
led_state_t led_state = START_BLINKING;
unsigned char blink = 0;
unsigned char how_many_blinks = 0;
volatile unsigned short timer_led = 0;

// for the buzzer
buzzer_state_t buzzer_state = BUZZER_INIT;
unsigned char buzzer_multiple;
volatile unsigned short buzzer_timeout = 0;


#define LED_ON    LED2_ON
#define LED_OFF    LED2_OFF

// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
// called from int timer on 1ms ticks
void HARD_Timeouts (void)
{
    if (timer_led)
        timer_led--;

    if (buzzer_timeout)
        buzzer_timeout--;

}

//cambia configuracion de bips del LED
void ChangeLed (unsigned char how_many)
{
    how_many_blinks = how_many;
    led_state = START_BLINKING;
}

//mueve el LED segun el estado del Programa
void UpdateLed (void)
{
    switch (led_state)
    {
        case START_BLINKING:
            blink = how_many_blinks;
            
            if (blink)
            {
                LED_ON;
                timer_led = 200;
                led_state++;
                blink--;
            }
            break;

        case WAIT_TO_OFF:
            if (!timer_led)
            {
                LED_OFF;
                timer_led = 200;
                led_state++;
            }
            break;

        case WAIT_TO_ON:
            if (!timer_led)
            {
                if (blink)
                {
                    blink--;
                    timer_led = 200;
                    led_state = WAIT_TO_OFF;
                    LED_ON;
                }
                else
                {
                    led_state = WAIT_NEW_CYCLE;
                    timer_led = 2000;
                }
            }
            break;

        case WAIT_NEW_CYCLE:
            if (!timer_led)
                led_state = START_BLINKING;

            break;

        default:
            led_state = START_BLINKING;
            break;
    }
}


void BuzzerCommands(unsigned char command, unsigned char multiple)
{
    buzzer_state = command;
    buzzer_multiple = multiple;
}


void UpdateBuzzer (void)
{
    switch (buzzer_state)
    {
        case BUZZER_INIT:
            break;

        case BUZZER_MULTIPLE_SHORT:
            if (buzzer_multiple > 0)
            {
                BUZZER_ON;
                buzzer_state++;
                buzzer_timeout = TIM_BIP_SHORT;
            }
            else
                buzzer_state = BUZZER_TO_STOP;
            break;

        case BUZZER_MULTIPLE_SHORTA:
            if (!buzzer_timeout)
            {
                buzzer_state++;
                BUZZER_OFF;
                buzzer_timeout = TIM_BIP_SHORT_WAIT;
            }
            break;

        case BUZZER_MULTIPLE_SHORTB:
            if (!buzzer_timeout)
            {
                if (buzzer_multiple)
                    buzzer_multiple--;

                buzzer_state = BUZZER_MULTIPLE_SHORT;
            }
            break;

        case BUZZER_MULTIPLE_HALF:
            if (buzzer_multiple > 0)
            {
                BUZZER_ON;
                buzzer_state++;
                buzzer_timeout = TIM_BIP_HALF;
            }
            else
                buzzer_state = BUZZER_TO_STOP;
            break;

        case BUZZER_MULTIPLE_HALFA:
            if (!buzzer_timeout)
            {
                buzzer_state++;
                BUZZER_OFF;
                buzzer_timeout = TIM_BIP_HALF_WAIT;
            }
            break;

        case BUZZER_MULTIPLE_HALFB:
            if (!buzzer_timeout)
            {
                if (buzzer_multiple)
                    buzzer_multiple--;

                buzzer_state = BUZZER_MULTIPLE_HALF;
            }
            break;

        case BUZZER_MULTIPLE_LONG:
            if (buzzer_multiple > 0)
            {
                BUZZER_ON;
                buzzer_state++;
                buzzer_timeout = TIM_BIP_LONG;
            }
            else
                buzzer_state = BUZZER_TO_STOP;
            break;

        case BUZZER_MULTIPLE_LONGA:
            if (!buzzer_timeout)
            {
                buzzer_state++;
                BUZZER_OFF;
                buzzer_timeout = TIM_BIP_LONG_WAIT;
            }
            break;

        case BUZZER_MULTIPLE_LONGB:
            if (!buzzer_timeout)
            {
                if (buzzer_multiple)
                    buzzer_multiple--;

                buzzer_state = BUZZER_MULTIPLE_LONG;
            }
            break;

        case BUZZER_TO_STOP:
        default:
            BUZZER_OFF;
            buzzer_state = BUZZER_INIT;
            break;
    }
}

void HARD_L1_ON (void)
{
    LED1_ON;
}


void HARD_L1_OFF (void)
{
    LED1_OFF;
}

unsigned char Led1_Is_On (void)
{
    return LED1;
}


void Led1_On (void)
{
    LED1_ON;
}


void Led1_Off (void)
{
    LED1_OFF;
}


#ifdef MAGNET_INFINITY
unsigned char Tamper_Pin (void)
{
    return TAMPER_PIN;
}


void Pb14_To_Output (void)
{
    unsigned long temp;

    temp = GPIOB->CRH;
    temp &= 0xF0FFFFFF;
    temp |= 0x02000000;
    GPIOB->CRH = temp;
    
}


void Pb14_To_Input (void)
{
    unsigned long temp;

    temp = GPIOB->CRH;
    temp &= 0xF0FFFFFF;
    temp |= 0x04000000;
    GPIOB->CRH = temp;
    
}


void Pb14_Off (void)
{
    PB14_OFF;
}


void Pb14_On (void)
{
    PB14_ON;
}
#endif    // MAGNET_INFINITY

//--- end of file ---//
