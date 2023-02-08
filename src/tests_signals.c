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
#include "tim.h"
#include "dsp.h"

// helper modules
#include "tests_ok.h"
#include "tests_mock_usart.h"
#include "tests_vector_utils.h"

#include <stdio.h>
// #include <math.h>


// Types Constants and Macros --------------------------------------------------


// Externals -------------------------------------------------------------------
volatile unsigned short adc_ch [10];
// volatile unsigned char usart1_have_data = 0;
unsigned short comms_messages_rpi = 0;
volatile unsigned char timer1_seq_ready = 0;


// Globals ---------------------------------------------------------------------


// Module Functions to Test ----------------------------------------------------
void Test_PI_Functions (void);
void Test_PI_Simul (void);
void Test_Generate_Setup (void);
void Test_Generate_All_Channels (void);


// Module Auxiliary Functions --------------------------------------------------


// Tests Module Auxiliary or General Functions ---------------------------------


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    Test_PI_Functions ();
    Test_PI_Simul ();
    Test_Generate_Setup ();
    Test_Generate_All_Channels ();

    return 0;
}


extern const unsigned short * p_table_inphase;
extern const unsigned short * p_table_outphase;
extern pi_data_obj_t pi_channel1;
extern pi_data_obj_t pi_channel2;
extern pi_data_obj_t pi_channel3;
extern pi_data_obj_t pi_channel4;
extern unsigned short signal_index;
void Test_Generate_Setup (void)
{
    int pointers_are_null = 0;
    int some_error = 0;
    
    if ((p_table_inphase == NULL) &&
        (p_table_outphase == NULL))
        pointers_are_null = 1;

    signal_index = 1;
    Signals_Setup_All_Channels ();

    printf("pointer signal assigment: ");
    if ((p_table_inphase != NULL) &&
        (p_table_outphase != NULL) &&
        (pointers_are_null))
        PrintOK();
    else
        PrintERR();

    printf("pointer index: ");
    if (!signal_index)
        PrintOK();
    else
        PrintERR();    
    
}


void Test_Generate_All_Channels (void)
{
    // short v_duty_ch1 [1000] = { 0 };
    // short v_error_ch1 [1000] = { 0 };
    // short v_sp_ch1 [1000] = { 0 };

    // short v_duty_ch2 [1000] = { 0 };
    // short v_error_ch2 [1000] = { 0 };
    // short v_sp_ch2 [1000] = { 0 };

    // short v_duty_ch3 [1000] = { 0 };
    // short v_error_ch3 [1000] = { 0 };
    // short v_sp_ch3 [1000] = { 0 };

    // short v_duty_ch4 [1000] = { 0 };
    // short v_error_ch4 [1000] = { 0 };
    // short v_sp_ch4 [1000] = { 0 };
    
    Signals_Setup_All_Channels();

    for (int i = 0; i < 511; i++)
    {
        timer1_seq_ready = 1;
        Signals_Generate_All_Channels ();
    }
}


void Test_PI_Functions (void)
{
    printf("Testing PI roof settings: ");

    pi_data_obj_t pi;
    pi.kp = 5;
    pi.ki = 5;
    pi.last_d = 100;
    pi.error_z1 = 100;

    PI_roof_Flush(&pi);
        
    if ((pi.last_d != 0) ||
        (pi.error_z1 != 0))
    {
        PrintERR();
    }
    else
        PrintOK();

    pi.kp = 5;
    pi.ki = 5;
    PI_roof_Flush(&pi);
        
    pi.setpoint = 1000;        
    for (int i = 0; i < 1000; i++)
    {
        pi.sample = pi.last_d;
        PI_roof(&pi);
    }
    printf("pi roof last_d: %d error_z1: %d\n", pi.last_d, pi.error_z1);
    
    printf("Testing PI roof performance: ");    
    if ((pi.last_d != 0) ||
        (pi.error_z1 != 1))
    {
        PrintERR();
    }
    else
        PrintOK();
    
}


void Test_PI_Simul (void)
{
    printf("\nTest PI and simulate\n");

    short vduty [1000] = { 0 };
    short verror [1000] = { 0 };
    short vsp [1000] = { 0 };

    pi_data_obj_t pi;    
    pi.kp = 10;
    pi.ki = 10;
    PI_roof_Flush(&pi);
        
    pi.setpoint = 1000;        
    for (int i = 0; i < 1000; i++)
    {
        pi.sample = pi.last_d;
        PI_roof(&pi);

        vduty[i] = pi.last_d;
        verror[i] = pi.error_z1;
        vsp[i] = pi.setpoint;
    }
    
    printf("pi roof last_d: %d error_z1: %d\n", pi.last_d, pi.error_z1);
    
    
    ///////////////////////////
    // Backup Data to a file //
    ///////////////////////////
    FILE * file = fopen("data.txt", "w");

    if (file == NULL)
    {
        printf("data file not created!\n");
        return;
    }

    Vector_Short_To_File (file, "duty", vduty, 1000);
    Vector_Short_To_File (file, "error", verror, 1000);
    Vector_Short_To_File (file, "setpoint", vsp, 1000);
    printf("\nRun by hand python3 simul_outputs.py\n");

}


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

//--- end of file ---//


