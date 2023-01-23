//---------------------------------------------------------
// #### PROJECT STRETCHER F103 - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### MAIN.C ############################################
//---------------------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "hard.h"

#include "adc.h"
#include "timer.h"
#include "gpio.h"
#include "usart.h"
#include "dma.h"

#include "treatment.h"
#include "comms_from_rasp.h"
#include "comms_from_power.h"
#include "comms.h"
#include "test_functions.h"

#include <stdio.h>
#include <string.h>



// Externals -------------------------------------------------------------------
//-- Externals to acknowledge data ready on usarts
volatile unsigned char usart1_have_data;
volatile unsigned char usart2_have_data;
volatile unsigned char usart3_have_data;
volatile unsigned char usart4_have_data;
volatile unsigned char usart5_have_data;

unsigned short comms_messages_1 = 0;
unsigned short comms_messages_2 = 0;
unsigned short comms_messages_3 = 0;
unsigned short comms_messages_rpi = 0;


char buffSendErr[64];

//--- Externals for keepalives on usarts
#define TIME_RUN_DEF 250
volatile unsigned short timeRun = TIME_RUN_DEF;

//--- Externals de los timers
volatile unsigned short wait_ms_var = 0;
volatile unsigned short comms_timeout = 0;

//--- Externals de los timers
volatile unsigned short adc_ch [ADC_CHANNEL_QUANTITY];


// Globals ---------------------------------------------------------------------
volatile unsigned short secs_in_treatment = 0;
volatile unsigned short millis = 0;
unsigned short secs_end_treatment;
unsigned short secs_elapsed_up_to_now;
volatile unsigned short timer_sync_xxx_ms = 0;
volatile unsigned short timer_out4 = 0;


// Module Private Functions ----------------------------------------------------
void TimingDelay_Decrement(void);
void SysTickError (void);

#define RPI_Flush_Comms (comms_messages_rpi &= ~COMM_RPI_ALL_MSG_MASK)


// Module Functions ------------------------------------------------------------
int main (void)
{
    char buff [64];
    treatment_t main_state = TREATMENT_STANDBY;
    unsigned short bytes_readed = 0;
    char s_to_senda [100];
    char s_to_sendb [100];

    // Gpio Configuration.
    GpioInit();
    OUT4_OFF;
    OUT1_OFF;
    
    // Systick Timer Activation
    if (SysTick_Config(72000))
        SysTickError();

    // Peripherals Activation    
    TIM7_Init();

    //-- DMA configuration.
    DMAConfig();
    DMA_ENABLE;
    
    //Uso ADC con DMA
    AdcConfig();

    //usart1 para comunicacion con raspberry
    Usart1Config();

    //usart2 para comunicacion con micros
    Usart2Config();
    
    ChangeLed(LED_NO_BLINKING);

    //-- Do some tests here, almost for hardware
    // TF_Voltages();
    // TF_Usart3_Tx();
    // TF_Usart3TxRx();
    // TF_Usart3Loop();
    // TF_CommsWithRaspberry();
    
    //-- Welcome Messages --------------------
    Usart1Send("\r\nGausstek Stretcher Board -- powered by: Kirno Technology\r\n");
    Wait_ms(100);

#ifdef HARD
    Usart1Send(HARD);
    Usart1Send("\r\n");
    Wait_ms(100);    
#else
#error	"No Hardware defined in hard.h file"
#endif

#ifdef SOFT
    Usart1Send(SOFT);
    Usart1Send("\r\n");
    Wait_ms(100);    
#else
#error	"No Soft Version defined in hard.h file"
#endif

    //limpio interfase serie
    //limpio buffers serie
    Wait_ms(3000);
    Power_Send("chf\n");
    ReadPowerBufferFlush();

    Wait_ms(1000);
    Power_Send("ch1 soft version\n");
    Wait_ms(100);
    if (power_have_data)
    {
        power_have_data = 0;
        ReadPowerBuffer(buff, 64);
        RPI_Send(buff);
        RPI_Send("\r\n");
    }
    else
        RPI_Send("no comms with ch1\r\n");

    Wait_ms(1000);
    Power_Send("ch2 soft version\n");
    Wait_ms(100);
    if (power_have_data)
    {
        power_have_data = 0;
        ReadPowerBuffer(buff, 64);
        RPI_Send(buff);
        RPI_Send("\r\n");
    }
    else
        RPI_Send("no comms with ch2\r\n");

    Wait_ms(1000);
    Power_Send("ch3 soft version\n");
    Wait_ms(100);
    if (power_have_data)
    {
        power_have_data = 0;
        ReadPowerBuffer(buff, 64);
        RPI_Send(buff);
        RPI_Send("\r\n");
    }
    else
        RPI_Send("no comms with ch3\r\n");
    
    

    //---- Programa Principal ----------
    ADC_START;
    ChangeLed(LED_TREATMENT_STANDBY);

#ifdef USE_ONE_BIP_INIT
    BuzzerCommands(BUZZER_SHORT_CMD, 2);
#endif

#ifdef MAGNETO_ESPECIAL_1CH
    unsigned char change_power = 0;
    unsigned char change_power_end = 0;    
    unsigned char power_output_stage = 1;
#endif
    while (1)
    {
        switch (main_state)
        {
        case TREATMENT_STANDBY:

            if (comms_messages_rpi & COMM_START_TREAT)
            {
                //me piden por el puerto que arranque el tratamiento
                comms_messages_rpi &= ~COMM_START_TREAT;
                if (TreatmentAssertParams() == resp_error)
                {
                    RPI_Send("ERROR\r\n");
                }
                else
                {
                    RPI_Send("OK\r\n");
                    PowerSendConf();
                    main_state = TREATMENT_STARTING;
                }
            }
            RPI_Flush_Comms;

#ifdef MAGNETO_ESPECIAL_1CH
            switch (change_power)
            {
            case 0:
                //cambiar potencia?
                if (CheckS1() > SW_NO)
                    change_power++;

                break;

            case 1:
                if (CheckS1() == SW_NO)
                {
                    if (power_output_stage < 3)
                        power_output_stage++;
                    else
                        power_output_stage = 1;

                    BuzzerCommands(BUZZER_SHORT_CMD, power_output_stage);
                    change_power = 0;
                }

                //empezar tratamiento?                
                if (CheckS1() > SW_MIN)
                {
                    TreatmentSetSignalType (SINUSOIDAL_SIGNAL);
                    TreatmentSetFrequency (7, 0);
                    TreatmentSetChannelsFlag(ENABLE_CH1_FLAG);
                    TreatmentSetChannelsFlag(DISABLE_CH2_FLAG);
                    TreatmentSetChannelsFlag(DISABLE_CH3_FLAG);
                    TreatmentSetTimeinMinutes(30);
                    if (power_output_stage == 1)
                        TreatmentSetPower (33);
                    else if (power_output_stage == 2)
                        TreatmentSetPower (66);
                    else
                        TreatmentSetPower (100);

                    change_power++;
                    BuzzerCommands(BUZZER_SHORT_CMD, 1);
                }
                break;

            case 2:
                if (CheckS1() == SW_NO)
                {
                    if (TreatmentAssertParams() == resp_error)
                        RPI_Send("ERROR\r\n");
                    else
                    {
                        RPI_Send("OK\r\n");
                        PowerSendConf();
                        main_state = TREATMENT_STARTING;
                    }
                    
                    change_power = 0;
                }
                break;

            default:
                change_power = 0;
                break;
            }
#endif
            break;

        case TREATMENT_STARTING:
            secs_end_treatment = TreatmentGetTime();
            secs_in_treatment = 1;    //con 1 arranca el timer
            secs_elapsed_up_to_now = 0;
            PowerCommunicationStackReset();

            // sprintf (buff, "treat start, ch1: 0x%04x, ch2: 0x%04x, ch3: 0x%04x\r\n",
            //          comms_messages_1,
            //          comms_messages_2,
            //          comms_messages_3);
                    
            // RPI_Send(buff);

            PowerSendStart();
            main_state = TREATMENT_RUNNING;
            ChangeLed(LED_TREATMENT_GENERATING);
#ifdef USE_BUZZER_ON_OUT3
            BuzzerCommands(BUZZER_HALF_CMD, 1);
#endif
            OUT5_ON;

            //OUT4 pulse to up the stretcher
            if (TreatmentGetUpDwn() == UPDWN_AUTO)
                comms_messages_rpi |= COMM_STRETCHER_UP;
            
            OUT2_ON;
            break;

        case TREATMENT_RUNNING:
            PowerCommunicationStack();    //me comunico con las potencias para conocer el estado

            if (comms_messages_rpi & COMM_PAUSE_TREAT)
            {
                comms_messages_rpi &= ~COMM_PAUSE_TREAT;
                RPI_Send("OK\r\n");
                PowerSendStop();
                main_state = TREATMENT_PAUSED;
                ChangeLed(LED_TREATMENT_PAUSED);
                secs_elapsed_up_to_now = secs_in_treatment;
            }

            if (comms_messages_rpi & COMM_STOP_TREAT)
            {
                comms_messages_rpi &= ~COMM_STOP_TREAT;

                //termine el tratamiento por stop, o finish_ok,
                RPI_Send("OK\r\n");
                PowerSendStop();
                main_state = TREATMENT_STOPPING;
            }

            //me mandaron start???
            if (comms_messages_rpi & COMM_START_TREAT)
            {
                comms_messages_rpi &= ~COMM_START_TREAT;
                RPI_Send("ERROR\r\n");
            }

            if (secs_in_treatment >= secs_end_treatment)
            {
                //termine el tratamiento
                PowerSendStop();
                RPI_Send("ended ok\r\n");
#ifdef USE_BUZZER_ON_OUT3
                BuzzerCommands(BUZZER_SHORT_CMD, 3);
#endif                
                main_state = TREATMENT_STOPPING;
            }

            //reviso si hay algun canal con error
            if ((comms_messages_1 & COMM_POWER_ERROR_MASK) ||
                (comms_messages_2 & COMM_POWER_ERROR_MASK) ||
                (comms_messages_3 & COMM_POWER_ERROR_MASK))
            {
                PowerSendStop();

                LED1_ON;
                secs_in_treatment = 0;    //con 0 freno el timer
                sprintf (buff, "treat err, ch1: 0x%04x, ch2: 0x%04x, ch3: 0x%04x\r\n",
                         comms_messages_1,
                         comms_messages_2,
                         comms_messages_3);
                    
                RPI_Send(buff);

                if (comms_messages_1 & COMM_POWER_ERROR_MASK)
                    Raspberry_Report_Errors(CH1, &comms_messages_1);

                if (comms_messages_2 & COMM_POWER_ERROR_MASK)
                    Raspberry_Report_Errors(CH2, &comms_messages_2);

                if (comms_messages_3 & COMM_POWER_ERROR_MASK)
                    Raspberry_Report_Errors(CH3, &comms_messages_3);

#ifdef USE_BUZZER_ON_OUT3
                BuzzerCommands(BUZZER_LONG_CMD, 1);
#endif                                
                LED1_OFF;
                main_state = TREATMENT_WITH_ERRORS;
            }
            RPI_Flush_Comms;

#ifdef MAGNETO_ESPECIAL_1CH
            //tengo que terminar??
            switch (change_power_end)
            {
            case 0:
                if (CheckS1() > SW_MIN)
                {
                    //termine el tratamiento
                    PowerSendStop();
                    RPI_Send("ended ok\r\n");
                    BuzzerCommands(BUZZER_SHORT_CMD, 3);
                    change_power_end++;
                }
                break;
                
            case 1:
                if (CheckS1() == SW_NO)
                {
                    change_power_end = 0;
                    main_state = TREATMENT_STOPPING;
                }
                break;
                
            default:
                change_power_end = 0;
                break;
            }
#endif
            break;

        case TREATMENT_PAUSED:
            // un segundo pause, me hace arrancar nuevamente
            if (comms_messages_rpi & COMM_PAUSE_TREAT)
            {
                comms_messages_rpi &= ~COMM_PAUSE_TREAT;
                secs_in_treatment = secs_elapsed_up_to_now;
                RPI_Send("OK\r\n");
                PowerSendStart();
                main_state = TREATMENT_RUNNING;
                ChangeLed(LED_TREATMENT_GENERATING);
            }

            if (comms_messages_rpi & COMM_STOP_TREAT)
            {
                //estaba en pausa y me mandaron stop
                comms_messages_rpi &= ~COMM_STOP_TREAT;
                RPI_Send("OK\r\n");
                PowerSendStop();
                main_state = TREATMENT_STOPPING;
            }
            RPI_Flush_Comms;
            break;
                
        case TREATMENT_STOPPING:
            secs_in_treatment = 0;    //con 0 freno el timer
            sprintf (buff, "treat end, ch1: 0x%04x, ch2: 0x%04x, ch3: 0x%04x\r\n",
                     comms_messages_1,
                     comms_messages_2,
                     comms_messages_3);
                    
            RPI_Send(buff);
            main_state = TREATMENT_STANDBY;
            ChangeLed(LED_TREATMENT_STANDBY);
            OUT5_OFF;

            //OUT4 pulse to down the stretcher
            if (TreatmentGetUpDwn() == UPDWN_AUTO)
                comms_messages_rpi |= COMM_STRETCHER_UP;
            
            OUT2_OFF;
            break;

        case TREATMENT_WITH_ERRORS:
            Wait_ms(1000);
            Power_Send("chf flush errors\n");
            RPI_Send("STOP\r\n");
            Wait_ms(1000);
            RPI_Send("STOP\r\n");
            Wait_ms(1000);
            RPI_Send("Flushing errors\r\n");

            Power_Send("chf flush errors\n");
            comms_messages_1 &= ~COMM_POWER_ERROR_MASK;
            comms_messages_2 &= ~COMM_POWER_ERROR_MASK;
            comms_messages_3 &= ~COMM_POWER_ERROR_MASK;            
            
            Wait_ms(1000);
            main_state = TREATMENT_STANDBY;
            ChangeLed(LED_TREATMENT_STANDBY);
            OUT5_OFF;
            
            //OUT4 pulse to down the stretcher
            if (TreatmentGetUpDwn() == UPDWN_AUTO)
                comms_messages_rpi |= COMM_STRETCHER_UP;
            
            OUT2_OFF;
            break;

        case MAIN_IN_BRIDGE_MODE:
            if (power_have_data)
            {
                power_have_data = 0;
                bytes_readed = ReadPowerBuffer(s_to_sendb, sizeof(s_to_sendb));

                if ((bytes_readed + 2) < sizeof(s_to_sendb))
                {
                    *(s_to_sendb + bytes_readed) = '\r';
                    *(s_to_sendb + bytes_readed + 1) = '\n';                    
                    *(s_to_sendb + bytes_readed + 2) = '\0';
                    RPI_Send(s_to_sendb);
                }
            }

            if (rpi_have_data)
            {
                rpi_have_data = 0;
                bytes_readed = ReadRPIBuffer(s_to_senda, sizeof(s_to_senda));
                    
                if (strncmp(s_to_senda, "goto normal mode", sizeof("goto normal mode") - 1) == 0)
                {
                    RPI_Send((char *)"Going to Normal Mode...\r\n");
                    main_state = TREATMENT_STANDBY;
                    ChangeLed(LED_TREATMENT_STANDBY);
                }
                else if ((bytes_readed + 1) < sizeof(s_to_senda))
                {
                    *(s_to_senda + bytes_readed - 1) = '\n';
                    *(s_to_senda + bytes_readed) = '\0';
                    Power_Send(s_to_senda);
                }

                if (LED1)
                    LED1_OFF;
                else
                    LED1_ON;
            }
            break;
                
        default:
            main_state = TREATMENT_STANDBY;
            break;
        }            


        if (main_state != MAIN_IN_BRIDGE_MODE)
        {
            //reviso comunicacion con raspberry
            UpdateRaspberryMessages();

            //reviso comunicacion con potencias
            UpdatePowerMessages();

            // en cualquier momento me pueden pedir mover la camilla
            if (comms_messages_rpi & COMM_STRETCHER_UP)
            {
                comms_messages_rpi &= ~COMM_STRETCHER_UP;
                timer_out4 = TIMER_OUT4_IN_ON;
                OUT4_ON;
                OUT1_ON;
            }

            //reviso si tengo que ir al modo bridge
            if (comms_messages_rpi & COMM_GOTO_BRIDGE)
            {
                comms_messages_rpi &= ~COMM_GOTO_BRIDGE;
                main_state = MAIN_IN_BRIDGE_MODE;
                ChangeLed(LED_TREATMENT_BRIDGE_MODE);
            }
        }

        if (sequence_ready)
            sequence_ready_reset;

        UpdateLed();
        UpdateBuzzer();

        TreatmentUpdateMainState(main_state);

#ifdef MAGNETO_ESPECIAL_1CH
        UpdateSwitches();
#endif
        
        
#ifdef USE_SYNC_ALL_PLACES        
        UpdateSyncPulses();
#elif defined USE_SYNC_ONLY_ON_TREATMENT
        if (main_state == TREATMENT_RUNNING)
            UpdateSyncPulses();
#else
#pragma message "NO SYNC SELECTED"
#endif
    }

//---- Fin Programa Pricipal ----------
}
//--- End of Main ---//


void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (comms_timeout)
        comms_timeout--;

    if (secs_in_treatment)
    {
        if (millis < 1000)
            millis++;
        else
        {
            secs_in_treatment++;
            millis = 0;
        }
    }

    if (timer_out4)
        timer_out4--;
    else
    {
        OUT4_OFF;
        OUT1_OFF;
    }
    
    // if (timer_standby)
    //     timer_standby--;

    // if (timer_filters)
    //     timer_filters--;
    

    // if (timer_led_pwm < 0xFFFF)
    //     timer_led_pwm ++;
    HARD_Timers_Update();
}


void SysTickError (void)
{
    //Capture systick error...
    while (1)
    {
        if (LED1)
            LED1_OFF;
        else
            LED1_ON;

        for (unsigned char i = 0; i < 255; i++)
        {
            asm ("nop \n\t"
                 "nop \n\t"
                 "nop \n\t" );
        }
    }
}

//--- end of file ---//

