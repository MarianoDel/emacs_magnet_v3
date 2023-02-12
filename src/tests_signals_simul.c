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
#define SIZEOF_SIGNALS    512

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



// Module Functions to Test ----------------------------------------------------
// -- set recursive
void Plant_Out_Recursive_Reset (void);
float Plant_Out_Recursive (short duty_in);
void Plant_Step_Response (void);

// void Plant_Step_Response_Duty (void);

// -- tests functions
void Test_Generate_All_Channels (void);
void Test_Plant_Step_Response (void);

// void TestGenSignalVoltage (void);
// void TestGenSignalBipolar (void);
// void TestSignalCloseLoop (void);
// void TestSignalSinus (void);
// void TestSignalPreDistorted (void);
// void TestStepDCM (void);
// void TestStepCCM (void);
// void TestInputToOutput (void);
// void TestRecursiveDump (void);
// void TestPRController (void);

// void Filter_Step_Response (void);
unsigned short Adc12BitsConvertion (float sample);
unsigned short Adc10BitsConvertion (float sample);


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    printf("Start of Analog simulations...\n");

    // printf("testing plant step response...\n");
    // printf("recursive reset on plant...\n");
    // Plant_Out_Recursive_Reset ();
    // Test_Plant_Step_Response ();
    // printf("end step response test\n");
    
    printf("testing signals and simulation...\n");
    printf("recursive reset on plant...\n");
    Plant_Out_Recursive_Reset ();
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
float b_vector [B_SIZE] = { 0.62806727 };
float a_vector [A_SIZE] = { 1., -0.94942247 };
float ins_vector [B_SIZE] = { 0.0 };
float outs_vector [A_SIZE] = { 0.0 };
recursive_filter_t filter_t;
void Plant_Out_Recursive_Reset (void)
{
    filter_t.b_params = b_vector;
    filter_t.a_params = a_vector;
    filter_t.b_size = B_SIZE;
    filter_t.a_size = A_SIZE;
    filter_t.last_inputs = ins_vector;
    filter_t.last_outputs = outs_vector;    
    Recursive_Filter_Float_Reset(&filter_t);
}


float Plant_Out_Recursive (short duty)
{
    return Recursive_Filter_Float(&filter_t, (float) duty / 1000.0);
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

    Vector_Short_To_File (file, "duty", v_duty_ch1, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "adc", v_adc_ch1, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "error", v_error_ch1, SIZEOF_SIGNALS);    
    Vector_Short_To_File (file, "setpoint", v_sp_ch1, SIZEOF_SIGNALS);
    printf("\nRun by hand python3 simul_outputs.py\n");
    
}


float v_dummy [SIZEOF_SIGNALS] = { 0 };
void Test_Plant_Step_Response (void)
{
    int length = 200;
    
    for (int i = 0; i < length; i++)
    {
        v_duty_ch1[i] = 950;
        v_dummy[i] = Plant_Out_Recursive (v_duty_ch1[i]);
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

// void TestSignalSinus (void)
// {
//     float calc = 0.0;
//     // int filter_cntr = 0;

//     Plant_Out_Recursive_Reset();
    
//     for (int j = 0; j < HOW_MANY_CYCLES; j++)
//     {
//         for (int i = 0; i < SIZEOF_SIGNAL; i++)
//         {
//             calc = sin (3.1415 * i / SIZEOF_SIGNAL);
//             calc = calc * 311;
//             vline[i + (j * SIZEOF_SIGNAL)] = calc;
//             vinput[i + (j * SIZEOF_SIGNAL)] = 350;
//         }
//     }

//     GenSignalSinusDutySet(100);

//     gen_signal_e sig_state = SIGNAL_RUNNING;
//     unsigned short duty = 0;
//     unsigned short isense = 0;
//     unsigned short ki_multiplier = KI_SIGNAL_PEAK_MULTIPLIER;    
//     for (int j = 0; j < HOW_MANY_CYCLES; j++)
//     {
//         GenSignalSinusReset();
//         sig_state = SIGNAL_RUNNING;
//         duty = 0;
//         isense = 0;
        
//         for (int i = 0; i < SIZEOF_SIGNAL; i++)
//         {
//             isense = last_output;
//             sig_state = GenSignalSinus(isense, ki_multiplier, &duty);
//             if (sig_state == SIGNAL_RUNNING)
//                 HIGH_LEFT(duty);
//             else
//                 HIGH_LEFT(0);
//         }

//         // if (filter_cntr > 50)
//         // {
//         //     filter_cntr = 0;
//         //     GenSignalSinusApplyFilter ();
//         // }
//         // else
//         //     filter_cntr++;
//     }

//     unsigned short reference [VECTOR_LENGTH] = { 0 };
//     unsigned int ref_calc = 0;
//     for (int j = 0; j < HOW_MANY_CYCLES; j++)
//     {
//         for (int i = 0; i < SIZEOF_SIGNAL; i++)
//         {
//             ref_calc = sin_half_cycle[i] * ki_multiplier;
//             ref_calc = ref_calc >> 12;
//             reference[i + (j * SIZEOF_SIGNAL)] = (unsigned short) ref_calc;
//         }
//     }

//     // ShowVectorUShort("\nVector reference:\n", reference, VECTOR_LENGTH);
//     // ShowVectorUShort("\nVector voltage input:\n", vinput, VECTOR_LENGTH);
//     // ShowVectorUShort("\nVector duty_high_left:\n", duty_high_left, VECTOR_LENGTH);
//     // ShowVectorUShort("\nVector duty_high_right:\n", duty_high_right, VECTOR_LENGTH);

//     // ShowVectorFloat("\nVector vinput_applied:\n", vinput_applied, VECTOR_LENGTH);
//     // ShowVectorFloat("\nVector plant output:\n", voutput, VECTOR_LENGTH);

//     // ShowVectorUShort("\nVector plant output ADC:\n", voutput_adc, VECTOR_LENGTH);

//     int error [VECTOR_LENGTH] = { 0 };
//     for (int i = 0; i < VECTOR_LENGTH; i++)
//         error[i] = reference[i] - voutput_adc[i];

//     // ShowVectorInt("\nPlant output error:\n", error, VECTOR_LENGTH);
//     // ShowVectorUShort("\nVector reference:\n", reference, SIZEOF_SIGNAL);

//     ///////////////////////////
//     // Backup Data to a file //
//     ///////////////////////////
//     FILE * file = fopen("data.txt", "w");

//     if (file == NULL)
//     {
//         printf("data file not created!\n");
//         return;
//     }

//     Vector_UShort_To_File(file, "reference", reference, VECTOR_LENGTH);
//     // Vector_UShort_To_File(file, "vinput", vinput, VECTOR_LENGTH);    
//     Vector_UShort_To_File(file, "duty_high_left", duty_high_left, VECTOR_LENGTH);

//     Vector_Float_To_File(file, "vinput applied", vinput_applied, VECTOR_LENGTH);
//     Vector_Float_To_File(file, "voutput getted", voutput, VECTOR_LENGTH);    

//     Vector_UShort_To_File(file, "voutput_adc", voutput_adc, VECTOR_LENGTH);

//     printf("\nRun by hand python3 simul_sinus_filter.py\n");    
    
// }



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
        output = Plant_Out_Recursive(a);
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
        Plant_Out_Recursive_Reset ();        
    }
    else if (a < 950)    // regulation on negative
    {
        float output = 0.0;
        // output = Plant_Out_Recursive(-a);
        output = Plant_Out_Recursive(-a << 5);    // simulate a fast discharge     
        IS_CH1 = Adc12BitsConvertion (output);
    }
    else
    {
        // regulation by positive, do nothing in here
    }
}

void TIM8_Update_CH2 (unsigned short a)
{
    // printf("HL CH2: %d\n", a);
    // float output = 0.0;
    // output = Plant_Out_Recursive(a);

    // IS_CH2 = Adc12BitsConvertion (output);    
}

void TIM8_Update_CH1 (unsigned short a)
{
    // printf("LR CH2: %d\n", a);
}

void TIM4_Update_CH2 (unsigned short a)
{
    // printf("HL CH3: %d\n", a);
    // float output = 0.0;
    // output = Plant_Out_Recursive(a);

    // IS_CH1 = Adc12BitsConvertion (output);    
}

void TIM4_Update_CH3 (unsigned short a)
{
    // printf("LR CH3: %d\n", a);
}

void TIM5_Update_CH1 (unsigned short a)
{
    // printf("HL CH4: %d\n", a);
    // float output = 0.0;
    // output = Plant_Out_Recursive(a);

    // IS_CH1 = Adc12BitsConvertion (output);    
}

void TIM5_Update_CH2 (unsigned short a)
{
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

//--- end of file ---//


