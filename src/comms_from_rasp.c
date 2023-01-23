//---------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### COMMS_FROM_RASP.C ############################################
//---------------------------------------------------------

// Includes --------------------------------------------------------------------
// #include "stm32f10x.h"
#include "hard.h"
#include "adc.h"

#include "comms_from_rasp.h"
#include "comms.h"
#include "usart.h"
#include "treatment.h"
#include "utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>



// Externals -------------------------------------------------------------------
extern volatile unsigned char rpi_have_data;
extern unsigned short comms_messages_rpi;
extern volatile unsigned short adc_ch [];



// Globals ---------------------------------------------------------------------
char local_rasp_buff [SIZEOF_RXDATA];

const char s_getall [] = {"get all conf"};
const char s_buzzer_short [] = {"buzzer short"};
const char s_buzzer_half [] = {"buzzer half"};
const char s_buzzer_long [] = {"buzzer long"};
char s_ok [] = {"OK\r\n"};
char s_nok [] = {"ERROR\r\n"};
    

// Module Private Functions ----------------------------------------------------
static void Raspberry_Messages (char *);
static void SendAllConf (void);
static void SendStatus (void);


// Module Functions ------------------------------------------------------------
void UpdateRaspberryMessages (void)
{
    if (rpi_have_data)
    {
        rpi_have_data = 0;
        HARD_L1_ON();
        ReadRPIBuffer(local_rasp_buff, SIZEOF_RXDATA);
        Raspberry_Messages(local_rasp_buff);
        HARD_L1_OFF();
    }
}


static void Raspberry_Messages (char * msg)
{
    resp_t resp = resp_ok;

    const char s_frequency [] = {"frequency"};
    unsigned short new_freq_int = 0;
    unsigned short new_freq_dec = 0;
    unsigned char decimales = 0;


    //mensajes nuevos
    if (!strncmp(msg, (const char *)"signal triangular", (sizeof("signal triangular") - 1)))
    {
        TreatmentSetSignalType(TRIANGULAR_SIGNAL);
        RPI_Send(s_ok);
    }

    else if (!strncmp(msg, (const char *)"signal square", (sizeof("signal square") - 1)))
    {
        TreatmentSetSignalType(SQUARE_SIGNAL);
        RPI_Send(s_ok);
    }

    else if (!strncmp(msg, (const char *)"signal sinusoidal", (sizeof("signal sinusoidal") - 1)))
    {
        TreatmentSetSignalType(SINUSOIDAL_SIGNAL);
        RPI_Send(s_ok);
    }

    else if (!strncmp(msg, (const char *)"power", (sizeof("power") - 1)))
    {
        char num [3];
        unsigned short power;

        resp = resp_error;

        if (strlen(msg) >= 9)
        {
            strncpy(num, (msg + 6), 3);
            power = atoi(num);

            if (power <= 100)
            {
                if (TreatmentSetPower ((unsigned char) power) == resp_ok)
                {
                    resp = resp_ok;
                    comms_messages_rpi |= COMM_CONF_CHANGE;
                }
            }
        }
            
        if (resp == resp_ok)
            RPI_Send(s_ok);
        else
            RPI_Send(s_nok);
    }

    //-- Frequency Setting
    else if (!strncmp(msg, s_frequency, sizeof(s_frequency) - 1))
    {
        // char to_send [64];
        
        msg += sizeof(s_frequency);    //normalizo al payload, hay un espacio

        //lo que viene es E.DD o EE.DD, siempre 2 posiciones decimales
        decimales = StringIsANumber(msg, &new_freq_int);
        // sprintf(to_send, "dec: %d freq: %d\n", decimales, new_freq_int);
        // RPI_Send(to_send);

        if ((decimales) && (decimales < 3))
        {
            msg += decimales + 1;    //normalizo con el punto
            decimales = StringIsANumber(msg, &new_freq_dec);

            if ((decimales > 1) && (decimales < 3))
            {
                resp = TreatmentSetFrequency ((unsigned char) new_freq_int, (unsigned char) new_freq_dec);
                if (resp == resp_ok)
                    comms_messages_rpi |= COMM_CONF_CHANGE;                
            }
        }

        if (resp == resp_ok)
            RPI_Send(s_ok);
        else
            RPI_Send(s_nok);
    }

    else if (!strncmp(msg, (const char *)"enable channel ", (sizeof("enable channel ") - 1)))
    {
        if (*(msg + 15) == '1')
        {
            TreatmentSetChannelsFlag(ENABLE_CH1_FLAG);
            RPI_Send(s_ok);
        }

        else if (*(msg + 15) == '2')
        {
            TreatmentSetChannelsFlag(ENABLE_CH2_FLAG);
            RPI_Send(s_ok);
        }

        else if (*(msg + 15) == '3')
        {
            TreatmentSetChannelsFlag(ENABLE_CH3_FLAG);
            RPI_Send(s_ok);
        }

        else
            RPI_Send(s_nok);

        
    }

    else if (!strncmp(msg, (const char *)"disable channel ", (sizeof("disable channel ") - 1)))
    {
        if (*(msg + 16) == '1')
        {
            TreatmentSetChannelsFlag(DISABLE_CH1_FLAG);
            RPI_Send(s_ok);
        }

        else if (*(msg + 16) == '2')
        {
            TreatmentSetChannelsFlag(DISABLE_CH2_FLAG);
            RPI_Send(s_ok);
        }

        else if (*(msg + 16) == '3')
        {
            TreatmentSetChannelsFlag(DISABLE_CH3_FLAG);
            RPI_Send(s_ok);
        }

        else
            RPI_Send(s_nok);
            
    }

    else if (!strncmp(msg, (const char *)"stretcher up", (sizeof("stretcher up") - 1)))
    {
        comms_messages_rpi |= COMM_STRETCHER_UP;
        RPI_Send(s_ok);        
    }

    else if (!strncmp(msg,
                      (const char *)"stretcher autoup on",
                      (sizeof("stretcher autoup on") - 1)))
    {
        TreatmentSetUpDwn(UPDWN_AUTO);
        RPI_Send(s_ok);        
    }

    else if (!strncmp(msg,
                      (const char *)"stretcher autoup off",
                      (sizeof("stretcher autoup off") - 1)))
    {
        TreatmentSetUpDwn(UPDWN_MANUAL);
        RPI_Send(s_ok);        
    }
    
    else if (!strncmp(msg, "goto bridge mode", sizeof("goto bridge mode") - 1))
    {
        comms_messages_rpi |= COMM_GOTO_BRIDGE;
        RPI_Send((char *)"Going to Bridge Mode...\r\n");
    }

    else if (!strncmp(msg, "voltage", sizeof("voltage") - 1))
    {
        char to_send [64];
        float fcalc = 1.0;
        short volt_int, volt_dec;

        fcalc = Sense_200V;
        fcalc = fcalc * K_200V;
        volt_int = (short) fcalc;
        fcalc = fcalc - volt_int;
        fcalc = fcalc * 10;
        volt_dec = (short) fcalc;
        sprintf(to_send, "High Supply: %3d.%01dV\r\n", volt_int, volt_dec);
        RPI_Send(to_send);

        fcalc = Sense_15V;
        fcalc = fcalc * K_15V;
        volt_int = (short) fcalc;
        fcalc = fcalc - volt_int;
        fcalc = fcalc * 10;
        volt_dec = (short) fcalc;        
        sprintf(to_send, "Low Supply: %3d.%01dV\r\n", volt_int, volt_dec);
        RPI_Send(to_send);
        
        // sprintf(to_send, "High supply: %d, Low supply: %d\r\n", Sense_200V, Sense_15V);                
        // RPI_Send(to_send);
    }
    
    else if (!strncmp(msg, "hard_soft", sizeof("hard_soft") - 1))
    {
        char to_send [80];
        sprintf(to_send, "%s\r\n%s\r\n", HARD, SOFT);
        RPI_Send(to_send);
    }


    //-- Buzzer Actions
    else if (strncmp(msg, s_buzzer_short, sizeof(s_buzzer_short) - 1) == 0)
    {
        unsigned short bips_qtty = 0;
        
        msg += sizeof(s_buzzer_short);		//normalizo al payload, hay un espacio

        //lo que viene es un byte de 1 a 9
        decimales = StringIsANumber(msg, &bips_qtty);
        if (decimales == 1)
        {
            BuzzerCommands(BUZZER_SHORT_CMD, (unsigned char) bips_qtty);
            RPI_Send(s_ok);
        }
        else
            resp = resp_error;
    }

    else if (strncmp(msg, s_buzzer_half, sizeof(s_buzzer_half) - 1) == 0)
    {
        unsigned short bips_qtty = 0;
        
        msg += sizeof(s_buzzer_half);		//normalizo al payload, hay un espacio

        //lo que viene es un byte de 1 a 9
        decimales = StringIsANumber(msg, &bips_qtty);
        if (decimales == 1)
        {
            BuzzerCommands(BUZZER_HALF_CMD, (unsigned char) bips_qtty);
            RPI_Send(s_ok);
        }
        else
            RPI_Send(s_nok);

    }

    else if (strncmp(msg, s_buzzer_long, sizeof(s_buzzer_long) - 1) == 0)
    {
        unsigned short bips_qtty = 0;
        
        msg += sizeof(s_buzzer_long);		//normalizo al payload, hay un espacio

        //lo que viene es un byte de 1 a 9
        decimales = StringIsANumber(msg, &bips_qtty);
        if (decimales == 1)
            BuzzerCommands(BUZZER_LONG_CMD, (unsigned char) bips_qtty);
        else
            resp = resp_error;
    }

    else if (!strncmp(msg, (const char *)"keepalive,", (sizeof("keepalive,") - 1)))
    {
        RPI_Send(s_ok);
    }

    else if (!strncmp(msg, (const char *)"duration,", (sizeof("duration,") - 1)))
    {
        unsigned short new_time = 0;
        
        msg += sizeof("duration,") - 1;		//normalizo al payload

        //lo que viene son tres bytes con el tiempo de 1 a 120 se supone
        decimales = StringIsANumber(msg, &new_time);
        if (decimales == 3)
        {
            if (TreatmentSetTimeinMinutes(new_time) == resp_ok)
            {
                RPI_Send(s_ok);
                comms_messages_rpi |= COMM_CONF_CHANGE;
            }
            else
                RPI_Send(s_nok);
        }
        else
            RPI_Send(s_nok);
    }

    else if (!strncmp((const char *)msg, (const char *)"serial num", (sizeof("serial num") - 1)))
    {
        char to_send[40] = { 0 };
        
#ifdef USE_DEVICE_ID_4BYTES
        unsigned int device_id = *((unsigned short*)0x1FFFF7E8);
        device_id <<= 16;
        device_id |= *((unsigned short*)(0x1FFFF7E8 + 2));
        device_id ^= *((unsigned int*)(0x1FFFF7E8 + 4));
        device_id ^= *((unsigned int*)(0x1FFFF7E8 + 8));
        sprintf(to_send, "Device Id: 0x%08x\r\n", device_id);
            
        RPI_Send(to_send);
#endif
#ifdef USE_DEVICE_ID_12BYTES
        sprintf(to_send, "Device Id: 0x%04x%04x%08x%08x\r\n",
                *((unsigned short*)0x1FFFF7E8),
                *((unsigned short*)(0x1FFFF7E8 + 2)),
                *((unsigned int*)(0x1FFFF7E8 + 4)),
                *((unsigned int*)(0x1FFFF7E8 + 8)));
        
        RPI_Send(to_send);
#endif
    }
    
    //fin mensajes nuevos
    
    //mensajes anteriores
    else if (!strncmp(msg, (const char *)"get_temp,", (sizeof("get_temp,") - 1)))
    {
        switch (*(msg+9))
        {
        case '1':
            // UART_CH1_Send("get_temp\r\n");
            RPI_Send(s_ok);
            break;

        case '2':
            // UART_CH2_Send("get_temp\r\n");
            RPI_Send(s_ok);
            break;

        case '3':
            // UART_CH3_Send("get_temp\r\n");
            RPI_Send(s_ok);
            break;

        default:
            RPI_Send(s_nok);
            break;
        }
    }

    else if (!strncmp(msg, (const char *)"stop,", (sizeof("stop,") - 1)))
    {
        comms_messages_rpi |= COMM_STOP_TREAT;
    }

    else if (!strncmp(msg, (const char *)"pause,", (sizeof("pause,") - 1)))
    {
        comms_messages_rpi |= COMM_PAUSE_TREAT;
    }
    
    else if (!strncmp(msg,(const char *)"start,", (sizeof("start,") - 1)))
    {
        comms_messages_rpi |= COMM_START_TREAT;
    }


    else if (strncmp(msg, s_getall, sizeof(s_getall) - 1) == 0)
        SendAllConf();

    else if (strncmp(msg, (const char *) "get status", sizeof("get status") - 1) == 0)
        SendStatus();
    
    else
        RPI_Send(s_nok);


//     //--- end ---//

}

//reporta las errores y los limpia
//TODO: o limpiar en otra funcion???
void Raspberry_Report_Errors (unsigned char ch, unsigned short * errors)
{
    //reporta errores como "ERROR(0xNN)\r\n"
    //0x1N antena desconectada
    //0x2N antena perdida
    //0x4N sobre temperatura
    //0x5N sobre corriente
    //N num de canal 1-4

    if (*errors & COMM_ERROR_NO_COMM)
    {
        if (ch == CH1)
            RPI_Send("ERROR(0x11)\r\n");

        if (ch == CH2)
            RPI_Send("ERROR(0x12)\r\n");

        if (ch == CH3)
            RPI_Send("ERROR(0x13)\r\n");

        *errors &= ~COMM_ERROR_NO_COMM;
    }

    if (*errors & COMM_ERROR_NO_CURRENT)
    {
        if (ch == CH1)
            RPI_Send("ERROR(0x21)\r\n");

        if (ch == CH2)
            RPI_Send("ERROR(0x22)\r\n");

        if (ch == CH3)
            RPI_Send("ERROR(0x23)\r\n");

        *errors &= ~COMM_ERROR_NO_CURRENT;
    }

    if (*errors & COMM_ERROR_OVERTEMP)
    {
        if (ch == CH1)
            RPI_Send("ERROR(0x41)\r\n");

        if (ch == CH2)
            RPI_Send("ERROR(0x42)\r\n");

        if (ch == CH3)
            RPI_Send("ERROR(0x43)\r\n");

        *errors &= ~COMM_ERROR_OVERTEMP;
    }
    
    if (*errors & COMM_ERROR_OVERCURRENT)
    {
        if (ch == CH1)
            RPI_Send("ERROR(0x51)\r\n");

        if (ch == CH2)
            RPI_Send("ERROR(0x52)\r\n");

        if (ch == CH3)
            RPI_Send("ERROR(0x53)\r\n");

        *errors &= ~COMM_ERROR_OVERCURRENT;
    }

    if (*errors & COMM_ERROR_SOFT_OVERCURRENT)
    {
        if (ch == CH1)
            RPI_Send("ERROR(0x61)\r\n");

        if (ch == CH2)
            RPI_Send("ERROR(0x62)\r\n");

        if (ch == CH3)
            RPI_Send("ERROR(0x63)\r\n");

        *errors &= ~COMM_ERROR_SOFT_OVERCURRENT;
    }

    if (*errors & COMM_ERROR_NO_TREATMENT)
    {
        if (ch == CH1)
            RPI_Send("ERROR(0x71)\r\n");

        if (ch == CH2)
            RPI_Send("ERROR(0x72)\r\n");

        if (ch == CH3)
            RPI_Send("ERROR(0x73)\r\n");

        *errors &= ~COMM_ERROR_NO_TREATMENT;
    }
}

static void SendAllConf (void)
{
    char b [128];
    TreatmentGetAllConf(b);
    RPI_Send(b);
}

static void SendStatus (void)
{
    char b [128];
    unsigned char st = 0;
    
    st = TreatmentGetMainState();
    sprintf(b, "Main State: %d\r\n", st);
    RPI_Send(b);
}























//---- End of File ----//
