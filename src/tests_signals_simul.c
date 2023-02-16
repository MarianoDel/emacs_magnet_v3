//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS_SIGNALS.C #######################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "signals.h"
#include "dsp.h"

// helper modules
#include "tests_ok.h"
#include "tests_vector_utils.h"
#include "tests_recursive_utils.h"

#include <stdio.h>
#include <math.h>

// Types Constants and Macros --------------------------------------------------
#define SIZEOF_SIGNALS    (256 * 6)    // six cycles
// #define SIZEOF_SIGNALS    512    // two cycles
// #define SIZEOF_SIGNALS    256    // only one cycle

#define CH1    0
#define CH2    1
#define CH3    2
#define CH4    3


#define IS_CH4    adc_ch[0]
#define IS_CH3    adc_ch[1]
#define IS_CH2    adc_ch[2]
#define IS_CH1    adc_ch[3]



// Externals -------------------------------------------------------------------
volatile unsigned short adc_ch [10];
volatile unsigned char timer1_seq_ready = 0;


// Globals ---------------------------------------------------------------------
short v_duty_ch1 [SIZEOF_SIGNALS] = { 0 };
short v_error_ch1 [SIZEOF_SIGNALS] = { 0 };
short v_sp_ch1 [SIZEOF_SIGNALS] = { 0 };
short v_adc_ch1 [SIZEOF_SIGNALS] = { 0 };

short v_duty_ch2 [SIZEOF_SIGNALS] = { 0 };
short v_error_ch2 [SIZEOF_SIGNALS] = { 0 };
short v_sp_ch2 [SIZEOF_SIGNALS] = { 0 };
short v_adc_ch2 [SIZEOF_SIGNALS] = { 0 };

short v_duty_ch3 [SIZEOF_SIGNALS] = { 0 };
short v_error_ch3 [SIZEOF_SIGNALS] = { 0 };
short v_sp_ch3 [SIZEOF_SIGNALS] = { 0 };
short v_adc_ch3 [SIZEOF_SIGNALS] = { 0 };

short v_duty_ch4 [SIZEOF_SIGNALS] = { 0 };
short v_error_ch4 [SIZEOF_SIGNALS] = { 0 };
short v_sp_ch4 [SIZEOF_SIGNALS] = { 0 };
short v_adc_ch4 [SIZEOF_SIGNALS] = { 0 };

recursive_filter_t plant_ch1;
recursive_filter_t plant_ch2;
recursive_filter_t plant_ch3;
recursive_filter_t plant_ch4;


// Module Functions to Test ----------------------------------------------------
// -- set recursive
void Plant_Out_Recursive_Reset (int which_channel, recursive_filter_t * f);
float Plant_Out_Recursive (recursive_filter_t * f, short duty_in);
void Plant_Step_Response (recursive_filter_t *f);


// -- tests functions
void Test_Generate_All_Channels (void);
void Test_Plant_Step_Response (void);

unsigned short Adc12BitsConvertion (float sample);
unsigned short Adc10BitsConvertion (float sample);


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    printf("Start of Analog simulations...\n");

    // printf("testing plant step response...\n");
    // printf("recursive reset on plant...\n");
    // Plant_Out_Recursive_Reset (&plant_ch1);
    // Test_Plant_Step_Response ();
    // printf("end step response test\n");
    
    printf("testing signals and simulation...\n");
    printf("recursive reset on plant...\n");
    Plant_Out_Recursive_Reset (CH1, &plant_ch1);
    Plant_Out_Recursive_Reset (CH2, &plant_ch2);
    Plant_Out_Recursive_Reset (CH3, &plant_ch3);
    Plant_Out_Recursive_Reset (CH4, &plant_ch4);    
    Test_Generate_All_Channels ();
    printf("end generation test\n");    

    printf("all simulations done!\n");
    
    return 0;
}


// Plant Output parameters vinput = 1 or vinput = 192
// Check ../control_to_output.py
// TransferFunctionDiscrete(
// array([0.62806727]),
// array([ 1.        , -0.94942247]),
// dt: 0.0006666666666666666
// )
#define B_SIZE    1
#define A_SIZE    2
// ch1 plant
float b_vector_ch1 [B_SIZE] = { 0.62806727 };
float a_vector_ch1 [A_SIZE] = { 1., -0.94942247 };
float ins_vector_ch1 [B_SIZE] = { 0.0 };
float outs_vector_ch1 [A_SIZE] = { 0.0 };
// ch2 plant
float b_vector_ch2 [B_SIZE] = { 0.62806727 };
float a_vector_ch2 [A_SIZE] = { 1., -0.94942247 };
float ins_vector_ch2 [B_SIZE] = { 0.0 };
float outs_vector_ch2 [A_SIZE] = { 0.0 };
// ch3 plant
float b_vector_ch3 [B_SIZE] = { 0.62806727 };
float a_vector_ch3 [A_SIZE] = { 1., -0.94942247 };
float ins_vector_ch3 [B_SIZE] = { 0.0 };
float outs_vector_ch3 [A_SIZE] = { 0.0 };
// ch4 plant
float b_vector_ch4 [B_SIZE] = { 0.62806727 };
float a_vector_ch4 [A_SIZE] = { 1., -0.94942247 };
float ins_vector_ch4 [B_SIZE] = { 0.0 };
float outs_vector_ch4 [A_SIZE] = { 0.0 };
void Plant_Out_Recursive_Reset (int which_channel, recursive_filter_t * f)
{
    switch (which_channel)
    {
    case CH1:
        f->b_params = b_vector_ch1;
        f->a_params = a_vector_ch1;
        f->b_size = B_SIZE;
        f->a_size = A_SIZE;
        f->last_inputs = ins_vector_ch1;
        f->last_outputs = outs_vector_ch1;    
        break;

    case CH2:
        f->b_params = b_vector_ch2;
        f->a_params = a_vector_ch2;
        f->b_size = B_SIZE;
        f->a_size = A_SIZE;
        f->last_inputs = ins_vector_ch2;
        f->last_outputs = outs_vector_ch2;    
        break;

    case CH3:
        f->b_params = b_vector_ch3;
        f->a_params = a_vector_ch3;
        f->b_size = B_SIZE;
        f->a_size = A_SIZE;
        f->last_inputs = ins_vector_ch3;
        f->last_outputs = outs_vector_ch3;    
        break;

    case CH4:
        f->b_params = b_vector_ch4;
        f->a_params = a_vector_ch4;
        f->b_size = B_SIZE;
        f->a_size = A_SIZE;
        f->last_inputs = ins_vector_ch4;
        f->last_outputs = outs_vector_ch4;    
        break;
    }
    
    Recursive_Filter_Float_Reset(f);
}


float Plant_Out_Recursive (recursive_filter_t * f, short duty)
{
    return Recursive_Filter_Float(f, (float) duty / 1000.0);
}


extern const unsigned short * p_table_inphase;
extern const unsigned short * p_table_outphase;
extern pi_data_obj_t pi_ch1;
extern pi_data_obj_t pi_ch2;
extern pi_data_obj_t pi_ch3;
extern pi_data_obj_t pi_ch4;
extern unsigned short signal_index;
void Test_Generate_All_Channels (void)
{
    
    Signals_Setup_All_Channels();

    for (int i = 0; i < (SIZEOF_SIGNALS - 1); i++)
    {
        timer1_seq_ready = 1;
        Signals_Generate_All_Channels ();

        // save ch1 data
        v_duty_ch1[i] = pi_ch1.last_d;
        v_sp_ch1[i] = pi_ch1.setpoint;
        v_error_ch1[i] = pi_ch1.setpoint - pi_ch1.sample;
        v_adc_ch1[i] = pi_ch1.sample;

        // save ch2 data
        v_duty_ch2[i] = pi_ch2.last_d;
        v_sp_ch2[i] = pi_ch2.setpoint;
        v_error_ch2[i] = pi_ch2.setpoint - pi_ch2.sample;
        v_adc_ch2[i] = pi_ch2.sample;

        // save ch3 data
        v_duty_ch3[i] = pi_ch3.last_d;
        v_sp_ch3[i] = pi_ch3.setpoint;
        v_error_ch3[i] = pi_ch3.setpoint - pi_ch3.sample;
        v_adc_ch3[i] = pi_ch3.sample;

        // save ch4 data
        v_duty_ch4[i] = pi_ch4.last_d;
        v_sp_ch4[i] = pi_ch4.setpoint;
        v_error_ch4[i] = pi_ch4.setpoint - pi_ch4.sample;
        v_adc_ch4[i] = pi_ch4.sample;
        
    }

    ///////////////////////////
    // Backup Data to a file //
    ///////////////////////////
    FILE * file = fopen("data.txt", "w");

    if (file == NULL)
    {
        printf("data file not created!\n");
        return;
    }

    Vector_Short_To_File (file, "duty1", v_duty_ch1, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "adc1", v_adc_ch1, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "error1", v_error_ch1, SIZEOF_SIGNALS);    
    Vector_Short_To_File (file, "setpoint1", v_sp_ch1, SIZEOF_SIGNALS);

    // Vector_Short_To_File (file, "duty2", v_duty_ch2, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "adc2", v_adc_ch2, SIZEOF_SIGNALS);
    // Vector_Short_To_File (file, "error2", v_error_ch2, SIZEOF_SIGNALS);    
    // Vector_Short_To_File (file, "setpoint2", v_sp_ch2, SIZEOF_SIGNALS);
    
    // Vector_Short_To_File (file, "duty4", v_duty_ch4, SIZEOF_SIGNALS);
    // Vector_Short_To_File (file, "adc4", v_adc_ch4, SIZEOF_SIGNALS);
    // Vector_Short_To_File (file, "error4", v_error_ch4, SIZEOF_SIGNALS);    
    // Vector_Short_To_File (file, "setpoint4", v_sp_ch4, SIZEOF_SIGNALS);
    
    printf("\nRun by hand python3 simul_outputs.py\n");
    
}


float v_dummy [SIZEOF_SIGNALS] = { 0 };
void Test_Plant_Step_Response (void)
{
    int length = 200;

    printf("testing only for ch1!\n");
    for (int i = 0; i < length; i++)
    {
        v_duty_ch1[i] = 950;
        v_dummy[i] = Plant_Out_Recursive (&plant_ch1, v_duty_ch1[i]);
        v_error_ch1[i] = Adc12BitsConvertion(v_dummy[i]);        
    }

    ///////////////////////////
    // Backup Data to a file //
    ///////////////////////////
    FILE * file = fopen("data.txt", "w");

    if (file == NULL)
    {
        printf("data file not created!\n");
        return;
    }

    Vector_Short_To_File (file, "duty", v_duty_ch1, length);
    Vector_Short_To_File (file, "adc_sample", v_error_ch1, length);
    Vector_Float_To_File (file, "plant_out", v_dummy, length);
    printf("\nRun by hand python3 simul_outputs.py\n");
    
}


unsigned short Adc12BitsConvertion (float sample)
{
    if (sample > 0.0001)
    {
        sample = sample / 3.3;
        sample = sample * 4095;
        
        if (sample > 4095)
            sample = 4095;
    }
    else
        sample = 0.0;

    return (unsigned short) sample;
    
}


unsigned short Adc10BitsConvertion (float sample)
{
    if (sample > 0.0001)
    {
        sample = sample / 3.3;
        sample = sample * 1023;
        
        if (sample > 1023)
            sample = 1023;
    }
    else
        sample = 0.0;

    return (unsigned short) sample;
    
}


// Mocked Module Functions -----------------------------------------------------
void TIM8_Update_CH3 (unsigned short a)
{
    // printf("HL CH1: %d\n", a);
    if (a)
    {
        float output = 0.0;
        output = Plant_Out_Recursive(&plant_ch1, a);
        // printf("input: %d output: %f ", a, output);
        IS_CH1 = Adc12BitsConvertion (output);
        // printf("sample: %d\n", IS_CH1);        
    }
}

void TIM8_Update_CH4 (unsigned short a)
{
    if (a == 0)    // no emition no adc data
    {
        IS_CH1 = 0;
        Plant_Out_Recursive_Reset (CH1, &plant_ch1);
    }
    else if (a < 950)    // regulation on negative
    {
        float output = 0.0;
        output = Plant_Out_Recursive(&plant_ch1, -a);
        // output = Plant_Out_Recursive(&plant_ch1, -a << 5);    // simulate a fast discharge     
        IS_CH1 = Adc12BitsConvertion (output);
    }
    else
    {
        // regulation by positive, do nothing in here
    }
}

void TIM8_Update_CH2 (unsigned short a)
{
    if (a)
    {
        float output = 0.0;
        output = Plant_Out_Recursive(&plant_ch2, a);
        IS_CH2 = Adc12BitsConvertion (output);
    }
    // printf("HL CH2: %d\n", a);
}

void TIM8_Update_CH1 (unsigned short a)
{
    if (a == 0)    // no emition no adc data
    {
        IS_CH2 = 0;
        Plant_Out_Recursive_Reset (CH2, &plant_ch2);
    }
    else if (a < 950)    // regulation on negative
    {
        float output = 0.0;
        // output = Plant_Out_Recursive(-a);
        output = Plant_Out_Recursive(&plant_ch2, -a << 5);    // simulate a fast discharge     
        IS_CH2 = Adc12BitsConvertion (output);
    }
    else
    {
        // regulation by positive, do nothing in here
    }
    // printf("LR CH2: %d\n", a);
}

void TIM4_Update_CH2 (unsigned short a)
{
    if (a)
    {
        float output = 0.0;
        output = Plant_Out_Recursive(&plant_ch3, a);
        IS_CH3 = Adc12BitsConvertion (output);
    }
    // printf("HL CH3: %d\n", a);
}

void TIM4_Update_CH3 (unsigned short a)
{
    if (a == 0)    // no emition no adc data
    {
        IS_CH3 = 0;
        Plant_Out_Recursive_Reset (CH3, &plant_ch3);
    }
    else if (a < 950)    // regulation on negative
    {
        float output = 0.0;
        // output = Plant_Out_Recursive(-a);
        output = Plant_Out_Recursive(&plant_ch3, -a << 5);    // simulate a fast discharge     
        IS_CH3 = Adc12BitsConvertion (output);
    }
    else
    {
        // regulation by positive, do nothing in here
    }
    // printf("LR CH3: %d\n", a);
}

void TIM5_Update_CH1 (unsigned short a)
{
    if (a)
    {
        float output = 0.0;
        output = Plant_Out_Recursive(&plant_ch4, a);
        IS_CH4 = Adc12BitsConvertion (output);
    }
    // printf("HL CH4: %d\n", a);
}

void TIM5_Update_CH2 (unsigned short a)
{
    if (a == 0)    // no emition no adc data
    {
        IS_CH4 = 0;
        Plant_Out_Recursive_Reset (CH4, &plant_ch4);
    }
    else if (a < 950)    // regulation on negative
    {
        float output = 0.0;
        // output = Plant_Out_Recursive(-a);
        output = Plant_Out_Recursive(&plant_ch4, -a << 5);    // simulate a fast discharge     
        IS_CH4 = Adc12BitsConvertion (output);
    }
    else
    {
        // regulation by positive, do nothing in here
    }    
    // printf("LR CH4: %d\n", a);
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

void ErrorSetStatus (unsigned char error, unsigned char channel)
{
    error <<= 4;
    error += channel + 1;
    printf("error: 0x%02x\n", error);
}
//--- end of file ---//


