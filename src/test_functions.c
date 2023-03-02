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
#include "stm32f10x.h"
#include "adc.h"
#include "usart.h"
#include "dma.h"
#include "tim.h"
#include "pwm.h"

#include "comms_from_rasp.h"

#include "comms_channels.h"
#include "antennas.h"

#include "treatment.h"

#include <stdio.h>
#include <string.h>


// Externals -------------------------------------------------------------------
extern volatile unsigned short adc_ch [];
extern volatile unsigned short wait_ms_var;
extern volatile unsigned short timer_standby;
// extern volatile unsigned char usart3_have_data;


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------
void TF_Led_1 (void);
void TF_Led_2 (void);
void TF_Buzzer (void);

void TF_Tim8_Channel1_Pwm (void);
void TF_Tim8_Channel2_Pwm (void);
void TF_Tim4_Channel3_Pwm (void);
void TF_Tim5_Channel4_Pwm (void);

void TF_PROT_Input_Ch1_Ch2 (void);
void TF_PROT_Input_Ch3_Ch4 (void);

void TF_Usart1_Tx (void);
void TF_Usart1_Tx_String (void);

void TF_Adc_Usart1_Tx (void);
void TF_Adc_Usart1_Voltages (void);
void TF_Adc_Usart1_IS (void);

void TF_Usart2_keepalive (void);
void TF_Usart3_keepalive (void);
void TF_Uart4_keepalive (void);
void TF_Uart5_keepalive (void);

void TF_Usart2_connect (void);
void TF_Usart3_connect (void);
void TF_Uart4_connect (void);
void TF_Uart5_connect (void);

void TF_Signal_PWM_Channel1 (void);
void TF_Signal_PWM_Channel2 (void);
void TF_Signal_PWM_Channel3 (void);
void TF_Signal_PWM_Channel4 (void);

short TF_PI_roof (short setpoint, short sample, short last_d);
void TF_PI_roof_Set (short p, short i, short ez1);
void TF_PI_roof_Flush (void);

void TF_Antennas_Connection (void);
void TF_Treatment_And_Antennas_Connection (void);


// Module Functions ------------------------------------------------------------
void TF_Hardware_Tests (void)
{
    // TF_Led_1 ();
    // TF_Led_2 ();
    // TF_Buzzer ();

    // TF_Tim5_Channel4_Pwm ();
    // TF_Tim8_Channel1_Pwm();
    // TF_Tim8_Channel2_Pwm();
    // TF_Tim4_Channel3_Pwm();
    
    // TF_PROT_Input_Ch1_Ch2 ();
    // TF_PROT_Input_Ch3_Ch4 ();

    // TF_Usart1_Tx ();
    // TF_Usart1_Tx_String ();
    
    // TF_Adc_Usart1_Tx ();
    // TF_Adc_Usart1_Voltages ();
    // TF_Adc_Usart1_IS ();

    // TF_Usart2_keepalive ();
    // TF_Usart3_keepalive ();    
    // TF_Uart4_keepalive ();
    // TF_Uart5_keepalive ();

    // TF_Usart2_connect ();
    // TF_Usart3_connect ();
    // TF_Uart4_connect ();
    // TF_Uart5_connect ();    

    // TF_Signal_PWM_Channel1 ();
    // TF_Signal_PWM_Channel2 ();
    // TF_Signal_PWM_Channel3 ();
    // TF_Signal_PWM_Channel4 ();

    // TF_Antennas_Connection ();
    TF_Treatment_And_Antennas_Connection ();
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


void TF_Tim4_Channel3_Pwm (void)
{
    TIM4_Init();
    
    while (1)
    {
        HIGH_LEFT_PWM_CH3 (DUTY_50_PERCENT);
        LOW_RIGHT_PWM_CH3 (DUTY_50_PERCENT);
        LED1_ON;
        Wait_ms(5000);

        HIGH_LEFT_PWM_CH3 (DUTY_NONE);
        LOW_RIGHT_PWM_CH3 (DUTY_NONE);
        LED1_OFF;
        Wait_ms(5000);
        
    }    
}


void TF_PROT_Input_Ch1_Ch2 (void)
{
    
    while (1)
    {
        if (PROT_CH1)
            LED1_ON;
        else
            LED1_OFF;

        if (PROT_CH2)
            LED2_ON;
        else
            LED2_OFF;
    }    
}


void TF_PROT_Input_Ch3_Ch4 (void)
{
    
    while (1)
    {
        if (PROT_CH3)
            LED1_ON;
        else
            LED1_OFF;

        if (PROT_CH4)
            LED2_ON;
        else
            LED2_OFF;
    }    
}


void TF_Usart1_Tx (void)
{
    Usart1Config();
    
    while (1)
    {
        unsigned char snd = 'M';
        Usart1SendUnsigned(&snd, 1);
        Wait_ms(100);
    }
}


void TF_Usart1_Tx_String (void)
{
    Usart1Config();
    
    while (1)
    {
        Usart1Send("Mariano\n");
        Wait_ms(2000);
    }
}


void TF_Adc_Usart1_Tx (void)
{
    char buff [100] = { 0 };
    unsigned int seq_cnt = 0;

    //-- Test ADC Multiple conversion Scanning Continuous Mode and DMA 
    //-- DMA configuration.
    DMAConfig();
    DMA_ENABLE;
    
    //Uso ADC con DMA
    AdcConfig();
    ADC_START;

    Usart1Config ();
    
    while (1)
    {
        if (!wait_ms_var)
        {
            sprintf(buff, "High supply: %d, Low supply: %d, Twelve: %d seq: %d\n",
                    Sense_200V,
                    Sense_15V,
                    Sense_12V,
                    seq_cnt);
            
            Usart1Send (buff);
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


void TF_Adc_Usart1_Voltages (void)
{
    char buff [100] = { 0 };
    unsigned int seq_cnt = 0;

    //-- Test ADC Multiple conversion Scanning Continuous Mode and DMA 
    //-- DMA configuration.
    DMAConfig();
    DMA_ENABLE;
    
    //Uso ADC con DMA
    AdcConfig();
    ADC_START;

    Usart1Config ();
    
    while (1)
    {
        if (!wait_ms_var)
        {
            int calc_int, calc_dec;

            calc_int = Sense_200V * 3300;
            calc_int = calc_int / 4054;
            calc_dec = calc_int;
            calc_int = calc_int / 10;
            calc_dec = calc_dec - calc_int * 10;
            
            sprintf(buff, "High supply: %d.%01d, ", calc_int, calc_dec);
            Usart1Send (buff);

            calc_int = Sense_15V * 3300;
            // calc_int = calc_int / 5339;
            calc_int = calc_int / 5310;    // ajusto decimales
            calc_dec = calc_int;
            calc_int = calc_int / 100;
            calc_dec = calc_dec - calc_int * 100;
            
            sprintf(buff, "Low supply: %d.%02d, ", calc_int, calc_dec);
            Usart1Send (buff);

            calc_int = Sense_12V * 3300;
            calc_int = calc_int / 5339;
            calc_dec = calc_int;
            calc_int = calc_int / 100;
            calc_dec = calc_dec - calc_int * 100;
            sprintf(buff, "Twelve supply: %d.%02d, ", calc_int, calc_dec);
            Usart1Send (buff);

            sprintf(buff, "seq: %d\n", seq_cnt);
            Usart1Send (buff);
            
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


void TF_Adc_Usart1_IS (void)
{
    char buff [100] = { 0 };
    unsigned int seq_cnt = 0;

    //-- Test ADC Multiple conversion Scanning Continuous Mode and DMA 
    //-- DMA configuration.
    DMAConfig();
    DMA_ENABLE;
    
    //Uso ADC con DMA
    AdcConfig();
    ADC_START;

    Usart1Config ();
    
    while (1)
    {
        if (!wait_ms_var)
        {
            sprintf(buff, "IS_CH1: %d IS_CH2: %d IS_CH3: %d IS_CH4: %d, seq: %d\n",
                    IS_CH1,
                    IS_CH2,
                    IS_CH3,
                    IS_CH4,                    
                    seq_cnt);
            
            Usart1Send (buff);
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


unsigned short sinusoidal_table [] = {25,50,75,100,125,150,174,199,224,248,
                                      272,296,320,344,368,391,414,437,459,482,
                                      504,525,547,568,589,609,629,648,668,687,
                                      705,723,740,757,774,790,806,821,836,850,
                                      864,877,890,902,913,924,935,945,954,963,
                                      971,978,985,992,998,1003,1007,1011,1015,1018,
                                      1020,1021,1022,1023,1022,1021,1020,1018,1015,1011,
                                      1007,1003,998,992,985,978,971,963,954,945,
                                      935,924,913,902,890,877,864,850,836,821,
                                      806,790,774,757,740,723,705,687,668,648,
                                      629,609,589,568,547,525,504,482,459,437,
                                      414,391,368,344,320,296,272,248,224,199,
                                      174,150,125,100,75,50,25,0,0,0,
                                      0,0,0,0,0,0,0,0,0,0,
                                      0,0,0,0,0,0,0,0,0,0,
                                      0,0,0,0,0,0,0,0,0,0,
                                      0,0,0,0,0,0,0,0,0,0,
                                      0,0,0,0,0,0,0,0,0,0,
                                      0,0,0,0,0,0,0,0,0,0,
                                      0,0,0,0,0,0,0,0,0,0,
                                      0,0,0,0,0,0,0,0,0,0,
                                      0,0,0,0,0,0,0,0,0,0,
                                      0,0,0,0,0,0,0,0,0,0,
                                      0,0,0,0,0,0,0,0,0,0,
                                      0,0,0,0,0,0,0,0,0,0,
                                      0,0,0,0,0,0};

extern volatile unsigned char timer1_seq_ready;
void TF_Signal_PWM_Channel1 (void)
{
    char buff [100] = { 0 };

    //-- Test ADC Multiple conversion Scanning Continuous Mode and DMA 
    //-- DMA configuration.
    DMAConfig();
    DMA_ENABLE;
    
    //Uso ADC con DMA
    AdcConfig();
    ADC_START;

    Usart1Config ();

    // init timer 8 for ch1 pwm output
    TIM8_Init();
    
    // init timer 1 for sequence ready
    TIM1_Init();
    
    unsigned int seq_ready_cnt = 0;
    short duty = 0;
    unsigned short signal_index = 0;
    char show_duty = 0;
    short show_d [256] = { 0 };
    short show_is [256] = { 0 };
    short show_sp [256] = { 0 };    
    
    while (1)
    {
        // five seconds generation on ch1
        Usart1Send ("Five seconds generation on ch1\n");
        wait_ms_var = 5000;
        seq_ready_cnt = 0;

        // init the signal index and pid variables
        signal_index = 0;
        duty = 0;
        TF_PI_roof_Set (100, 40, 0);

        show_duty = 0;
        for (int i = 0; i < 256; i++)
        {
            show_d[i] = 0;
            show_is[i] = 0;
            show_sp[i] = 0;            
        }
        
        do {
            if (timer1_seq_ready)
            {
                timer1_seq_ready = 0;
                seq_ready_cnt++;
                
                // get the current SP
                unsigned int sp = sinusoidal_table [signal_index];

                // max current fixt
                sp = sp * 887;    // 0.715V
                // sp = sp * 1737;    // 1.4V
                // sp = sp * 173;    // 140mV
                sp >>= 10;    // div 1024

                // index update (modulo 8?)               
                if (signal_index < 255)
                    signal_index++;
                else
                    signal_index = 0;

                // get the duty cycle
                if (sp == 0)
                {
                    //fast discharge
                    HIGH_LEFT_PWM_CH1 (DUTY_NONE);
                    LOW_RIGHT_PWM_CH1 (DUTY_NONE);
                    duty = 0;
                    TF_PI_roof_Flush ();
                }
                else
                {
                    // normal emission
                    duty = TF_PI_roof (sp, IS_CH1, duty);

                    if (duty > 0)
                    {
                        if (duty > DUTY_95_PERCENT)
                            duty = DUTY_95_PERCENT;
                        
                        HIGH_LEFT_PWM_CH1 (duty);
                        LOW_RIGHT_PWM_CH1 (DUTY_ALWAYS);
                    }
                    // else if (duty > -40)
                    else if (duty == 0)
                    {
                        HIGH_LEFT_PWM_CH1 (DUTY_NONE);
                        LOW_RIGHT_PWM_CH1 (DUTY_ALWAYS);
                    }
                    else    // duty negative
                    {
                        // fixt duty
                        duty = -duty;

                        if (duty > DUTY_95_PERCENT)
                            duty = DUTY_95_PERCENT;
                                                
                        HIGH_LEFT_PWM_CH1 (DUTY_NONE);
                        // LOW_RIGHT_PWM_CH1 (duty >> 1);
                        // duty >>= 4;                        
                        LOW_RIGHT_PWM_CH1 (duty);                        

                        // fixt duty
                        duty = -duty;
                    }

                    if (!show_duty)
                    {
                        if ((signal_index) && (signal_index < 255))
                        {
                            show_d[signal_index - 1] = duty;
                            show_is[signal_index - 1] = IS_CH1;
                            show_sp[signal_index - 1] = sp;
                        }
                        else
                            show_duty = 1;
                    }
                }
            }
            
        } while (wait_ms_var);

        seq_ready_cnt = seq_ready_cnt / 5;
        sprintf(buff, "Ten seconds rest seq in second: %d\n", seq_ready_cnt);
        Usart1Send (buff);

        for (int i = 0; i < 130; i++)
        {
            sprintf(buff, "i: %03d sp: %d is: %d d: %d\n",
                    i,
                    show_sp[i],
                    show_is[i],
                    show_d[i]);
            
            Usart1Send (buff);
            Wait_ms(30);
        }
        Wait_ms(10000);
        
    }
}


void TF_Signal_PWM_Channel2 (void)
{
    char buff [100] = { 0 };

    //-- Test ADC Multiple conversion Scanning Continuous Mode and DMA 
    //-- DMA configuration.
    DMAConfig();
    DMA_ENABLE;
    
    //Uso ADC con DMA
    AdcConfig();
    ADC_START;

    Usart1Config ();

    // init timer 8 for ch2 pwm output
    TIM8_Init();
    
    // init timer 1 for sequence ready
    TIM1_Init();
    
    unsigned int seq_ready_cnt = 0;
    short duty = 0;
    unsigned short signal_index = 0;
    char show_duty = 0;
    short show_d [256] = { 0 };
    short show_is [256] = { 0 };
    short show_sp [256] = { 0 };    
    
    while (1)
    {
        // five seconds generation on ch2
        Usart1Send ("Five seconds generation on ch2\n");
        wait_ms_var = 5000;
        seq_ready_cnt = 0;

        // init the signal index and pid variables
        signal_index = 0;
        duty = 0;
        TF_PI_roof_Set (100, 40, 0);

        show_duty = 0;
        for (int i = 0; i < 256; i++)
        {
            show_d[i] = 0;
            show_is[i] = 0;
            show_sp[i] = 0;            
        }
        
        do {
            if (timer1_seq_ready)
            {
                timer1_seq_ready = 0;
                seq_ready_cnt++;
                
                // get the current SP
                unsigned int sp = sinusoidal_table [signal_index];

                // max current fixt
                // sp = sp * 887;    // 0.715V
                // sp = sp * 1737;    // 1.4V
                sp = sp * 173;    // 140mV
                sp >>= 10;    // div 1024

                // index update (modulo 8?)               
                if (signal_index < 255)
                    signal_index++;
                else
                    signal_index = 0;

                // get the duty cycle
                if (sp == 0)
                {
                    //fast discharge
                    HIGH_LEFT_PWM_CH2 (DUTY_NONE);
                    LOW_RIGHT_PWM_CH2 (DUTY_NONE);
                    duty = 0;
                    TF_PI_roof_Flush ();
                }
                else
                {
                    // normal emission
                    duty = TF_PI_roof (sp, IS_CH2, duty);

                    if (duty > 0)
                    {
                        if (duty > DUTY_95_PERCENT)
                            duty = DUTY_95_PERCENT;
                        
                        HIGH_LEFT_PWM_CH2 (duty);
                        LOW_RIGHT_PWM_CH2 (DUTY_ALWAYS);
                    }
                    // else if (duty > -40)
                    else if (duty == 0)
                    {
                        HIGH_LEFT_PWM_CH2 (DUTY_NONE);
                        LOW_RIGHT_PWM_CH2 (DUTY_ALWAYS);
                    }
                    else    // duty negative
                    {
                        // fixt duty
                        duty = -duty;

                        if (duty > DUTY_95_PERCENT)
                            duty = DUTY_95_PERCENT;
                                                
                        HIGH_LEFT_PWM_CH2 (DUTY_NONE);
                        // LOW_RIGHT_PWM_CH2 (duty >> 1);
                        // duty >>= 4;                        
                        LOW_RIGHT_PWM_CH2 (duty);                        

                        // fixt duty
                        duty = -duty;
                    }

                    if (!show_duty)
                    {
                        if ((signal_index) && (signal_index < 255))
                        {
                            show_d[signal_index - 1] = duty;
                            show_is[signal_index - 1] = IS_CH2;
                            show_sp[signal_index - 1] = sp;
                        }
                        else
                            show_duty = 1;
                    }
                }
            }
            
        } while (wait_ms_var);

        seq_ready_cnt = seq_ready_cnt / 5;
        sprintf(buff, "Ten seconds rest seq in second: %d\n", seq_ready_cnt);
        Usart1Send (buff);

        for (int i = 0; i < 130; i++)
        {
            sprintf(buff, "i: %03d sp: %d is: %d d: %d\n",
                    i,
                    show_sp[i],
                    show_is[i],
                    show_d[i]);
            
            Usart1Send (buff);
            Wait_ms(30);
        }
        Wait_ms(10000);
        
    }
}


void TF_Signal_PWM_Channel3 (void)
{
    char buff [100] = { 0 };

    //-- Test ADC Multiple conversion Scanning Continuous Mode and DMA 
    //-- DMA configuration.
    DMAConfig();
    DMA_ENABLE;
    
    //Uso ADC con DMA
    AdcConfig();
    ADC_START;

    Usart1Config ();

    // init timer 4 for channel 3 pwm output
    TIM4_Init();
    
    // init timer 1 for sequence ready
    TIM1_Init();
    
    unsigned int seq_ready_cnt = 0;
    short duty = 0;
    unsigned short signal_index = 0;
    char show_duty = 0;
    short show_d [256] = { 0 };
    short show_is [256] = { 0 };
    short show_sp [256] = { 0 };    
    
    while (1)
    {
        // five seconds generation on ch3
        Usart1Send ("Five seconds generation on ch3\n");
        wait_ms_var = 5000;
        seq_ready_cnt = 0;

        // init the signal index and pid variables
        signal_index = 0;
        duty = 0;
        TF_PI_roof_Set (100, 40, 0);

        show_duty = 0;
        for (int i = 0; i < 256; i++)
        {
            show_d[i] = 0;
            show_is[i] = 0;
            show_sp[i] = 0;            
        }
        
        do {
            if (timer1_seq_ready)
            {
                timer1_seq_ready = 0;
                seq_ready_cnt++;
                
                // get the current SP
                unsigned int sp = sinusoidal_table [signal_index];

                // max current fixt
                // sp = sp * 173;    // 140mV
                // sp = sp * 887;    // 0.715V
                sp = sp * 1737;    // 1.4V
                sp >>= 10;    // div 1024

                // index update (modulo 8?)               
                if (signal_index < 255)
                    signal_index++;
                else
                    signal_index = 0;

                // get the duty cycle
                if (sp == 0)
                {
                    //fast discharge
                    HIGH_LEFT_PWM_CH3 (DUTY_NONE);
                    LOW_RIGHT_PWM_CH3 (DUTY_NONE);
                    duty = 0;
                    TF_PI_roof_Flush ();
                }
                else
                {
                    // normal emission
                    duty = TF_PI_roof (sp, IS_CH3, duty);

                    if (duty > 0)
                    {
                        if (duty > DUTY_95_PERCENT)
                            duty = DUTY_95_PERCENT;
                        
                        HIGH_LEFT_PWM_CH3 (duty);
                        LOW_RIGHT_PWM_CH3 (DUTY_ALWAYS);
                    }
                    // else if (duty > -40)
                    else if (duty == 0)
                    {
                        HIGH_LEFT_PWM_CH3 (DUTY_NONE);
                        LOW_RIGHT_PWM_CH3 (DUTY_ALWAYS);
                    }
                    else    // duty negative
                    {
                        // fixt duty
                        duty = -duty;

                        if (duty > DUTY_95_PERCENT)
                            duty = DUTY_95_PERCENT;
                                                
                        HIGH_LEFT_PWM_CH3 (DUTY_NONE);
                        // LOW_RIGHT_PWM_CH3 (duty >> 1);
                        // duty >>= 4;                        
                        LOW_RIGHT_PWM_CH3 (duty);                        

                        // fixt duty
                        duty = -duty;
                    }

                    if (!show_duty)
                    {
                        if ((signal_index) && (signal_index < 255))
                        {
                            show_d[signal_index - 1] = duty;
                            show_is[signal_index - 1] = IS_CH3;
                            show_sp[signal_index - 1] = sp;
                        }
                        else
                            show_duty = 1;
                    }
                }
            }
            
        } while (wait_ms_var);

        seq_ready_cnt = seq_ready_cnt / 5;
        sprintf(buff, "Ten seconds rest seq in second: %d\n", seq_ready_cnt);
        Usart1Send (buff);

        for (int i = 0; i < 130; i++)
        {
            sprintf(buff, "i: %03d sp: %d is: %d d: %d\n",
                    i,
                    show_sp[i],
                    show_is[i],
                    show_d[i]);
            
            Usart1Send (buff);
            Wait_ms(30);
        }
        Wait_ms(10000);
        
    }
}


void TF_Signal_PWM_Channel4 (void)
{
    char buff [100] = { 0 };

    //-- Test ADC Multiple conversion Scanning Continuous Mode and DMA 
    //-- DMA configuration.
    DMAConfig();
    DMA_ENABLE;
    
    //Uso ADC con DMA
    AdcConfig();
    ADC_START;

    Usart1Config ();

    // init timer 5 for channel 4 pwm output
    TIM5_Init();
    
    // init timer 1 for sequence ready
    TIM1_Init();
    
    unsigned int seq_ready_cnt = 0;
    short duty = 0;
    unsigned short signal_index = 0;
    char show_duty = 0;
    short show_d [256] = { 0 };
    short show_is [256] = { 0 };
    short show_sp [256] = { 0 };    
    
    while (1)
    {
        // five seconds generation on ch4
        Usart1Send ("Five seconds generation on ch4\n");
        wait_ms_var = 5000;
        seq_ready_cnt = 0;

        // init the signal index and pid variables
        signal_index = 0;
        duty = 0;
        TF_PI_roof_Set (100, 40, 0);

        show_duty = 0;
        for (int i = 0; i < 256; i++)
        {
            show_d[i] = 0;
            show_is[i] = 0;
            show_sp[i] = 0;            
        }
        
        do {
            if (timer1_seq_ready)
            {
                timer1_seq_ready = 0;
                seq_ready_cnt++;
                
                // get the current SP
                unsigned int sp = sinusoidal_table [signal_index];

                // max current fixt
                // sp = sp * 173;    // 140mV
                // sp = sp * 887;    // 0.715V
                sp = sp * 1737;    // 1.4V
                sp >>= 10;    // div 1024

                // index update (modulo 8?)               
                if (signal_index < 255)
                    signal_index++;
                else
                    signal_index = 0;

                // get the duty cycle
                if (sp == 0)
                {
                    //fast discharge
                    HIGH_LEFT_PWM_CH4 (DUTY_NONE);
                    LOW_RIGHT_PWM_CH4 (DUTY_NONE);
                    duty = 0;
                    TF_PI_roof_Flush ();
                }
                else
                {
                    // normal emission
                    duty = TF_PI_roof (sp, IS_CH4, duty);

                    if (duty > 0)
                    {
                        if (duty > DUTY_95_PERCENT)
                            duty = DUTY_95_PERCENT;
                        
                        HIGH_LEFT_PWM_CH4 (duty);
                        LOW_RIGHT_PWM_CH4 (DUTY_ALWAYS);
                    }
                    // else if (duty > -40)
                    else if (duty == 0)
                    {
                        HIGH_LEFT_PWM_CH4 (DUTY_NONE);
                        LOW_RIGHT_PWM_CH4 (DUTY_ALWAYS);
                    }
                    else    // duty negative
                    {
                        // fixt duty
                        duty = -duty;

                        if (duty > DUTY_95_PERCENT)
                            duty = DUTY_95_PERCENT;
                                                
                        HIGH_LEFT_PWM_CH4 (DUTY_NONE);
                        // LOW_RIGHT_PWM_CH4 (duty >> 1);
                        // duty >>= 4;                        
                        LOW_RIGHT_PWM_CH4 (duty);                        

                        // fixt duty
                        duty = -duty;
                    }

                    if (!show_duty)
                    {
                        if ((signal_index) && (signal_index < 255))
                        {
                            show_d[signal_index - 1] = duty;
                            show_is[signal_index - 1] = IS_CH4;
                            show_sp[signal_index - 1] = sp;
                        }
                        else
                            show_duty = 1;
                    }
                }
            }
            
        } while (wait_ms_var);

        seq_ready_cnt = seq_ready_cnt / 5;
        sprintf(buff, "Ten seconds rest seq in second: %d\n", seq_ready_cnt);
        Usart1Send (buff);

        for (int i = 0; i < 130; i++)
        {
            sprintf(buff, "i: %03d sp: %d is: %d d: %d\n",
                    i,
                    show_sp[i],
                    show_is[i],
                    show_d[i]);
            
            Usart1Send (buff);
            Wait_ms(30);
        }
        Wait_ms(10000);
        
    }
}


// #define K1    (pid_param_p + pid_param_i + pid_param_d)
// #define K2    (pid_param_p + pid_param_d + pid_param_d)
// #define K3    (pid_param_d)
short pi_p = 0;    
short pi_i = 0;
short error_z1 = 0;
short TF_PI_roof (short setpoint, short sample, short last_d)
{
    int acc = 0;
    
    short error = setpoint - sample;

    // Integral accumulator part
    // int_acc = pi_i * error;
    
    // K1
    acc = (pi_p + pi_i) * error;
    // acc = pi_p  * error + int_acc;    
    short val_k1 = acc >> 7;

    // K2
    acc = pi_p * error_z1;
    short val_k2 = acc >> 7;

    // PI
    short d = last_d + val_k1 - val_k2;    
    error_z1 = error;

    return d;
}


void TF_PI_roof_Set (short p, short i, short ez1)
{
    pi_p = p;
    pi_i = i;
    error_z1 = ez1;
}


void TF_PI_roof_Flush (void)
{
    error_z1 = 0;
}


void TF_Usart2_keepalive (void)
{
    char buff [128] = { 0 };
    
    Usart2Config ();
    
    while (1)
    {
        if (!timer_standby)
        {
            LED1_ON;
            Usart2Send("keepalive\n");
            Wait_ms(100);
            timer_standby = 2000;
            LED1_OFF;
        }

        if (Usart2HaveData())
        {
            Usart2ReadBuffer(buff, 128);
            if (!strncmp(buff, "keepalive", sizeof("keepalive") - 1))
            {
                LED2_ON;
                Wait_ms(100);
                LED2_OFF;
            }
            Usart2HaveDataReset();
        }
    }
}


void TF_Usart3_keepalive (void)
{
    char buff [128] = { 0 };
    
    Usart3Config ();
    
    while (1)
    {
        if (!timer_standby)
        {
            LED1_ON;
            Usart3Send("keepalive\n");
            Wait_ms(100);
            timer_standby = 2000;
            LED1_OFF;
        }

        if (Usart3HaveData())
        {
            Usart3ReadBuffer(buff, 128);
            if (!strncmp(buff, "keepalive", sizeof("keepalive") - 1))
            {
                LED2_ON;
                Wait_ms(100);
                LED2_OFF;
            }
            Usart3HaveDataReset();
        }
    }
}


void TF_Uart4_keepalive (void)
{
    char buff [128] = { 0 };
    
    Uart4Config ();
    
    while (1)
    {
        if (!timer_standby)
        {
            LED1_ON;
            Uart4Send("keepalive\n");
            Wait_ms(100);
            timer_standby = 2000;
            LED1_OFF;
        }

        if (Uart4HaveData())
        {
            Uart4ReadBuffer(buff, 128);
            if (!strncmp(buff, "keepalive", sizeof("keepalive") - 1))
            {
                LED2_ON;
                Wait_ms(100);
                LED2_OFF;
            }
            Uart4HaveDataReset();
        }
    }
}


void TF_Uart5_keepalive (void)
{
    char buff [128] = { 0 };
    
    Uart5Config ();
    
    while (1)
    {
        if (!timer_standby)
        {
            LED1_ON;
            Uart5Send("keepalive\n");
            Wait_ms(100);
            timer_standby = 2000;
            LED1_OFF;
        }

        if (Uart5HaveData())
        {
            Uart5ReadBuffer(buff, 128);
            if (!strncmp(buff, "keepalive", sizeof("keepalive") - 1))
            {
                LED2_ON;
                Wait_ms(100);
                LED2_OFF;
            }
            Uart5HaveDataReset();
        }
    }
}


void TF_Usart2_connect (void)
{
    char buff [128] = { 0 };

    Usart1Config ();    
    Usart1Send("Testing usart2 antenna connection\n");
    Usart2Config ();

    while (1)
    {
        if (!timer_standby)
        {
            LED1_ON;
            Usart2Send("keepalive\n");
            wait_ms_var = 100;
            timer_standby = 2000;
        }

        if (!wait_ms_var)
            LED1_OFF;

        if (Usart2HaveData())
        {
            Usart2ReadBuffer(buff, 128);
            Usart1Send("<- ");
            Usart1Send(buff);
            Usart1Send("\n");
            if (!strncmp(buff, "ok", sizeof("ok") - 1))
            {
                LED2_ON;
                Wait_ms(100);
                LED2_OFF;
            }
            Usart2HaveDataReset();
        }
    }
}


void TF_Usart3_connect (void)
{
    char buff [128] = { 0 };

    Usart1Config ();    
    Usart1Send("Testing usart3 antenna connection\n");
    Usart3Config ();

    while (1)
    {
        if (!timer_standby)
        {
            LED1_ON;
            Usart3Send("keepalive\n");
            wait_ms_var = 100;
            timer_standby = 2000;
        }

        if (!wait_ms_var)
            LED1_OFF;

        if (Usart3HaveData())
        {
            Usart3ReadBuffer(buff, 128);
            Usart1Send("<- ");
            Usart1Send(buff);
            Usart1Send("\n");
            if (!strncmp(buff, "ok", sizeof("ok") - 1))
            {
                LED2_ON;
                Wait_ms(100);
                LED2_OFF;
            }
            Usart3HaveDataReset();
        }
    }
}


void TF_Uart4_connect (void)
{
    char buff [128] = { 0 };

    Usart1Config ();    
    Usart1Send("Testing uart4 antenna connection\n");
    Uart4Config ();

    while (1)
    {
        if (!timer_standby)
        {
            LED1_ON;
            Uart4Send("keepalive\n");
            wait_ms_var = 100;
            timer_standby = 2000;
        }

        if (!wait_ms_var)
            LED1_OFF;

        if (Uart4HaveData())
        {
            Uart4ReadBuffer(buff, 128);
            Usart1Send("<- ");
            Usart1Send(buff);
            Usart1Send("\n");
            if (!strncmp(buff, "ok", sizeof("ok") - 1))
            {
                LED2_ON;
                Wait_ms(100);
                LED2_OFF;
            }
            Uart4HaveDataReset();
        }
    }
}


void TF_Uart5_connect (void)
{
    char buff [128] = { 0 };

    Usart1Config ();    
    Usart1Send("Testing uart5 antenna connection\n");
    Uart5Config ();

    while (1)
    {
        if (!timer_standby)
        {
            LED1_ON;
            Uart5Send("keepalive\n");
            // Uart5Send("get_params\n");            
            wait_ms_var = 100;
            timer_standby = 2000;
        }

        if (!wait_ms_var)
            LED1_OFF;

        if (Uart5HaveData())
        {
            Uart5ReadBuffer(buff, 128);
            Usart1Send("<- ");
            Usart1Send(buff);
            Usart1Send("\n");
            if (!strncmp(buff, "ok", sizeof("ok") - 1))
            {
                LED2_ON;
                Wait_ms(100);
                LED2_OFF;
            }
            Uart5HaveDataReset();
        }
    }
}


void TF_Antennas_Connection (void)
{
    Usart1Config ();    
    Usart1Send("Testing antennas connection\n");

    // start channels usarts
    Usart2Config ();
    Usart3Config ();
    Uart4Config ();
    Uart5Config ();    

    while (1)
    {
        // update the antennas module state        
        AntennaUpdateStates ();

        // update the channels comms
        Comms_Channel1 ();
        Comms_Channel2 ();
        Comms_Channel3 ();
        Comms_Channel4 ();
        
        // 1ms timer update
        if (!timer_standby)
        {
            timer_standby = 1;
            AntennaTimeouts ();            
        }
    }
}


void TF_Treatment_And_Antennas_Connection (void)
{
    Usart1Config ();    
    Usart1Send("Testing antennas connection\n");

    // start channels usarts
    Usart2Config ();
    Usart3Config ();
    Uart4Config ();
    Uart5Config ();
    

    while (1)
    {
        // update the antennas module state        
        AntennaUpdateStates ();

        // update the channels comms
        Comms_Channel1 ();
        Comms_Channel2 ();
        Comms_Channel3 ();
        Comms_Channel4 ();

        // update treatment state
        Treatment_Manager();
        
        // 1ms timer update
        if (!timer_standby)
        {
            timer_standby = 1;
            AntennaTimeouts ();
            Treatment_Timeouts ();
        }
    }
}

//--- end of file ---//
