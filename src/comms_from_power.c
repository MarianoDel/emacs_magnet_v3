//---------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### COMMS_FROM_POWER.C ############################################
//---------------------------------------------------------

/* Includes ------------------------------------------------------------------*/
// #include "stm32f10x.h"
#include "hard.h"

#include "comms_from_power.h"
#include "comms_from_rasp.h"
#include "comms.h"
#include "usart.h"
#include "treatment.h"
#include "timer.h"

#include <string.h>
#include <stdio.h>


/* Externals ------------------------------------------------------------------*/
extern volatile unsigned char power_have_data;
extern volatile unsigned short comms_timeout;
extern unsigned short comms_messages_1;
extern unsigned short comms_messages_2;
extern unsigned short comms_messages_3;





/* Globals ------------------------------------------------------------------*/
char local_power_buff [SIZEOF_RXDATA];

unsigned char it_was_feedback_ch;

unsigned char error_counter_no_comm_ch1 = 0;
unsigned char error_counter_no_comm_ch2 = 0;
unsigned char error_counter_no_comm_ch3 = 0;
unsigned char error_counter_no_treat_ch1 = 0;
unsigned char error_counter_no_treat_ch2 = 0;
unsigned char error_counter_no_treat_ch3 = 0;
comm_state_t power_comm_state = ASK_CH1;


//estos son los mismos que en stretcher_F030
const char s_ch1 [] = {"ch1"};
const char s_ch2 [] = {"ch2"};
const char s_ch3 [] = {"ch3"};
const char s_chf [] = {"chf"};
const char s_set_signal [] = {"signal"};
const char s_frequency [] = {"frequency"};
const char s_power [] = {"power"};

const char s_square_0 [] = {"square 0"};
const char s_square_120 [] = {"square 120"};
const char s_square_180 [] = {"square 180"};
const char s_square_240 [] = {"square 240"};

const char s_triangular_0 [] = {"triangular 0"};
const char s_triangular_120 [] = {"triangular 120"};
const char s_triangular_180 [] = {"triangular 180"};
const char s_triangular_240 [] = {"triangular 240"};

const char s_sinusoidal_0 [] = {"sinusoidal 0"};
const char s_sinusoidal_120 [] = {"sinusoidal 120"};
const char s_sinusoidal_180 [] = {"sinusoidal 180"};
const char s_sinusoidal_240 [] = {"sinusoidal 240"};

const char s_7_83_hz [] = {"7.83Hz"};
const char s_14_3_hz [] = {"14.3Hz"};
const char s_20_8_hz [] = {"20.8Hz"};
const char s_27_3_hz [] = {"27.3Hz"};
const char s_33_8_hz [] = {"33.8Hz"};
const char s_62_6_hz [] = {"62.6Hz"};

const char s_start_treatment [] = {"start treatment"};
const char s_stop_treatment [] = {"stop treatment"};
const char s_status [] = {"status"};
const char s_flush_errors [] = {"flush errors"};
// const char s_getall [] = {"get all conf"};


/* Module Private Functions -----------------------------------------------------------*/
static void Power_Messages (char *);
static void Power_Error_Messages (unsigned char, char *);

/* Module Exported Functions -----------------------------------------------------------*/
void UpdatePowerMessages (void)
{
    if (power_have_data)
    {
        power_have_data = 0;
        // L_ALARMA_ON;
        ReadPowerBuffer(local_power_buff, SIZEOF_RXDATA);
        Power_Messages(local_power_buff);
        // L_ALARMA_OFF;
    }
}

//TODO: revisar que haya canales habilitados antes de esto o que esta misma conteste con error
void PowerSendConf (void)
{
    char buff [128];
    signal_type_t sig;
    unsigned char ch_state;
    unsigned char ch_in_this_treatment = 0;
    char s_signal_1 [20];
    char s_signal_2 [20];
    char s_signal_3 [20];

    sig = TreatmentGetSignalType ();
    //la senial la mando defasada segun los canales que se vayan a utilizar
    //los canales que entren en tratamiento

    ch_state = TreatmentGetChannelsFlag();
    if (ch_state & 0x01)
        ch_in_this_treatment++;

    if (ch_state & 0x02)
        ch_in_this_treatment++;

    if (ch_state & 0x04)
        ch_in_this_treatment++;
    
    if (sig == SQUARE_SIGNAL)
    {
        if (ch_in_this_treatment == 3)
        {
            strcpy (s_signal_1, s_square_0);
            strcpy (s_signal_2, s_square_120);
            strcpy (s_signal_3, s_square_240);
        }

        if (ch_in_this_treatment == 2)
        {
            if ((ch_state & 0x01) && (ch_state & 0x02))
            {
                strcpy (s_signal_1, s_square_0);
                strcpy (s_signal_2, s_square_180);
            }

            if ((ch_state & 0x01) && (ch_state & 0x04))
            {
                strcpy (s_signal_1, s_square_0);
                strcpy (s_signal_3, s_square_180);
            }

            if ((ch_state & 0x02) && (ch_state & 0x04))
            {
                strcpy (s_signal_2, s_square_0);
                strcpy (s_signal_3, s_square_180);
            }
        }

        if (ch_in_this_treatment == 1)
        {
            strcpy (s_signal_1, s_square_0);
            strcpy (s_signal_2, s_square_0);
            strcpy (s_signal_3, s_square_0);            
        }
    }

    if (sig == TRIANGULAR_SIGNAL)
    {
        if (ch_in_this_treatment == 3)
        {
            strcpy (s_signal_1, s_triangular_0);
            strcpy (s_signal_2, s_triangular_120);
            strcpy (s_signal_3, s_triangular_240);
        }

        if (ch_in_this_treatment == 2)
        {
            if ((ch_state & 0x01) && (ch_state & 0x02))
            {
                strcpy (s_signal_1, s_triangular_0);
                strcpy (s_signal_2, s_triangular_180);
            }

            if ((ch_state & 0x01) && (ch_state & 0x04))
            {
                strcpy (s_signal_1, s_triangular_0);
                strcpy (s_signal_3, s_triangular_180);
            }

            if ((ch_state & 0x02) && (ch_state & 0x04))
            {
                strcpy (s_signal_2, s_triangular_0);
                strcpy (s_signal_3, s_triangular_180);
            }
        }

        if (ch_in_this_treatment == 1)
        {
            strcpy (s_signal_1, s_triangular_0);
            strcpy (s_signal_2, s_triangular_0);
            strcpy (s_signal_3, s_triangular_0);            
        }
    }

    if (sig == SINUSOIDAL_SIGNAL)
    {
        if (ch_in_this_treatment == 3)
        {
            strcpy (s_signal_1, s_sinusoidal_0);
            strcpy (s_signal_2, s_sinusoidal_120);
            strcpy (s_signal_3, s_sinusoidal_240);
        }

        if (ch_in_this_treatment == 2)
        {
            if ((ch_state & 0x01) && (ch_state & 0x02))
            {
                strcpy (s_signal_1, s_sinusoidal_0);
                strcpy (s_signal_2, s_sinusoidal_180);
            }

            if ((ch_state & 0x01) && (ch_state & 0x04))
            {
                strcpy (s_signal_1, s_sinusoidal_0);
                strcpy (s_signal_3, s_sinusoidal_180);
            }

            if ((ch_state & 0x02) && (ch_state & 0x04))
            {
                strcpy (s_signal_2, s_sinusoidal_0);
                strcpy (s_signal_3, s_sinusoidal_180);
            }
        }

        if (ch_in_this_treatment == 1)
        {
            strcpy (s_signal_1, s_sinusoidal_0);
            strcpy (s_signal_2, s_sinusoidal_0);
            strcpy (s_signal_3, s_sinusoidal_0);            
        }
    }

    //limpio buffers serie
    Power_Send("chf\n");
    Wait_ms(100);
    //tengo todas la seniales cargadas con sus fases ahora envio los canales    
    if (ch_state & 0x01)
    {
        sprintf (buff, "%s %s %s\n",s_ch1, s_set_signal, s_signal_1);
        Power_Send(buff);            
    }

    if (ch_state & 0x02)
    {
        sprintf (buff, "%s %s %s\n",s_ch2, s_set_signal, s_signal_2);
        Power_Send(buff);                        
    }

    if (ch_state & 0x04)
    {
        sprintf (buff, "%s %s %s\n",s_ch3, s_set_signal, s_signal_3);
        Power_Send(buff);
    }
    Wait_ms(100);

    sprintf (buff, "%s %s %d\n",s_chf, s_power, TreatmentGetPower());
    Power_Send(buff);
    Wait_ms(100);

    unsigned char freq_int = 0;
    unsigned char freq_dec = 0;
    TreatmentGetFrequency (&freq_int, &freq_dec);

    sprintf (buff, "%s %s %d.%02dHz\n",
             s_chf,
             s_frequency,
             freq_int,
             freq_dec);

    Power_Send(buff);
    Wait_ms(100);


}

void PowerSendStart( void )
{
    unsigned char ch;
    char buff [64];

    ch = TreatmentGetChannelsFlag();
    if (ch & ENABLE_CH1_FLAG)
    {
        sprintf (buff, "%s %s\n", s_ch1, s_start_treatment);
        Power_Send(buff);
    }

    if (ch & ENABLE_CH2_FLAG)
    {
        sprintf (buff, "%s %s\n", s_ch2, s_start_treatment);
        Power_Send(buff);
    }

    if (ch & ENABLE_CH3_FLAG)
    {
        sprintf (buff, "%s %s\n", s_ch3, s_start_treatment);
        Power_Send(buff);
    }
}

void PowerSendStop( void )
{
    char buff [64];
    sprintf (buff, "%s %s\n", s_chf, s_stop_treatment);
    Power_Send(buff);
}


static void Power_Messages (char * msg)
{
    if (!strncmp(msg, (const char *)"OK", (sizeof("OK") - 1)))
    {        
    }

    else if (!strncmp(msg, (const char *)"NOK", (sizeof("NOK") - 1)))
    {
    }

    //mensajes de ch1
    else if (!strncmp(msg, (const char *)"ch1", (sizeof("ch1") - 1)))
        Power_Error_Messages(CH1, (msg + 4));

    //mensajes de ch2
    else if (!strncmp(msg, (const char *)"ch2", (sizeof("ch2") - 1)))
        Power_Error_Messages(CH2, (msg + 4));        

    //mensajes de ch3
    else if (!strncmp(msg, (const char *)"ch3", (sizeof("ch3") - 1)))
        Power_Error_Messages(CH3, (msg + 4));
    
}

char s_manager_status [] = {"Manager status:"};
void Power_Error_Messages (unsigned char ch, char * msg)
{
    if (!strncmp(msg, (const char *)"Error: Overcurrent", (sizeof("Error: Overcurrent") - 1)))
    {
        if (ch == CH1)
            comms_messages_1 |= COMM_ERROR_OVERCURRENT;

        if (ch == CH2)
            comms_messages_2 |= COMM_ERROR_OVERCURRENT;

        if (ch == CH3)
            comms_messages_3 |= COMM_ERROR_OVERCURRENT;
    }

    else if (!strncmp(msg, (const char *)"Error: No current", (sizeof("Error: No current") - 1)))
    {
        if (ch == CH1)
            comms_messages_1 |= COMM_ERROR_NO_CURRENT;

        if (ch == CH2)
            comms_messages_2 |= COMM_ERROR_NO_CURRENT;

        if (ch == CH3)
            comms_messages_3 |= COMM_ERROR_NO_CURRENT;
    }

    else if (!strncmp(msg, (const char *)"Error: Soft Overcurrent", (sizeof("Error: Soft Overcurrent") - 1)))
    {
        if (ch == CH1)
            comms_messages_1 |= COMM_ERROR_SOFT_OVERCURRENT;

        if (ch == CH2)
            comms_messages_2 |= COMM_ERROR_SOFT_OVERCURRENT;

        if (ch == CH3)
            comms_messages_3 |= COMM_ERROR_SOFT_OVERCURRENT;        
    }

    else if (!strncmp(msg, (const char *)"Error: Overtemp", (sizeof("Error: Overtemp") - 1)))
    {
        if (ch == CH1)
            comms_messages_1 |= COMM_ERROR_OVERTEMP;

        if (ch == CH2)
            comms_messages_2 |= COMM_ERROR_OVERTEMP;

        if (ch == CH3)
            comms_messages_3 |= COMM_ERROR_OVERTEMP;        
    }

    else if (!strncmp(msg, s_manager_status, (sizeof(s_manager_status) - 1)))
    {
#ifdef USE_NO_TREATMENT_DETECT
        msg += sizeof(s_manager_status);    //ajusto y tomo en cuenta el espacio
        if (*msg != '3')    //TREATMENT_GENERATING en la potencia
        {
            if (ch == CH1)
                error_counter_no_treat_ch1++;

            if (ch == CH2)
                error_counter_no_treat_ch2++;

            if (ch == CH3)
                error_counter_no_treat_ch3++;
        }
#endif
        it_was_feedback_ch = 1;
    }
}

void PowerCommunicationStackReset (void)
{
    error_counter_no_comm_ch1 = 0;
    error_counter_no_comm_ch2 = 0;
    error_counter_no_comm_ch3 = 0;
    error_counter_no_treat_ch1 = 0;
    error_counter_no_treat_ch2 = 0;
    error_counter_no_treat_ch3 = 0;
}

void PowerCommunicationStack (void)
{
    unsigned char channels_treat = 0;

    channels_treat = TreatmentGetChannelsFlag();
    
    switch (power_comm_state)
    {
        case ASK_CH1:
            //reviso si el canal esta en tratamiento
            if (!(channels_treat & ENABLE_CH1_FLAG))
                power_comm_state = ASK_CH2;
            else
            {
                it_was_feedback_ch = 0;
                Power_Send((char *) "ch1 status\n");
                comms_timeout = POWER_COMMS_TT;
                power_comm_state++;
            }
            break;

        case WAIT_ANSWER_CH1:
            //agrandar falta de comm            
            if (!comms_timeout)
            {
                if (it_was_feedback_ch)
                    error_counter_no_comm_ch1 = 0;
                else
                    error_counter_no_comm_ch1++;

                power_comm_state++;
            }            
            break;

        case ASK_CH2:
            if (!(channels_treat & ENABLE_CH2_FLAG))
                power_comm_state = ASK_CH3;
            else
            {
                it_was_feedback_ch = 0;            
                Power_Send((char *) "ch2 status\n");
                comms_timeout = POWER_COMMS_TT;
                power_comm_state++;
            }
            break;

        case WAIT_ANSWER_CH2:
            //agrandar falta de comm
            if (!comms_timeout)
            {
                if (it_was_feedback_ch)
                    error_counter_no_comm_ch2 = 0;
                else
                    error_counter_no_comm_ch2++;

                power_comm_state++;
            }            
            break;

        case ASK_CH3:
            if (!(channels_treat & ENABLE_CH3_FLAG))
                power_comm_state = CHECK_COUNTERS;
            else
            {
                it_was_feedback_ch = 0;            
                Power_Send((char *) "ch3 status\n");
                comms_timeout = POWER_COMMS_TT;
                power_comm_state++;
            }
            break;

        case WAIT_ANSWER_CH3:
            //agrandar falta de comm
            if (!comms_timeout)
            {
                if (it_was_feedback_ch)
                    error_counter_no_comm_ch3 = 0;
                else
                    error_counter_no_comm_ch3++;

                power_comm_state++;
            }            
            break;

        case CHECK_COUNTERS:
            if (error_counter_no_comm_ch1 > MAX_NO_COMM_ERRORS)
                comms_messages_1 |= COMM_ERROR_NO_COMM;
            if (error_counter_no_comm_ch2 > MAX_NO_COMM_ERRORS)
                comms_messages_2 |= COMM_ERROR_NO_COMM;
            if (error_counter_no_comm_ch3 > MAX_NO_COMM_ERRORS)
                comms_messages_3 |= COMM_ERROR_NO_COMM;

            if (error_counter_no_treat_ch1 > MAX_NO_TREAT_ERRORS)
                comms_messages_1 |= COMM_ERROR_NO_TREATMENT;
            if (error_counter_no_treat_ch2 > MAX_NO_TREAT_ERRORS)
                comms_messages_2 |= COMM_ERROR_NO_TREATMENT;
            if (error_counter_no_treat_ch3 > MAX_NO_TREAT_ERRORS)
                comms_messages_3 |= COMM_ERROR_NO_TREATMENT;
            
            power_comm_state = ASK_CH1;
            break;
            
        default:
            power_comm_state = ASK_CH1;
            break;
    }
}

// treatment_t TreatmentGetState (void)
// {
//     return treatment_state;
// }

// resp_t TreatmentStart (void)
// {
//     if (treatment_state == TREATMENT_STANDBY)
//     {
//         if ((TreatmentAssertParams() == resp_ok) && (TreatmentGetErrorStatus() == ERROR_OK))
//         {
//             treatment_state = TREATMENT_RUNNING;
//             return resp_ok;
//         }
//     }
//     return resp_error;
// }

// void TreatmentStop (void)
// {
//     if (treatment_state != TREATMENT_STANDBY)
//         treatment_state = TREATMENT_STOPPING;
// }

// error_t TreatmentGetErrorStatus (void)
// {
//     error_t error = ERROR_OK;

//     if (global_error & ERROR_OVERTEMP_MASK)
//         error = ERROR_OVERTEMP;
//     else if (global_error & ERROR_OVERCURRENT_MASK)
//         error = ERROR_OVERCURRENT;
//     else if (global_error & ERROR_NO_CURRENT_MASK)
//         error = ERROR_NO_CURRENT;
//     else if (global_error & ERROR_SOFT_OVERCURRENT_MASK)
//         error = ERROR_SOFT_OVERCURRENT;

//     return error;
// }

// void TreatmentSetErrorStatus (error_t e)
// {
//     if (e == ERROR_FLUSH_MASK)
//         global_error = 0;
//     else
//     {
//         if (e == ERROR_OVERTEMP)
//             global_error |= ERROR_OVERTEMP_MASK;
//         if (e == ERROR_OVERCURRENT)
//             global_error |= ERROR_OVERCURRENT_MASK;
//         if (e == ERROR_SOFT_OVERCURRENT)
//             global_error |= ERROR_SOFT_OVERCURRENT_MASK;
//         if (e == ERROR_NO_CURRENT)
//             global_error |= ERROR_NO_CURRENT_MASK;
//     }
// }
// #define FlushErrorStatus() SetErrorStatus(ERROR_FLUSH_MASK)
