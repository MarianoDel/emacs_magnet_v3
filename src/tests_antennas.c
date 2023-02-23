//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "antennas.h"
// #include "tim.h"
// #include "dsp.h"

// helper modules
#include "tests_ok.h"
#include "tests_mock_usart.h"

#include <stdio.h>


// Types Constants and Macros --------------------------------------------------


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Functions to Test ----------------------------------------------------
void Test_Antennas_Setup (void);


// Module Auxiliary Functions --------------------------------------------------


// Tests Module Auxiliary or General Functions ---------------------------------


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    Test_Antennas_Setup ();

    return 0;
}


void Test_Antennas_Setup (void)
{
    for (int i = 0; i < 5; i++)
    {
        AntennaUpdateStates ();
    }
    
    // if (!signal_index)
    //     PrintOK();
    // else
    //     PrintERR();    
    
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


// void Test_PI_Functions (void)
// {
//     printf("Testing PI roof settings: ");

//     pi_data_obj_t pi;
//     pi.kp = 5;
//     pi.ki = 5;
//     pi.last_d = 100;
//     pi.error_z1 = 100;

//     PI_roof_Flush(&pi);
        
//     if ((pi.last_d != 0) ||
//         (pi.error_z1 != 0))
//     {
//         PrintERR();
//     }
//     else
//         PrintOK();

//     pi.kp = 5;
//     pi.ki = 5;
//     PI_roof_Flush(&pi);
        
//     pi.setpoint = 1000;        
//     for (int i = 0; i < 1000; i++)
//     {
//         pi.sample = pi.last_d;
//         PI_roof(&pi);
//     }
//     printf("pi roof last_d: %d error_z1: %d\n", pi.last_d, pi.error_z1);
    
//     printf("Testing PI roof performance: ");    
//     if ((pi.last_d != 0) ||
//         (pi.error_z1 != 1))
//     {
//         PrintERR();
//     }
//     else
//         PrintOK();
    
// }


// void Test_PI_Simul (void)
// {
//     printf("\nTest PI and simulate\n");

//     short vduty [1000] = { 0 };
//     short verror [1000] = { 0 };
//     short vsp [1000] = { 0 };

//     pi_data_obj_t pi;    
//     pi.kp = 10;
//     pi.ki = 10;
//     PI_roof_Flush(&pi);
        
//     pi.setpoint = 1000;        
//     for (int i = 0; i < 1000; i++)
//     {
//         pi.sample = pi.last_d;
//         PI_roof(&pi);

//         vduty[i] = pi.last_d;
//         verror[i] = pi.error_z1;
//         vsp[i] = pi.setpoint;
//     }
    
//     printf("pi roof last_d: %d error_z1: %d\n", pi.last_d, pi.error_z1);
    
    
//     ///////////////////////////
//     // Backup Data to a file //
//     ///////////////////////////
//     FILE * file = fopen("data.txt", "w");

//     if (file == NULL)
//     {
//         printf("data file not created!\n");
//         return;
//     }

//     Vector_Short_To_File (file, "duty", vduty, 1000);
//     Vector_Short_To_File (file, "error", verror, 1000);
//     Vector_Short_To_File (file, "setpoint", vsp, 1000);
//     printf("\nRun by hand python3 simul_outputs.py\n");

// }


// // Mocked Module Functions -----------------------------------------------------
// void TIM8_Update_CH3 (unsigned short a)
// {
//     printf("HL CH1: %d\n", a);
// }

// void TIM8_Update_CH4 (unsigned short a)
// {
//     printf("LR CH1: %d\n", a);
// }

// void TIM8_Update_CH2 (unsigned short a)
// {
//     printf("HL CH2: %d\n", a);
// }

// void TIM8_Update_CH1 (unsigned short a)
// {
//     printf("LR CH2: %d\n", a);
// }

// void TIM4_Update_CH2 (unsigned short a)
// {
//     printf("HL CH3: %d\n", a);
// }

// void TIM4_Update_CH3 (unsigned short a)
// {
//     printf("LR CH3: %d\n", a);
// }

// void TIM5_Update_CH1 (unsigned short a)
// {
//     printf("HL CH4: %d\n", a);
// }

// void TIM5_Update_CH2 (unsigned short a)
// {
//     printf("LR CH4: %d\n", a);
// }

void RPI_Send (char * a)
{
    Usart1Send(a);
}

void UART_CH1_Send (char * a)
{
    Usart2Send(a);
}

void UART_CH2_Send (char * a)
{
    Usart3Send(a);
}

void UART_CH3_Send (char * a)
{
    Uart4Send(a);
}

void UART_CH4_Send (char * a)
{
    Uart5Send(a);
}

// void UpdateLed (void)
// {
// }

// void UpdateBuzzer (void)
// {
// }

// void UpdateRaspberryMessages (void)
// {
// }

//--- end of file ---//


