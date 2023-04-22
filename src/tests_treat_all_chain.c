//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "treatment.h"
#include "comms.h"
#include "antennas_defs.h"

// helper modules
#include "tests_ok.h"
#include "tests_mock_usart.h"

#include <stdio.h>
// #include <math.h>

// Types Constants and Macros --------------------------------------------------
//ESTADOS DEL BUZZER copied from hard.h
typedef enum
{    
    BUZZER_INIT = 0,
    BUZZER_TO_STOP,

    BUZZER_MULTIPLE_LONG,
    BUZZER_MULTIPLE_LONGA,
    BUZZER_MULTIPLE_LONGB,

    BUZZER_MULTIPLE_HALF,
    BUZZER_MULTIPLE_HALFA,
    BUZZER_MULTIPLE_HALFB,

    BUZZER_MULTIPLE_SHORT,
    BUZZER_MULTIPLE_SHORTA,
    BUZZER_MULTIPLE_SHORTB
    
} buzzer_state_t;

//COMANDOS DEL BUZZER	(tienen que ser los del estado de arriba)
// copied from hard.h
#define BUZZER_STOP_CMD		BUZZER_TO_STOP
#define BUZZER_LONG_CMD		BUZZER_MULTIPLE_LONG
#define BUZZER_HALF_CMD		BUZZER_MULTIPLE_HALF
#define BUZZER_SHORT_CMD	BUZZER_MULTIPLE_SHORT


// Externals -------------------------------------------------------------------
volatile unsigned short adc_ch [2];
// volatile unsigned char usart1_have_data = 0;
unsigned short comms_messages_rpi = 0;
extern volatile unsigned short secs_in_treatment;
extern unsigned short secs_end_treatment;
extern unsigned short secs_elapsed_up_to_now;


// Globals ---------------------------------------------------------------------
int antenna_in_this_treatment = 0;


// Module Functions to Test ----------------------------------------------------
void Test_Treatment_All_Chain (void);


// Module Auxiliary Functions --------------------------------------------------


// Tests Module Auxiliary or General Functions ---------------------------------


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{

    Test_Treatment_All_Chain();

    return 0;
}


void Test_Treatment_Module_Settings (void)
{
    int some_err = 0;    
    resp_e resp = resp_ok;
    
    printf("\n-- Testing Treatment Module Settings --\n");

    signal_type_e signal = SQUARE_SIGNAL;
    resp = Treatment_SetSignalType (signal);
    signal = Treatment_GetSignalType();
    if ((resp != resp_ok) || (signal != SQUARE_SIGNAL))
    {
        printf("\nTreatSignal error with %d ", signal);
        some_err = 1;
    }

    signal = TRIANGULAR_SIGNAL;
    resp = Treatment_SetSignalType (signal);
    signal = Treatment_GetSignalType();
    if ((resp != resp_ok) || (signal != TRIANGULAR_SIGNAL))
    {
        printf("\nTreatSignal error with %d ", signal);
        some_err = 1;
    }

    signal = SINUSOIDAL_SIGNAL;
    resp = Treatment_SetSignalType (signal);
    signal = Treatment_GetSignalType();
    if ((resp != resp_ok) || (signal != SINUSOIDAL_SIGNAL))
    {
        printf("\nTreatSignal error with %d ", signal);
        some_err = 1;
    }

    signal = 10;
    resp = Treatment_SetSignalType (signal);
    if (resp != resp_error)
    {
        printf("\nTreatSignal error with %d ", signal);
        some_err = 1;
    }

    unsigned char f_int = 10;
    unsigned char f_dec = 10;
    resp = Treatment_SetFrequency (f_int, f_dec);
    Treatment_GetFrequency (&f_int, &f_dec);
    if ((resp != resp_ok) || (f_int != 10) || (f_dec != 10))
    {
        printf("\nTreatFreq error with %d.%d ", f_int, f_dec);
        some_err = 1;
    }

    f_int = 100;
    f_dec = 10;
    resp = Treatment_SetFrequency (f_int, f_dec);
    if (resp != resp_error)
    {
        printf("\nTreatFreq error with %d.%d ", f_int, f_dec);
        some_err = 1;
    }

    unsigned char power = 100;
    resp = Treatment_SetPower (power);
    power = Treatment_GetPower ();

    if ((resp != resp_ok) || (power != 100))
    {
        printf("\nTreatPower error with %d ", power);
         some_err = 1;
    }

    power = 1;
    resp = Treatment_SetPower (power);
    power = Treatment_GetPower ();
    if ((resp != resp_ok) || (power != 10))
    {
        printf("\nTreatPower error with %d ", power);
        some_err = 1;
    }

    power = 120;
    resp = Treatment_SetPower (power);
    power = Treatment_GetPower ();
    if ((resp != resp_ok) || (power != 100))
    {
        printf("\nTreatPower error with %d ", power);
        some_err = 1;
    }

    unsigned char minutes = 100;
    unsigned short secs = 0;
    resp = Treatment_SetTimeinMinutes (minutes);
    secs = Treatment_GetTime ();
    if ((resp != resp_ok) || (secs != (minutes * 60)))
    {
        printf("\nTreatTime error with %d ", minutes);
        some_err = 1;
    }

    minutes = 121;
    resp = Treatment_SetTimeinMinutes (minutes);
    if (resp != resp_error)
    {
        printf("\nTreatTime error with %d ", minutes);
        some_err = 1;
    }

    unsigned char channels_a = 0;
    unsigned char channels_b = 0;    
    channels_a |= ENABLE_CH1_FLAG | ENABLE_CH2_FLAG | ENABLE_CH3_FLAG;
    Treatment_SetChannelsFlag (channels_a);
    channels_b = Treatment_GetChannelsFlag ();
    if ((channels_a & 0x0f) != channels_b)
    {
        printf("\nTreatChannels error with setted: %d getted: %d ", channels_a, channels_b);
        some_err = 1;
    }

    channels_a = DISABLE_CH1_FLAG;
    Treatment_SetChannelsFlag (channels_a);
    channels_b = Treatment_GetChannelsFlag ();
    if (channels_b != ((ENABLE_CH2_FLAG | ENABLE_CH3_FLAG) & 0x0f))
    {
        printf("\nTreatChannels error with setted: %d getted: %d ", channels_a, channels_b);
        some_err = 1;
    }
    

    char all_conf [250];
    Treatment_GetAllConf(all_conf);
    printf("-- get all conf\n");
    printf("%s", all_conf);
    
    printf("Testing Treatment Module Settings: ");
    if (some_err)
        PrintERR();
    else
        PrintOK();
    
}


typedef enum {
    TREATMENT_STANDBY = 0,
    TREATMENT_CHECK_ANTENNAS_CONNECTED,
    TREATMENT_STARTING,    
    TREATMENT_RUNNING,
    TREATMENT_PAUSED,
    TREATMENT_WITH_ERRORS,
    TREATMENT_STOPPING

} treatment_t;


extern unsigned char treat_state;
extern treatment_conf_t treatment_conf;
void Test_Treatment_Manager_Bad_Params (void)
{
    int some_err = 0;    
    resp_e resp = resp_ok;

    antenna_in_this_treatment = 0;
    printf("\n-- Testing Manager Bad Params --\n");    
    printf("-- treat in standby\n");
    for (int i = 0; i < 20; i++)
        Treatment_Manager();

    if (treat_state != TREATMENT_STANDBY)
        some_err = 1;


    treatment_conf.treatment_signal.signal = 5;
    if (!some_err)
    {
        printf("-- treat to running\n");        
        comms_messages_rpi |= COMM_START_TREAT;
        
        for (int i = 0; i < 20; i++)
        {
            // printf("running loop: %d treat_state: %d\n", i, treat_state);
            Treatment_Manager();
        }

        if (treat_state != TREATMENT_STANDBY)
            some_err = 1;
        
    }
    
    printf("Testing Treatment Bad Params: ");
    if (some_err)
        PrintERR();
    else
        PrintOK();
    
}


void Test_Treatment_All_Chain (void)
{
    int some_err = 0;    
    resp_e resp = resp_ok;

    antenna_in_this_treatment = 0;
    printf("\n-- Testing Treatment Manager All Chain --\n");    
    printf("-- treat in standby\n");
    for (int i = 0; i < 20; i++)
        Treatment_Manager();

    if (treat_state != TREATMENT_STANDBY)
        some_err = 1;

    printf("-- settings treatment params...\n");
    for (int i = 0; i < 20; i++)
    {
        Treatment_Manager();
        if (i == 0)
        {
            printf("-- set signal\n");
            Usart1FillRxBuffer("signal square\r\n");
        }

        if (i == 5)
        {
            printf("-- set power\n");
            Usart1FillRxBuffer("power 100\r\n");
        }

        if (i == 10)
        {
            printf("-- set frequency\n");
            Usart1FillRxBuffer("frequency 20.00\r\n");
        }

        if (i == 15)
        {
            printf("-- set time\n");
            Usart1FillRxBuffer("duration,120\r\n");
        }
    }
        
    if (treat_state != TREATMENT_STANDBY)
        some_err = 1;
    
    
    if (!some_err)
    {
        printf("-- treat to running\n");        
        
        for (int i = 0; i < 20; i++)
        {
            Treatment_Manager();
            if (i == 0)
            {
                printf("-- set to run\n");
                Usart1FillRxBuffer("start,\r\n");
            }
        }

        if (treat_state != TREATMENT_RUNNING)
            some_err = 1;
        
    }
    
    printf("Testing Treatment All Chain: ");
    if (some_err)
        PrintERR();
    else
        PrintOK();
    
}




// Module Mocked Functions -----------------------------------------------------
void RPI_Send (char * a)
{
    Usart1Send(a);
}


void HARD_L1_ON (void)
{
    printf("Led1 -> ON\n");
}


void HARD_L1_OFF (void)
{
    printf("Led1 -> OFF\n");
}

void ChangeLed (unsigned char number)
{
    printf("change led to %d bips\n", number);
}


void Wait_ms (unsigned short number)
{
    printf("wait for %d millis\n", number);
}


void AntennaSendKnowInfoWithTimer (void)
{
}


void BuzzerCommands (unsigned char cmd, unsigned char number)
{
    switch (cmd)
    {
    case BUZZER_SHORT_CMD:
        printf("buzzer %d bips short\n", number);
        break;

    case BUZZER_HALF_CMD:
        printf("buzzer %d bips half\n", number);        
        break;

    case BUZZER_LONG_CMD:
        printf("buzzer %d bips long\n", number);        
        break;

    default:
        printf("buzzer %d bips UNKNOW\n", number);        
        break;
           
    }
}


void UpdateLed (void)
{
}

void UpdateBuzzer (void)
{
}

// void UpdateRaspberryMessages (void)
// {
// }

#define STOP_SIGNAL    1
#define SETUP_SIGNAL    2
#define GENERATE_SIGNAL    3
int last_signal_state = 0;
void Signals_Stop_All_Channels (void)
{
    if (last_signal_state != STOP_SIGNAL)
    {
        printf("stopping all channels\n");
        last_signal_state = STOP_SIGNAL;
    }
}


void Signals_Setup_All_Channels (void)
{
    if (last_signal_state != SETUP_SIGNAL)
    {
        printf("setup in all channels\n");
        last_signal_state = SETUP_SIGNAL;
    }
}

void Signals_Generate_All_Channels (void)
{
    if (last_signal_state != GENERATE_SIGNAL)
    {
        printf("generate all channels\n");
        last_signal_state = GENERATE_SIGNAL;
    }
}


unsigned char AntennaVerifyForTreatment (unsigned char ch)
{
    printf("antenna verify for treatment on ch%d -- ", ch + 1);
    
    if (antenna_in_this_treatment & (1 << ch))
    {
        printf("finded\n");
        return 1;
    }

    printf("not present\n");
    return 0;    
}


void Signals_Set_Reset_Channel_For_Treatment (unsigned char which_channel, unsigned char state)
{
    if (state)
        printf("in signals ch%d set for treatment\n", which_channel + 1);
    else
        printf("in signals ch%d not in this treatment\n", which_channel + 1);
}


void AntennaGetParamsStruct (unsigned char ch, antenna_st *ant)
{
    printf("get antenna info for ch%d\n", ch + 1);
}

void Signals_Set_Channel_PI_Parameters (unsigned char which_channel, antenna_st * ant)
{
    printf("calculate pi params for ch%d\n", which_channel + 1);
}
//--- end of file ---//


