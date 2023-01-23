//----------------------------------------------------------
// #### MAGNET PROJECT - Custom Board ####
// ## Internal Test Functions Module
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEST_FUNCTIONS.C ###################################
//----------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "test_functions.h"
#include "hard.h"
#include "adc.h"
#include "usart.h"
#include "dma.h"
#include "tim.h"
#include "pwm.h"

#include "comms_from_rasp.h"

#include <stdio.h>
#include <string.h>


// Externals -------------------------------------------------------------------
extern volatile unsigned short adc_ch [];
extern volatile unsigned short wait_ms_var;
extern volatile unsigned char usart3_have_data;


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------
void TF_Led_1 (void);
void TF_Led_2 (void);
void TF_Buzzer (void);

void TF_Tim5_Channel4_Pwm (void);

void TF_Tim8_Channel1_Pwm (void);
void TF_Tim8_Channel2_Pwm (void);

void TF_Reset_Input (void);
void TF_Usart1_Tx_Int (void);
void TF_Usart1_Tx_Rx_Int (void);
void TF_Usart2_Tx_Int (void);
void TF_Usart2_Tx_Rx_Int (void);
void TF_Adc_PB0_Usart2 (void);

void TF_Usart3_Tx_Int (void);
void TF_Usart3_Tx_Rx_Int (void);
void TF_Spi1_Send_Single (void);
void TF_Spi_Chip_Select (void);
void TF_Spi1_Send_Single_Chip_Select (void);
void TF_Spi1_Send_Receive_Usart2 (void);
void TF_Timer1_Ch1_Pwm (void);    

void TF_Usart3_Tx (void);
void TF_Usart3Loop (void);
void TF_Usart3TxRx (void);
void TF_CommsWithRaspberry (void);

// Module Functions ------------------------------------------------------------
void TF_Hardware_Tests (void)
{
    // TF_Led_1 ();
    // TF_Led_2 ();
    // TF_Buzzer ();

    // TF_Tim5_Channel4_Pwm ();
    // TF_Tim8_Channel1_Pwm();
    TF_Tim8_Channel2_Pwm();
    
    // TF_Reset_Input ();
    // TF_Usart1_Tx_Int ();
    // TF_Usart1_Tx_Rx_Int ();
    // TF_Usart2_Tx_Int ();
    // TF_Usart2_Tx_Rx_Int ();
    // TF_Adc_PB0_Usart2 ();
    
}


void TF_Led_1 (void)
{
    while (1)
    {
        if (LED1)
            LED1_OFF;
        else
            LED1_ON;

        Wait_ms(100);
    }
}


void TF_Led_2 (void)
{
    while (1)
    {
        if (LED2)
            LED2_OFF;
        else
            LED2_ON;

        Wait_ms(100);
    }
}


void TF_Buzzer (void)
{
    while (1)
    {
        BUZZER_ON;
        LED1_ON;
        Wait_ms(50);
        BUZZER_OFF;
        LED1_OFF;        
        Wait_ms(1450);
    }
}


void TF_Tim5_Channel4_Pwm (void)
{
    TIM5_Init();
    
    while (1)
    {
        HIGH_LEFT_PWM_CH4 (DUTY_50_PERCENT);
        LOW_RIGHT_PWM_CH4 (DUTY_50_PERCENT);
        LED1_ON;
        Wait_ms(5000);

        HIGH_LEFT_PWM_CH4 (DUTY_NONE);
        LOW_RIGHT_PWM_CH4 (DUTY_NONE);
        LED1_OFF;
        Wait_ms(5000);
        
    }    
}


void TF_Tim8_Channel1_Pwm (void)
{
    TIM8_Init();
    
    while (1)
    {
        HIGH_LEFT_PWM_CH1 (DUTY_50_PERCENT);
        LOW_RIGHT_PWM_CH1 (DUTY_50_PERCENT);
        LED1_ON;
        Wait_ms(5000);

        HIGH_LEFT_PWM_CH1 (DUTY_NONE);
        LOW_RIGHT_PWM_CH1 (DUTY_NONE);
        LED1_OFF;
        Wait_ms(5000);
        
    }    
}


void TF_Tim8_Channel2_Pwm (void)
{
    TIM8_Init();
    
    while (1)
    {
        HIGH_LEFT_PWM_CH2 (DUTY_50_PERCENT);
        LOW_RIGHT_PWM_CH2 (DUTY_50_PERCENT);
        LED1_ON;
        Wait_ms(5000);

        HIGH_LEFT_PWM_CH2 (DUTY_NONE);
        LOW_RIGHT_PWM_CH2 (DUTY_NONE);
        LED1_OFF;
        Wait_ms(5000);
        
    }    
}


void TF_Voltages (void)
{
    //--- Test ADC Multiple conversion Scanning Continuous Mode and DMA -------------------//
    char buff [100] = { 0 };
    unsigned int seq_cnt = 0;
    ADC_START;
    while (1)
    {
        if (!wait_ms_var)
        {
            sprintf(buff, "High supply: %d, Low supply: %d, seq: %d\n",
                    Sense_200V,
                    Sense_15V,
                    seq_cnt);
            
            RPI_Send(buff);
            wait_ms_var = 1000;
            seq_cnt = 0;
        }

        if (sequence_ready)
        {
            seq_cnt++;
            sequence_ready_reset;
        }
    }
    //--- End Test ADC Multiple conversion Scanning Continuous Mode and DMA ----------------//        
}


void TF_Usart3_Tx (void)
{
    while (1)
    {
        unsigned char snd = 'd';
        Usart3SendUnsigned(&snd, 1);
        Wait_ms(100);
    }
}


// Terminal Looping on Usart3
void TF_Usart3Loop (void)
{
    char local_buff [100] = { 0 };
    unsigned char i = 0;
    
    while (1)
    {
        if (usart3_have_data)
        {
            usart3_have_data = 0;
            LED1_ON;
            ReadUsart3Buffer((unsigned char *)local_buff, 64);
            Wait_ms(1000);
            i = strlen(local_buff);
            if (i < 62)
            {
                local_buff[i] = '\n';
                local_buff[i+1] = '\0';
                Usart3Send(local_buff);
            }
            LED1_OFF;
        }
    }
}


void TF_Usart3TxRx (void)
{
    char local_buff [100] = { 0 };
    
    while (1)
    {
        // Wait_ms(2000);
        LED1_OFF;
        Usart3Send("HOLA!!!\n");
        Wait_ms(100);

        if (usart3_have_data)
        {
            usart3_have_data = 0;
            LED1_ON;
            ReadUsart3Buffer((unsigned char *)local_buff, 64);
            if (strcmp((const char *) "HOLA!!!", local_buff) == 0)
                LED2_ON;
            else
                LED2_OFF;

            Wait_ms(100);
            LED1_OFF;
            LED2_OFF;
            Wait_ms(1900);
        }
    }
}


// Tests communications between Rasp or PC and this board
void TF_CommsWithRaspberry (void)
{
    while (1)
    {
        UpdateRaspberryMessages();
    }
}




//--- end of file ---//
