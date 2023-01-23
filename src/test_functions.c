//----------------------------------------------------------
// #### STRETCHER PROJECT - Custom Board ####
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
#include "timer.h"

#include "comms_from_rasp.h"

#include <stdio.h>
#include <string.h>


// Externals -------------------------------------------------------------------
extern volatile unsigned short adc_ch [];
extern volatile unsigned short wait_ms_var;
extern volatile unsigned char usart3_have_data;


// Globals ---------------------------------------------------------------------


// Module Private Types & Macros -----------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
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
