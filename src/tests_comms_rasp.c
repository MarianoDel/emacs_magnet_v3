//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "comms_from_rasp.h"
#include "treatment.h"
#include "comms.h"


#include <stdio.h>
#include <string.h>
// #include <math.h>

// Types Constants and Macros --------------------------------------------------


// Externals -------------------------------------------------------------------
volatile unsigned short adc_ch [2];
volatile unsigned char usart1_have_data = 0;
unsigned short comms_messages_rpi = 0;

// Globals ---------------------------------------------------------------------
char s_test [100] = { 0 };

// Module Functions to Test ----------------------------------------------------
void Test_Comm_From_Rasp_Module (void);
void Test_Functions (void);

// Module Auxiliary Functions --------------------------------------------------
void HARD_L1_ON (void);
void HARD_L1_OFF (void);
void Usart1Send (char * s);
unsigned char ReadUsart1Buffer (char * bout, unsigned short max_len);
void BuzzerCommands(unsigned char command, unsigned char multiple);


// Tests Module Auxiliary or General Functions ---------------------------------
void PrintOK (void);
void PrintERR (void);


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    // Test_Functions ();
    Test_Comm_From_Rasp_Module();

    return 0;
}


void Test_Comm_From_Rasp_Module (void)
{
    int some_err = 0;

    //////////////////////
    // Test Signal Type //
    //////////////////////
    signal_type_t signal;    
    strcpy(s_test, "signal triangular\r\n");
    rpi_have_data = 1;
    UpdateRaspberryMessages();

    signal = TreatmentGetSignalType();
    if ((rpi_have_data != 0) || (signal != TRIANGULAR_SIGNAL))
    {
        printf("\nSignal type error: %d ", signal);
        some_err = 1;
    }

    strcpy(s_test, "signal square\r\n");
    rpi_have_data = 1;
    UpdateRaspberryMessages();

    signal = TreatmentGetSignalType();
    if ((rpi_have_data != 0) || (signal != SQUARE_SIGNAL))
    {
        printf("\nSignal type error: %d ", signal);
        some_err = 1;
    }

    strcpy(s_test, "signal sinusoidal\r\n");
    rpi_have_data = 1;
    UpdateRaspberryMessages();

    signal = TreatmentGetSignalType();
    if ((rpi_have_data != 0) || (signal != SINUSOIDAL_SIGNAL))
    {
        printf("\nSignal type error: %d ", signal);
        some_err = 1;
    }

    ////////////////
    // Test Power //
    ////////////////
    unsigned char power = 0;
    strcpy(s_test, "power 010\r\n");
    rpi_have_data = 1;
    UpdateRaspberryMessages();

    power = TreatmentGetPower();
    if ((rpi_have_data != 0) || (power != 10))
    {
        printf("\nPower error: %d ", power);
        some_err = 1;
    }

    ////////////////////
    // Test Frequency //
    ////////////////////
    unsigned char f_int = 10;
    unsigned char f_dec = 20;
    sprintf(s_test, "frequency %02d.%02d\r\n", f_int, f_dec);
    rpi_have_data = 1;
    UpdateRaspberryMessages();

    TreatmentGetFrequency(&f_int, &f_dec);
    if ((rpi_have_data != 0) || (f_int != 10) || f_dec != 20)
    {
        printf("\nFrequency error: %d.%d ", f_int, f_dec);
        some_err = 1;
    }

    ///////////////////
    // Test Channels //
    ///////////////////
    strcpy(s_test, "enable channel 1\r\n");
    rpi_have_data = 1;
    UpdateRaspberryMessages();
    strcpy(s_test, "enable channel 2\r\n");
    rpi_have_data = 1;
    UpdateRaspberryMessages();
    strcpy(s_test, "enable channel 3\r\n");
    rpi_have_data = 1;
    UpdateRaspberryMessages();

    unsigned char channels = TreatmentGetChannelsFlag ();
    if (channels != ((ENABLE_CH1_FLAG | ENABLE_CH2_FLAG | ENABLE_CH3_FLAG) & 0x0f))
    {
        printf("\nChannels error getted: %d ", channels);
        some_err = 1;
    }

    strcpy(s_test, "disable channel 1\r\n");
    rpi_have_data = 1;
    UpdateRaspberryMessages();
    strcpy(s_test, "disable channel 2\r\n");
    rpi_have_data = 1;
    UpdateRaspberryMessages();
    strcpy(s_test, "disable channel 3\r\n");
    rpi_have_data = 1;
    UpdateRaspberryMessages();

    channels = TreatmentGetChannelsFlag ();
    if (channels != 0)
    {
        printf("\nChannels error getted: %d ", channels);
        some_err = 1;
    }

    ////////////////////////////
    // Test Stretcher Up-Down //
    ////////////////////////////
    strcpy(s_test, "stretcher up\r\n");
    comms_messages_rpi = 0;
    rpi_have_data = 1;
    UpdateRaspberryMessages();

    if ((rpi_have_data != 0) || (comms_messages_rpi != COMM_STRETCHER_UP))
    {
        printf("\nUpDwn error getted: %d ", comms_messages_rpi);
        some_err = 1;
    }

    strcpy(s_test, "stretcher autoup on\r\n");
    rpi_have_data = 1;
    UpdateRaspberryMessages();

    updwn_e updwn = TreatmentGetUpDwn ();
    if ((rpi_have_data != 0) || (updwn != UPDWN_AUTO))
    {
        printf("\nUpDwn error getted: %d ", updwn);
        some_err = 1;
    }

    strcpy(s_test, "stretcher autoup off\r\n");
    rpi_have_data = 1;
    UpdateRaspberryMessages();

    updwn = TreatmentGetUpDwn ();
    if ((rpi_have_data != 0) || (updwn != UPDWN_MANUAL))
    {
        printf("\nUpDwn error getted: %d ", updwn);
        some_err = 1;
    }

    //////////////////////
    // Test Bridge Mode //
    //////////////////////
    strcpy(s_test, "goto bridge mode\r\n");
    comms_messages_rpi = 0;
    rpi_have_data = 1;
    UpdateRaspberryMessages();

    if ((rpi_have_data != 0) || (comms_messages_rpi != COMM_GOTO_BRIDGE))
    {
        printf("\nUpDwn error getted: %d ", comms_messages_rpi);
        some_err = 1;
    }

    //////////////////
    // Test Voltage //
    //////////////////
    adc_ch[0] = 3000;
    adc_ch[1] = 2500;

    strcpy(s_test, "voltage\r\n");
    rpi_have_data = 1;
    UpdateRaspberryMessages();
    
    if (rpi_have_data != 0)
    {
        printf("\nVoltage with some error ");
        some_err = 1;
    }

    //////////////////////
    // Test Hard & Soft //
    //////////////////////
    strcpy(s_test, "hard_soft\r\n");
    rpi_have_data = 1;
    UpdateRaspberryMessages();
    
    if (rpi_have_data != 0)
    {
        printf("\nVoltage with some error ");
        some_err = 1;
    }

    /////////////////
    // Test Buzzer //
    /////////////////
    strcpy(s_test, "buzzer short 3\r\n");
    rpi_have_data = 1;
    UpdateRaspberryMessages();
    
    if (rpi_have_data != 0)
    {
        printf("\nBuzzer with errors ");
        some_err = 1;
    }

    strcpy(s_test, "buzzer half 2\r\n");
    rpi_have_data = 1;
    UpdateRaspberryMessages();
    
    if (rpi_have_data != 0)
    {
        printf("\nBuzzer with errors ");
        some_err = 1;
    }

    strcpy(s_test, "buzzer long 1\r\n");
    rpi_have_data = 1;
    UpdateRaspberryMessages();
    
    if (rpi_have_data != 0)
    {
        printf("\nBuzzer with errors ");
        some_err = 1;
    }

    ////////////////////
    // Test keepalive //
    ////////////////////
    strcpy(s_test, "keepalive,\r\n");
    rpi_have_data = 1;
    UpdateRaspberryMessages();
    
    if (rpi_have_data != 0)
    {
        printf("\nNo keepalive ");
        some_err = 1;
    }

    ///////////////////
    // Test Duration //
    ///////////////////
    strcpy(s_test, "duration,100\r\n");
    rpi_have_data = 1;
    UpdateRaspberryMessages();

    unsigned short time = TreatmentGetTime ();
    if ((rpi_have_data != 0) || (time != (100*60)))
    {
        printf("\nTime error getted: %d ", time);
        some_err = 1;
    }


    ///////////////////////////
    // Test Stop Pause Start //
    ///////////////////////////
    strcpy(s_test, "stop,\r\n");
    comms_messages_rpi = 0;
    rpi_have_data = 1;
    UpdateRaspberryMessages();

    if ((rpi_have_data != 0) || (comms_messages_rpi != COMM_STOP_TREAT))
    {
        printf("\nStop error getted: %d ", comms_messages_rpi);
        some_err = 1;
    }

    strcpy(s_test, "start,\r\n");
    comms_messages_rpi = 0;
    rpi_have_data = 1;
    UpdateRaspberryMessages();

    if ((rpi_have_data != 0) || (comms_messages_rpi != COMM_START_TREAT))
    {
        printf("\nStart error getted: %d ", comms_messages_rpi);
        some_err = 1;
    }

    strcpy(s_test, "pause,\r\n");
    comms_messages_rpi = 0;
    rpi_have_data = 1;
    UpdateRaspberryMessages();

    if ((rpi_have_data != 0) || (comms_messages_rpi != COMM_PAUSE_TREAT))
    {
        printf("\nPause error getted: %d ", comms_messages_rpi);
        some_err = 1;
    }
    

    
    
    
    if (some_err)
        PrintERR();
    else
        PrintOK();
    
}

void HARD_L1_ON (void)
{
    printf("Led1 -> ON\n");
}

void HARD_L1_OFF (void)
{
    printf("Led1 -> OFF\n");
}

void Usart1Send (char * s)
{
    printf("sended: %s\n", s);
}

void BuzzerCommands(unsigned char command, unsigned char multiple)
{
    printf("buzzer cmd: %d multiple: %d\n", command, multiple);
}

unsigned char ReadUsart1Buffer (char * bout, unsigned short max_len)
{
    unsigned char len = 0;
    strcpy(bout, s_test);
    len = strlen(bout);
    return len;
}


void Test_Functions (void)
{
    printf("tested ok!\n");
    PrintOK();
}


void PrintOK (void)
{
    printf("\033[0;32m");    //green
    printf("OK\n");
    printf("\033[0m");    //reset
}


void PrintERR (void)
{
    printf("\033[0;31m");    //red
    printf("ERR\n");
    printf("\033[0m");    //reset
}
//--- end of file ---//


