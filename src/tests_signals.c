//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "signals.h"
#include "signals_defs.h"
#include "tim.h"
#include "dsp.h"

#include "antennas_defs.h"

// helper modules
#include "tests_ok.h"
#include "tests_mock_usart.h"
#include "tests_vector_utils.h"
#include "tests_know_antennas.h"

#include <stdio.h>
// #include <math.h>


// Types Constants and Macros --------------------------------------------------
#define SIZEOF_SIGNALS    512


// Externals -------------------------------------------------------------------
volatile unsigned short adc_ch [10];
volatile unsigned char timer1_seq_ready = 0;

extern void Signals_Setup_Phase_Accumulator (unsigned char freq_int,
                                             unsigned char freq_dec,
                                             unsigned short * phase_accum);



// Globals ---------------------------------------------------------------------
// short v_duty_ch1 [SIZEOF_SIGNALS] = { 0 };
// short v_error_ch1 [SIZEOF_SIGNALS] = { 0 };
// short v_sp_ch1 [SIZEOF_SIGNALS] = { 0 };

// short v_duty_ch2 [SIZEOF_SIGNALS] = { 0 };
// short v_error_ch2 [SIZEOF_SIGNALS] = { 0 };
// short v_sp_ch2 [SIZEOF_SIGNALS] = { 0 };

// short v_duty_ch3 [SIZEOF_SIGNALS] = { 0 };
// short v_error_ch3 [SIZEOF_SIGNALS] = { 0 };
// short v_sp_ch3 [SIZEOF_SIGNALS] = { 0 };

// short v_duty_ch4 [SIZEOF_SIGNALS] = { 0 };
// short v_error_ch4 [SIZEOF_SIGNALS] = { 0 };
// short v_sp_ch4 [SIZEOF_SIGNALS] = { 0 };


// Module Functions to Test ----------------------------------------------------
void Test_Setup_Phase_And_Increment (void);
void Test_Signals_Table (void);



// Module Auxiliary Functions --------------------------------------------------


// Tests Module Auxiliary or General Functions ---------------------------------


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    // Test_Setup_Phase_And_Increment ();

    Test_Signals_Table ();

    return 0;
}

extern const unsigned short * p_table_inphase;
extern const unsigned short * p_table_outphase;
extern signals_struct_t global_signals;

void Test_Setup_Phase_And_Increment (void)
{
    unsigned char freq_int = 0;
    unsigned char freq_dec = 50;
    unsigned short phase_a = 0;
    float fs = 6400.0;

    // test for each freq and tables
    printf("with fs: %f table len: 256\n", fs);
    for (int i = 0; i < 200; i += 10)
    {
        freq_int = i;
        Signals_Setup_Phase_Accumulator(freq_int, freq_dec, &phase_a);

        int remainder = 65536 % phase_a;
        float eff_freq = phase_a * fs / (256 * 256);
        printf("estimated for fsampling: %f effective freq: %f\n", fs, eff_freq);
        printf("with %02d.%02dHz phase acc: %d\n\n",
               freq_int,
               freq_dec,
               phase_a);
    }

    // freq_int = 23;
    // freq_dec = 80;
    // printf("\n\nfor exact table and phase, selected freq: %d.%dHz\n", freq_int, freq_dec);
    // float eff_freq = 255 * fs / (256 * 256);
    // printf("estimated for fsampling: %f effective freq: %f\n", fs, eff_freq);

}


extern short table_ch1 [];
extern short table_ch2 [];
extern const unsigned short sinusoidal_table_inphase [];
extern const unsigned short sinusoidal_table_outphase [];
extern const unsigned short triangular_table_inphase [];
extern const unsigned short triangular_table_outphase [];
extern const unsigned short square_table_inphase [];
extern const unsigned short square_table_outphase [];
void Test_Signals_Table (void)
{
    unsigned char freq_int = 25;
    unsigned char freq_dec = 00;
    unsigned short phase_a = 0;
    float fs = 6400.0;

    unsigned short * p_signal_ref;
    
    // set up treatment data
    signals_struct_t new_treat_data;
    new_treat_data.freq_int = freq_int;
    new_treat_data.freq_dec = freq_dec;
    new_treat_data.signal = TRIANGULAR_SIGNAL;
    // new_treat_data.signal = SINUSOIDAL_SIGNAL;
    // new_treat_data.signal = SQUARE_SIGNAL;
    new_treat_data.power = 100;
    Signals_Setup_Treatment_Data(&new_treat_data);

    if (new_treat_data.signal == TRIANGULAR_SIGNAL)
        p_signal_ref = triangular_table_inphase;
    else if (new_treat_data.signal == SINUSOIDAL_SIGNAL)
        p_signal_ref = sinusoidal_table_inphase;
    else if (new_treat_data.signal == SQUARE_SIGNAL)
        p_signal_ref = square_table_inphase;
    else
        printf("error on selected signal for test");
    
    // get antenna params
    antenna_st ch_ant;
                
    Signals_Set_Reset_Channel_For_Treatment(0, 1);    // ch1 antenna present
    TSP_Get_Know_Single_Antenna (&ch_ant, 9);
    Signals_Set_Channel_Table_Open_Loop (0, &ch_ant);

    // Signals_Setup_Phase_Accumulator(freq_int, freq_dec, &phase_a);    

    ///////////////////////////
    // Backup Data to a file //
    ///////////////////////////
    FILE * file = fopen("data.txt", "w");

    if (file == NULL)
    {
        printf("data file not created!\n");
        return;
    }

    Vector_Short_To_File (file, "table", table_ch1, 256);
    Vector_Short_To_File (file, "ref", p_signal_ref, 256);
    // Vector_Short_To_File (file, "setpoint", v_sp_ch1, SIZEOF_SIGNALS);
    printf("\nRun by hand python3 simul_outputs.py\n");
    
}


// void Test_Generate_All_Channels (void)
// {
    
//     Signals_Setup_All_Channels();

//     for (int i = 0; i < (SIZEOF_SIGNALS - 1); i++)
//     {
//         timer1_seq_ready = 1;
//         Signals_Generate_All_Channels ();

//         // save ch1 data
//         v_duty_ch1[i] = pi_ch1.last_d;
//         v_sp_ch1[i] = pi_ch1.setpoint;
//         v_error_ch1[i] = pi_ch1.setpoint - pi_ch1.sample;

//         // save ch2 data
//         v_duty_ch2[i] = pi_ch2.last_d;
//         v_sp_ch2[i] = pi_ch2.setpoint;
//         v_error_ch2[i] = pi_ch2.setpoint - pi_ch2.sample;

//         // save ch3 data
//         v_duty_ch3[i] = pi_ch3.last_d;
//         v_sp_ch3[i] = pi_ch3.setpoint;
//         v_error_ch3[i] = pi_ch3.setpoint - pi_ch3.sample;

//         // save ch4 data
//         v_duty_ch4[i] = pi_ch4.last_d;
//         v_sp_ch4[i] = pi_ch4.setpoint;
//         v_error_ch4[i] = pi_ch4.setpoint - pi_ch4.sample;
        
//     }

//     ///////////////////////////
//     // Backup Data to a file //
//     ///////////////////////////
//     FILE * file = fopen("data.txt", "w");

//     if (file == NULL)
//     {
//         printf("data file not created!\n");
//         return;
//     }

//     Vector_Short_To_File (file, "duty", v_duty_ch1, SIZEOF_SIGNALS);
//     Vector_Short_To_File (file, "error", v_error_ch1, SIZEOF_SIGNALS);
//     Vector_Short_To_File (file, "setpoint", v_sp_ch1, SIZEOF_SIGNALS);
//     printf("\nRun by hand python3 simul_outputs.py\n");
    
// }




// Mocked Module Functions -----------------------------------------------------
void TIM8_Update_CH3 (unsigned short a)
{
    printf("HL CH1: %d\n", a);
}

void TIM8_Update_CH4 (unsigned short a)
{
    printf("LR CH1: %d\n", a);
}

void TIM8_Update_CH2 (unsigned short a)
{
    printf("HL CH2: %d\n", a);
}

void TIM8_Update_CH1 (unsigned short a)
{
    printf("LR CH2: %d\n", a);
}

void TIM4_Update_CH2 (unsigned short a)
{
    printf("HL CH3: %d\n", a);
}

void TIM4_Update_CH3 (unsigned short a)
{
    printf("LR CH3: %d\n", a);
}

void TIM5_Update_CH1 (unsigned short a)
{
    printf("HL CH4: %d\n", a);
}

void TIM5_Update_CH2 (unsigned short a)
{
    printf("LR CH4: %d\n", a);
}

void UpdateLed (void)
{
}

void UpdateBuzzer (void)
{
}

void UpdateRaspberryMessages (void)
{
}

void Error_SetStatus (unsigned char error, unsigned char channel)
{
    error <<= 4;
    error += channel + 1;
    printf("error: 0x%02x\n", error);
}

void Led1_On (void)
{
    // printf("Led1 -> ON\n");
}


void Led1_Off (void)
{
    // printf("Led1 -> OFF\n");    
}

unsigned char TIM1_SyncGet (void)
{
    return 0;
}

void TIM1_SyncReset (void)
{
}

unsigned char TIM1_SyncVerify (unsigned char * freq_int, unsigned char * freq_dec)
{
    return 1;
}

void TIM_Void_CH (unsigned short a)
{
}

//--- end of file ---//


