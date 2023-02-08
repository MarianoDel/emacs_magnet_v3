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

// helper modules
#include "tests_ok.h"
#include "tests_mock_usart.h"

#include <stdio.h>
// #include <math.h>

// Types Constants and Macros --------------------------------------------------


// Externals -------------------------------------------------------------------
// volatile unsigned short adc_ch [2];
// volatile unsigned char usart1_have_data = 0;
unsigned short comms_messages_rpi = 0;
volatile unsigned short secs_in_treatment = 0;
unsigned short secs_end_treatment = 0;
unsigned short secs_elapsed_up_to_now = 0;


// Globals ---------------------------------------------------------------------


// Module Functions to Test ----------------------------------------------------
void Test_Utils_Module (void);
void Test_Treatment_Module (void);
void Test_Functions (void);
void Test_Treatment_Manager (void);


// Module Auxiliary Functions --------------------------------------------------


// Tests Module Auxiliary or General Functions ---------------------------------


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    // Test_Functions ();
    // Test_Utils_Module();
    Test_Treatment_Module();
    Test_Treatment_Manager();    

    return 0;
}


// void Test_Utils_Module (void)
// {
//     int some_err = 0;
//     unsigned short new_number;
//     unsigned char cnt;

//     char num0 [] = "000";
//     char num1 [] = "999";

//     printf("Testing Utils Module: ");
    
//     new_number = 0;
//     cnt = 0;
//     cnt = StringIsANumber(num1, &new_number);    
//     if ((cnt != 3) || (new_number != 999))
//     {
//         printf("\nStringIs error with number %s ", num1);
//         some_err = 1;
//     }

//     new_number = 0;
//     cnt = 0;
//     cnt = StringIsANumber(num0, &new_number);    
//     if ((cnt != 3) || (new_number != 0))
//     {
//         printf("\nStringIs error with number %s ", num0);
//         some_err = 1;
//     }


//     char num2 [] = "1111,99";
//     new_number = GetValue(num2, ',');    
//     if (new_number != 1111)
//     {
//         printf("\nGetValue error with number %s ", num2);
//         some_err = 1;
//     }

//     if (some_err)
//         PrintERR();
//     else
//         PrintOK();
    
// }


void Test_Treatment_Module (void)
{
    int some_err = 0;    
    resp_t resp = resp_ok;
    
    printf("Testing Treatment Module: ");

    signal_type_t signal = SQUARE_SIGNAL;
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
    

    if (some_err)
        PrintERR();
    else
        PrintOK();
    
}


typedef enum {
    TREATMENT_STANDBY = 0,
    TREATMENT_STARTING,
    TREATMENT_RUNNING,
    TREATMENT_PAUSED,
    TREATMENT_WITH_ERRORS,
    TREATMENT_STOPPING

} treatment_t;

extern unsigned char treat_state;
void Test_Treatment_Manager (void)
{
    int some_err = 0;    
    resp_t resp = resp_ok;

    printf("-- treat in standby\n");
    for (int i = 0; i < 20; i++)
        Treatment_Manager();

    if (treat_state != TREATMENT_STANDBY)
        some_err = 1;

    printf("-- treat to running\n");    
    if (!some_err)
    {
        comms_messages_rpi |= COMM_START_TREAT;
        
        for (int i = 0; i < 20; i++)
            Treatment_Manager();

        if (treat_state != TREATMENT_RUNNING)
            some_err = 1;
        
    }

    printf("-- treat to pause\n");
    if (!some_err)
    {
        comms_messages_rpi |= COMM_PAUSE_TREAT;
        
        for (int i = 0; i < 20; i++)
        {
            Treatment_Manager();
            if (i == 5)
                comms_messages_rpi |= COMM_START_TREAT;
        }

        if (treat_state != TREATMENT_PAUSED)
            some_err = 1;
        
    }

    printf("-- treat to run again\n");    
    if (!some_err)
    {
        comms_messages_rpi |= COMM_PAUSE_TREAT;
        
        for (int i = 0; i < 20; i++)
            Treatment_Manager();

        if (treat_state != TREATMENT_RUNNING)
            some_err = 1;
        
    }

    printf("-- treat to stop\n");
    if (!some_err)
    {
        comms_messages_rpi |= COMM_STOP_TREAT;
        
        for (int i = 0; i < 20; i++)
            Treatment_Manager();

        if (treat_state != TREATMENT_STANDBY)
            some_err = 1;
        
    }

    printf("-- treat to running\n");    
    if (!some_err)
    {
        comms_messages_rpi |= COMM_START_TREAT;
        
        for (int i = 0; i < 20; i++)
        {
            Treatment_Manager();
            if (i == 5)
                comms_messages_rpi |= COMM_START_TREAT;
        }

        if (treat_state != TREATMENT_RUNNING)
            some_err = 1;
        
    }


    printf("-- treat to pause and stop\n");
    if (!some_err)
    {
        comms_messages_rpi |= COMM_PAUSE_TREAT;
        
        for (int i = 0; i < 20; i++)
        {
            Treatment_Manager();
            if (i == 5)
                comms_messages_rpi |= COMM_STOP_TREAT;
        }

        if (treat_state != TREATMENT_STANDBY)
            some_err = 1;
        
    }
    
    printf("Testing Treatment Manager: ");
    if (some_err)
        PrintERR();
    else
        PrintOK();
    
}



void Test_Functions (void)
{
    printf("tested ok!\n");
    PrintOK();
}


void RPI_Send (char * a)
{
    Usart1Send(a);
}


void ChangeLed (unsigned char number)
{
    printf("change led to %d bips\n", number);
}


void Wait_ms (unsigned short number)
{
    printf("wait for %d millis\n", number);
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

void UpdateRaspberryMessages (void)
{
}

//--- end of file ---//


