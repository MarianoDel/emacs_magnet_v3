//---------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### TREATMENT.C ###############################
//---------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "treatment.h"
#include "comms.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


// Private Types Constants and Macros ------------------------------------------
typedef enum {
    TREATMENT_STANDBY = 0,
    TREATMENT_STARTING,
    TREATMENT_RUNNING,
    TREATMENT_PAUSED,
    TREATMENT_WITH_ERRORS,
    TREATMENT_STOPPING

} treatment_t;

#define ENABLE_CHX_MASK    0x40
#define DISABLE_CHX_MASK    0x80
#define CHX_MASK        0x0F


// Externals -------------------------------------------------------------------
extern unsigned short comms_messages_rpi;


// Globals ---------------------------------------------------------------------
treatment_conf_t treatment_conf;
unsigned char global_error = 0;
treatment_t treat_state = 0;


volatile unsigned short secs_in_treatment = 0;
volatile unsigned short millis = 0;
unsigned short secs_end_treatment = 0;
unsigned short secs_elapsed_up_to_now = 0;



#define RPI_Flush_Comms (comms_messages_rpi &= ~COMM_RPI_ALL_MSG_MASK)

// Module Functions ------------------------------------------------------------
void Treatment_Manager (void)
{
    char buff [100] = { 0 };
    
    switch (treat_state)
    {
    case TREATMENT_STANDBY:
        if (comms_messages_rpi & COMM_START_TREAT)
        {
            // rpi asks to start treatment
            comms_messages_rpi &= ~COMM_START_TREAT;
            if (Treatment_AssertParams() == resp_error)
            {
                RPI_Send("ERROR on params\r\n");
            }
            // else if (check_other_antenna_conns)
            // {
            //     RPI_Send("ERROR no antenna\r\n");                
            // }
            else
            {
                RPI_Send("OK\r\n");
                // PowerSendConf();
                treat_state = TREATMENT_STARTING;
            }
        }
        RPI_Flush_Comms;
        break;

    case TREATMENT_STARTING:
        secs_end_treatment = Treatment_GetTime();
        secs_in_treatment = 1;    // a 1 here starts the timer
        secs_elapsed_up_to_now = 0;

        // show channels in treatment
        // PowerCommunicationStackReset();

        // sprintf (buff, "treat start, ch1: 0x%04x, ch2: 0x%04x, ch3: 0x%04x\r\n",
        //          comms_messages_1,
        //          comms_messages_2,
        //          comms_messages_3);
                    
        // RPI_Send(buff);

        // PowerSendStart();
        treat_state = TREATMENT_RUNNING;
        ChangeLed(LED_TREATMENT_GENERATING);
#ifdef USE_BUZZER_ON_START
        BuzzerCommands(BUZZER_HALF_CMD, 1);
#endif
        Signals_Setup_All_Channels ();    // consecutive starts and stops will always use this info
        break;

    case TREATMENT_RUNNING:
        Signals_Generate_All_Channels ();
        
        if (comms_messages_rpi & COMM_PAUSE_TREAT)
        {
            comms_messages_rpi &= ~COMM_PAUSE_TREAT;
            RPI_Send("OK\r\n");
            ChangeLed(LED_TREATMENT_PAUSED);
            secs_elapsed_up_to_now = secs_in_treatment;
            Signals_Stop_All_Channels ();
            treat_state = TREATMENT_PAUSED;            
        }

        if (comms_messages_rpi & COMM_STOP_TREAT)
        {
            comms_messages_rpi &= ~COMM_STOP_TREAT;

            // treatment finish by other timer or stop, finish_ok,
            RPI_Send("OK\r\n");
            Signals_Stop_All_Channels ();            
            treat_state = TREATMENT_STOPPING;
        }

        // someone sends me another start???
        if (comms_messages_rpi & COMM_START_TREAT)
        {
            comms_messages_rpi &= ~COMM_START_TREAT;
            RPI_Send("ERROR\r\n");
        }

        if (secs_in_treatment >= secs_end_treatment)
        {
            // treatment finish by own timer
            RPI_Send("ended ok\r\n");
#ifdef USE_BUZZER_ON_END
            BuzzerCommands(BUZZER_SHORT_CMD, 3);
#endif
            Signals_Stop_All_Channels ();
            treat_state = TREATMENT_STOPPING;
        }

        //reviso si hay algun canal con error
//         if ((comms_messages_1 & COMM_POWER_ERROR_MASK) ||
//             (comms_messages_2 & COMM_POWER_ERROR_MASK) ||
//             (comms_messages_3 & COMM_POWER_ERROR_MASK))
//         {
//             PowerSendStop();

//             LED1_ON;
//             secs_in_treatment = 0;    //con 0 freno el timer
//             sprintf (buff, "treat err, ch1: 0x%04x, ch2: 0x%04x, ch3: 0x%04x\r\n",
//                      comms_messages_1,
//                      comms_messages_2,
//                      comms_messages_3);
                    
//             RPI_Send(buff);

//             if (comms_messages_1 & COMM_POWER_ERROR_MASK)
//                 Raspberry_Report_Errors(CH1, &comms_messages_1);

//             if (comms_messages_2 & COMM_POWER_ERROR_MASK)
//                 Raspberry_Report_Errors(CH2, &comms_messages_2);

//             if (comms_messages_3 & COMM_POWER_ERROR_MASK)
//                 Raspberry_Report_Errors(CH3, &comms_messages_3);

// #ifdef USE_BUZZER_ON_ERROR
//             BuzzerCommands(BUZZER_LONG_CMD, 1);
// #endif                                
//             LED1_OFF;
//             treat_state = TREATMENT_WITH_ERRORS;
//         }
        RPI_Flush_Comms;
        break;

    case TREATMENT_PAUSED:
        // un segundo pause, me hace arrancar nuevamente
        if (comms_messages_rpi & COMM_PAUSE_TREAT)
        {
            comms_messages_rpi &= ~COMM_PAUSE_TREAT;
            secs_in_treatment = secs_elapsed_up_to_now;
            RPI_Send("OK\r\n");
            treat_state = TREATMENT_RUNNING;
            ChangeLed(LED_TREATMENT_GENERATING);
        }

        if (comms_messages_rpi & COMM_STOP_TREAT)
        {
            //estaba en pausa y me mandaron stop
            comms_messages_rpi &= ~COMM_STOP_TREAT;
            RPI_Send("OK\r\n");
            // PowerSendStop();
            treat_state = TREATMENT_STOPPING;
        }
        RPI_Flush_Comms;
        break;
                
    case TREATMENT_STOPPING:
        secs_in_treatment = 0;    //con 0 freno el timer
        // sprintf (buff, "treat end, ch1: 0x%04x, ch2: 0x%04x, ch3: 0x%04x\r\n",
        //          comms_messages_1,
        //          comms_messages_2,
        //          comms_messages_3);
                    
        RPI_Send(buff);
        treat_state = TREATMENT_STANDBY;
        ChangeLed(LED_TREATMENT_STANDBY);
        break;

    case TREATMENT_WITH_ERRORS:
        Wait_ms(1000);
        // Power_Send("chf flush errors\n");
        RPI_Send("STOP\r\n");
        Wait_ms(1000);
        RPI_Send("STOP\r\n");
        Wait_ms(1000);
        RPI_Send("Flushing errors\r\n");

        // Power_Send("chf flush errors\n");
        // comms_messages_1 &= ~COMM_POWER_ERROR_MASK;
        // comms_messages_2 &= ~COMM_POWER_ERROR_MASK;
        // comms_messages_3 &= ~COMM_POWER_ERROR_MASK;            
            
        Wait_ms(1000);
        treat_state = TREATMENT_STANDBY;
        ChangeLed(LED_TREATMENT_STANDBY);
        break;

    default:
        treat_state = TREATMENT_STANDBY;
        break;
    }            


    // check rasp comms at all time
    UpdateRaspberryMessages();

    UpdateLed();
    
    UpdateBuzzer();
}


resp_t Treatment_SetSignalType (signal_type_e a)
{
    if ((a == SQUARE_SIGNAL) ||
        (a == TRIANGULAR_SIGNAL) ||
        (a == SINUSOIDAL_SIGNAL))

        treatment_conf.treatment_signal.signal = a;
    else
        return resp_error;

    return resp_ok;
}


signal_type_e Treatment_GetSignalType (void)
{
    return treatment_conf.treatment_signal.signal;
}


resp_t Treatment_SetFrequency (unsigned char freq_int, unsigned char freq_dec)
{
    resp_t resp = resp_error;
    unsigned int calc = 1000000;
    unsigned int freq = 0;

    //el synchro es un timer con tick cada 100us
    //la cuenta para 2 decimales da 1M/(freq*100)
    freq = freq_int * 100;
    freq += freq_dec;
    freq = freq * K_SYNCHRO_ADJUST;
    freq = freq / 100;

    calc = calc / freq;
    if ((calc < TIMER_SYNCHRO_MAX) && (calc > TIMER_SYNCHRO_MIN))
    {
        treatment_conf.treatment_signal.freq_int = freq_int;
        treatment_conf.treatment_signal.freq_dec = freq_dec;
        // treatment_conf.timer_synchro = (unsigned short) calc;
        resp = resp_ok;
    }
    
    return resp;
}


void Treatment_SetChannelsFlag (unsigned char  a)
{
    if (a & ENABLE_CHX_MASK)
        treatment_conf.channels_in_treatment |= (a & CHX_MASK);

    if (a & DISABLE_CHX_MASK)
        treatment_conf.channels_in_treatment &= (~(a & CHX_MASK));
}


unsigned char Treatment_GetChannelsFlag (void)
{
    return (treatment_conf.channels_in_treatment & CHX_MASK);
}

// resp_t TreatmentChannelFlags (ch_in_treatment_t a)
// {
//     if (a == CH1_ENABLE)
//         treatment_conf.ch1_enable = 1;

//     if (a == CH2_ENABLE)
//         treatment_conf.ch2_enable = 1;

//     if (a == CH3_ENABLE)
//         treatment_conf.ch3_enable = 1;

//     if (a == CH1_DISABLE)
//         treatment_conf.ch1_enable = 0;

//     if (a == CH2_DISABLE)
//         treatment_conf.ch2_enable = 0;

//     if (a == CH3_DISABLE)
//         treatment_conf.ch3_enable = 0;
    
//     return resp_ok;
// }

void Treatment_GetFrequency (unsigned char * f_int, unsigned char * f_dec)
{
    *f_int = treatment_conf.treatment_signal.freq_int;
    *f_dec = treatment_conf.treatment_signal.freq_dec;    
}


resp_t Treatment_SetPower (unsigned char a)
{
    if (a > 100)
        treatment_conf.treatment_signal.power = 100;
    else if (a < 10)
        treatment_conf.treatment_signal.power = 10;
    else
        treatment_conf.treatment_signal.power = a;

    return resp_ok;
}


unsigned char Treatment_GetPower (void)
{
    return treatment_conf.treatment_signal.power;
}


resp_t Treatment_SetTime (unsigned char h, unsigned char m, unsigned char s)
{
    if ((h > 1) || (m > 60) || (s > 60))
        return resp_error;

    treatment_conf.treatment_time = h * 3600;
    treatment_conf.treatment_time += m * 60;
    treatment_conf.treatment_time += s;
    
    return resp_ok;
}


resp_t Treatment_SetTimeinMinutes (unsigned short m)
{
    if (m > 120)
        return resp_error;

    treatment_conf.treatment_time = m * 60;
    
    return resp_ok;
}


unsigned short Treatment_GetTime (void)
{
    return treatment_conf.treatment_time;
}


void Treatment_GetAllConf (char * tosend)
{
    char buf[30];

    // Selected signal
    sprintf(buf, "signal: %d\n", treatment_conf.treatment_signal.signal);
    strcpy(tosend, buf);

    // Selected Freq
    sprintf(buf, "freq: %d.%02dHz\n",
            treatment_conf.treatment_signal.freq_int,
            treatment_conf.treatment_signal.freq_dec);
    strcat(tosend, buf);

    // Selected power
    sprintf(buf, "power: %d\n", treatment_conf.treatment_signal.power);
    strcat(tosend, buf);

    // Treatment duration
    sprintf(buf, "time in s: %d\n", treatment_conf.treatment_time);
    strcat(tosend, buf);

    // Active channels
    unsigned char c1 = 0;
    unsigned char c2 = 0;
    unsigned char c3 = 0;

    if (treatment_conf.channels_in_treatment & ENABLE_CH1_FLAG)
        c1 = 1;

    if (treatment_conf.channels_in_treatment & ENABLE_CH2_FLAG)
        c2 = 1;

    if (treatment_conf.channels_in_treatment & ENABLE_CH3_FLAG)
        c3 = 1;
    
    sprintf(buf, "in treat ch1:%d ch2:%d ch3:%d\n", c1, c2, c3);
    strcat(tosend, buf);

    //Timer for sync
    unsigned short tsync = 0;
    // tsync = treatment_conf.timer_synchro / 10;
    sprintf(buf, "synchro: %dms\n", tsync);
    strcat(tosend, buf);
}


//verifica que se cumplan con todos los parametros para poder enviar una senial coherente
resp_t Treatment_AssertParams (void)
{
    resp_t resp = resp_error;

    if ((treatment_conf.treatment_signal.power > 100) || (treatment_conf.treatment_signal.power < 10))
        return resp;

    if ((treatment_conf.treatment_signal.freq_dec > 99) ||
        (treatment_conf.treatment_signal.freq_int < FREQ_MIN_ALLOWED) ||
        (treatment_conf.treatment_signal.freq_int > FREQ_MAX_ALLOWED))
        return resp;

    if ((treatment_conf.treatment_signal.signal != SQUARE_SIGNAL) &&
        (treatment_conf.treatment_signal.signal != TRIANGULAR_SIGNAL) &&
        (treatment_conf.treatment_signal.signal != SINUSOIDAL_SIGNAL))
        return resp;

    if (!(treatment_conf.channels_in_treatment & CHX_MASK))
        return resp;

    return resp_ok;
}


// treatment_t Treatment_GetState (void)
// {
//     return treat_state;
// }


void Treatment_Timeouts (void)
{
    if (secs_in_treatment)
    {
        if (millis < (1000 - 1)) 
            millis++;
        else
        {
            secs_in_treatment++;
            millis = 0;
        }
    }    
}


//--- end of file ---//
