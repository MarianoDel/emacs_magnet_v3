//------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### SIGNALS.C ################################
//------------------------------------------------

// Includes --------------------------------------------------------------------
#include "signals.h"
#include "signals_defs.h"

#include "hard.h"
// #include "stm32f0xx.h"
#include "tim.h"
#include "dsp.h"
#include "adc.h"
#include "pwm.h"
#include "channels_defs.h"
#include "antennas_defs.h"
#include "errors.h"

#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
	ERROR_OK = 0,
	ERROR_OVERCURRENT,
	ERROR_NO_CURRENT,
	ERROR_SOFT_OVERCURRENT,
	ERROR_OVERTEMP

} error_e;


#define USE_SOFT_NO_CURRENT
#define USE_SOFT_OVERCURRENT
// Externals -------------------------------------------------------------------
//del ADC
// extern volatile unsigned char seq_ready;
extern volatile unsigned short adc_ch[];

//del Main
// extern volatile unsigned short timer_signals;


//de usart para sync
// extern volatile unsigned char sync_on_signal;

//del pid dsp.c
// extern unsigned short pid_param_p;
// extern unsigned short pid_param_i;
// extern unsigned short pid_param_d;

// externals from tim
extern volatile unsigned char timer1_seq_ready;



// Globals ---------------------------------------------------------------------
unsigned short phase_accum = 0;
unsigned short signal_index = 0;


//-- para determinacion de soft overcurrent ------------
#ifdef USE_SOFT_OVERCURRENT
ma8_u16_data_obj_t signal_ovcp_filter_ch1;
ma8_u16_data_obj_t signal_ovcp_filter_ch2;
ma8_u16_data_obj_t signal_ovcp_filter_ch3;
ma8_u16_data_obj_t signal_ovcp_filter_ch4;

unsigned short signal_ovcp_threshold_ch1 = 0;
unsigned short signal_ovcp_threshold_ch2 = 0;
unsigned short signal_ovcp_threshold_ch3 = 0;
unsigned short signal_ovcp_threshold_ch4 = 0;
#endif

//-- para determinar no current
//SIZEOF_SIGNALS * max_ADC = 150 * 1023 = 153450
//ojo, depende del salto de indice en la tabla segun la freq elegida
#ifdef USE_SOFT_NO_CURRENT
unsigned int signal_no_current_threshold_ch1 = 0;
unsigned int signal_no_current_threshold_ch2 = 0;
unsigned int signal_no_current_threshold_ch3 = 0;
unsigned int signal_no_current_threshold_ch4 = 0;

unsigned int signal_integral_ch1 = 0;
unsigned int signal_integral_ch2 = 0;
unsigned int signal_integral_ch3 = 0;
unsigned int signal_integral_ch4 = 0;

unsigned char signal_no_current_cnt_ch1 = 0;
unsigned char signal_no_current_cnt_ch2 = 0;
unsigned char signal_no_current_cnt_ch3 = 0;
unsigned char signal_no_current_cnt_ch4 = 0;
#define NO_CURRENT_THRESHOLD_CNT    4
#endif


const unsigned short sinusoidal_table_inphase [] = {25,50,75,100,125,150,174,199,224,248,
                                                    272,296,320,344,368,391,414,437,459,482,
                                                    504,525,547,568,589,609,629,648,668,687,
                                                    705,723,740,757,774,790,806,821,836,850,
                                                    864,877,890,902,913,924,935,945,954,963,
                                                    971,978,985,992,998,1003,1007,1011,1015,1018,
                                                    1020,1021,1022,1023,1022,1021,1020,1018,1015,1011,
                                                    1007,1003,998,992,985,978,971,963,954,945,
                                                    935,924,913,902,890,877,864,850,836,821,
                                                    806,790,774,757,740,723,705,687,668,648,
                                                    629,609,589,568,547,525,504,482,459,437,
                                                    414,391,368,344,320,296,272,248,224,199,
                                                    174,150,125,100,75,50,25,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0};

const unsigned short sinusoidal_table_outphase [] = {0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,25,50,
                                                     75,100,125,150,174,199,224,248,272,296,
                                                     320,344,368,391,414,437,459,482,504,525,
                                                     547,568,589,609,629,648,668,687,705,723,
                                                     740,757,774,790,806,821,836,850,864,877,
                                                     890,902,913,924,935,945,954,963,971,978,
                                                     985,992,998,1003,1007,1011,1015,1018,1020,1021,
                                                     1022,1023,1022,1021,1020,1018,1015,1011,1007,1003,
                                                     998,992,985,978,971,963,954,945,935,924,
                                                     913,902,890,877,864,850,836,821,806,790,
                                                     774,757,740,723,705,687,668,648,629,609,
                                                     589,568,547,525,504,482,459,437,414,391,
                                                     368,344,320,296,272,248,224,199,174,150,
                                                     125,100,75,50,25,0};


const unsigned short triangular_table_inphase [] = {7,15,23,31,39,47,55,63,71,79,
                                                    87,95,103,111,119,127,135,143,151,159,
                                                    167,175,183,191,199,207,215,223,231,239,
                                                    247,255,263,271,279,287,295,303,311,319,
                                                    327,335,343,351,359,367,375,383,391,399,
                                                    407,415,423,431,439,447,455,463,471,479,
                                                    487,495,503,511,519,527,535,543,551,559,
                                                    567,575,583,591,599,607,615,623,631,639,
                                                    647,655,663,671,679,687,695,703,711,719,
                                                    727,735,743,751,759,767,775,783,791,799,
                                                    807,815,823,831,839,847,855,863,871,879,
                                                    887,895,903,911,919,927,935,943,951,959,
                                                    967,975,983,991,999,1007,1015,1023,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0,0,0,0,0,
                                                    0,0,0,0,0,0};


const unsigned short triangular_table_outphase [] = {0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,7,15,
                                                     23,31,39,47,55,63,71,79,87,95,
                                                     103,111,119,127,135,143,151,159,167,175,
                                                     183,191,199,207,215,223,231,239,247,255,
                                                     263,271,279,287,295,303,311,319,327,335,
                                                     343,351,359,367,375,383,391,399,407,415,
                                                     423,431,439,447,455,463,471,479,487,495,
                                                     503,511,519,527,535,543,551,559,567,575,
                                                     583,591,599,607,615,623,631,639,647,655,
                                                     663,671,679,687,695,703,711,719,727,735,
                                                     743,751,759,767,775,783,791,799,807,815,
                                                     823,831,839,847,855,863,871,879,887,895,
                                                     903,911,919,927,935,943,951,959,967,975,
                                                     983,991,999,1007,1015,1023};


const unsigned short square_table_inphase [] = {1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                1023,1023,1023,1023,1023,1023,1023,1023,0,0,
                                                0,0,0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0};

const unsigned short square_table_outphase [] = {0,0,0,0,0,0,0,0,0,0,
                                                 0,0,0,0,0,0,0,0,0,0,
                                                 0,0,0,0,0,0,0,0,0,0,
                                                 0,0,0,0,0,0,0,0,0,0,
                                                 0,0,0,0,0,0,0,0,0,0,
                                                 0,0,0,0,0,0,0,0,0,0,
                                                 0,0,0,0,0,0,0,0,0,0,
                                                 0,0,0,0,0,0,0,0,0,0,
                                                 0,0,0,0,0,0,0,0,0,0,
                                                 0,0,0,0,0,0,0,0,0,0,
                                                 0,0,0,0,0,0,0,0,0,0,
                                                 0,0,0,0,0,0,0,0,0,0,
                                                 0,0,0,0,0,0,0,0,1023,1023,
                                                 1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                 1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                 1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                 1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                 1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                 1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                 1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                 1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                 1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                 1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                 1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                 1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                                                 1023,1023,1023,1023,1023,1023};



// Module Private Functions ----------------------------------------------------
void Signals_Generate_Channel (unsigned char which_channel, unsigned short new_sp);
void Signals_Setup_Phase_Accumulator (unsigned char freq_int,
                                      unsigned char freq_dec,
                                      unsigned short * phase_accum);


// Module Functions ------------------------------------------------------------
const unsigned short * p_table_inphase;
const unsigned short * p_table_outphase;
pi_data_obj_t pi_ch1;
pi_data_obj_t pi_ch2;
pi_data_obj_t pi_ch3;
pi_data_obj_t pi_ch4;
unsigned short ch1_max_current = 0;
unsigned short ch2_max_current = 0;
unsigned short ch3_max_current = 0;
unsigned short ch4_max_current = 0;

// just for test, im still thinking how to manage this
signals_struct_t global_signals = {
    // general all channels things
    // .signal = SINUSOIDAL_SIGNAL,
    // .signal = SQUARE_SIGNAL,
    .signal = TRIANGULAR_SIGNAL,

    .freq_int = 23,
    .freq_dec = 10,

    .power = 100,

    // by channel things    
    .kp_ch1 = 50,
    .ki_ch1 = 3,
    .kp_ch2 = 10,
    .ki_ch2 = 10,
    // .kp_ch2 = 0,
    // .ki_ch2 = 0,
    .kp_ch3 = 5,
    .ki_ch3 = 2,
    .kp_ch4 = 1,
    .ki_ch4 = 30,

    // .max_c_ch1 = 870,
    .max_c_ch1 = 1590,    
    .max_c_ch2 = 870,
    .max_c_ch3 = 870,
    .max_c_ch4 = 870,
    
};


void Signals_Setup_Treatment_Data (signals_struct_t * new_treat_data)
{
    global_signals.signal = new_treat_data->signal;
    global_signals.freq_int = new_treat_data->freq_int;
    global_signals.freq_dec = new_treat_data->freq_dec;
    global_signals.power = new_treat_data->power;
}


void Signals_Setup_All_Channels (void)
{
    unsigned char signal_mean = 0;
    
    switch (global_signals.signal)
    {
    case SINUSOIDAL_SIGNAL:
        p_table_inphase = sinusoidal_table_inphase;
        p_table_outphase = sinusoidal_table_outphase;
        signal_mean = 32;
        break;

    case SQUARE_SIGNAL:
        p_table_inphase = square_table_inphase;
        p_table_outphase = square_table_outphase;
        signal_mean = 50;        
        break;

    case TRIANGULAR_SIGNAL:
        p_table_inphase = triangular_table_inphase;
        p_table_outphase = triangular_table_outphase;
        signal_mean = 25;
        break;
        
    }
    
    signal_index = 0;

    // phase accum calc and signal index increment
    Signals_Setup_Phase_Accumulator (
        global_signals.freq_int,
        global_signals.freq_dec,
        &phase_accum);

#ifdef TESTING_SHOW_INFO
    printf(" -- freq: %d.%02dHz phase_accum: %d --\n", 
           global_signals.freq_int,
           global_signals.freq_dec,           
           phase_accum);
#endif
    
    ch1_max_current = global_signals.max_c_ch1;
    ch2_max_current = global_signals.max_c_ch2;
    ch3_max_current = global_signals.max_c_ch3;
    ch4_max_current = global_signals.max_c_ch4;

    PI_roof_Flush (&pi_ch1);
    PI_roof_Flush (&pi_ch2);
    PI_roof_Flush (&pi_ch3);
    PI_roof_Flush (&pi_ch4);

    pi_ch1.kp = global_signals.kp_ch1;
    pi_ch1.ki = global_signals.ki_ch1;

    pi_ch2.kp = global_signals.kp_ch2;
    pi_ch2.ki = global_signals.ki_ch2;

    pi_ch3.kp = global_signals.kp_ch3;
    pi_ch3.ki = global_signals.ki_ch3;

    pi_ch4.kp = global_signals.kp_ch4;
    pi_ch4.ki = global_signals.ki_ch4;

#ifdef USE_SOFT_NO_CURRENT
    if (phase_accum == 0)
    {
        phase_accum = 256;    //default for 27.34Hz
#ifdef TESTING_SHOW_INFO
        printf("ERROR phase_accum is 0 on Signals_Setup_All_Channels\n");
#endif                
    }

    unsigned int mean_current = 65536 / phase_accum;
    mean_current = mean_current * global_signals.power * signal_mean;
    mean_current = mean_current / 10000;    //adjust power, mean and phase_accum

    signal_no_current_threshold_ch1 = mean_current * ch1_max_current;    //antenna current peak value
    signal_no_current_threshold_ch2 = mean_current * ch2_max_current;    //antenna current peak value
    signal_no_current_threshold_ch3 = mean_current * ch3_max_current;    //antenna current peak value
    signal_no_current_threshold_ch4 = mean_current * ch4_max_current;    //antenna current peak value    

    signal_no_current_threshold_ch1 >>= 1;    // threshold at 50%
    signal_no_current_threshold_ch2 >>= 1;    // threshold at 50%
    signal_no_current_threshold_ch3 >>= 1;    // threshold at 50%
    signal_no_current_threshold_ch4 >>= 1;    // threshold at 50%
    
#ifdef TESTING_SHOW_INFO
    printf(" -- no current ch1 mean: %d threshold: %d\n", 
           mean_current * ch1_max_current,
           signal_no_current_threshold_ch1);
#endif
    
    signal_integral_ch1 = 0;
    signal_integral_ch2 = 0;
    signal_integral_ch3 = 0;
    signal_integral_ch4 = 0;    
#endif

#ifdef USE_SOFT_OVERCURRENT
    // soft overcurrent is 20% above the maximun current with 100% power

    signal_ovcp_threshold_ch1 = 12 * ch1_max_current / 10;
    signal_ovcp_threshold_ch2 = 12 * ch2_max_current / 10;
    signal_ovcp_threshold_ch3 = 12 * ch3_max_current / 10;
    signal_ovcp_threshold_ch4 = 12 * ch4_max_current / 10;
    
    MA8_U16Circular_Reset(&signal_ovcp_filter_ch1);
    MA8_U16Circular_Reset(&signal_ovcp_filter_ch2);
    MA8_U16Circular_Reset(&signal_ovcp_filter_ch3);
    MA8_U16Circular_Reset(&signal_ovcp_filter_ch4);
    
#endif    
}


void Signals_Setup_Phase_Accumulator (unsigned char freq_int,
                                      unsigned char freq_dec,
                                      unsigned short * phase_accum)
{
    // try first with increment of 1
    float fsampling = 7000.0;
    float freq = freq_int + freq_dec / 100.0;
    float calc = freq * 256 * 256 / fsampling;
    
    *phase_accum = (unsigned short) calc;

    // float fsampling = 7000.0;
    // float freq = freq_int + freq_dec / 100.0;
    // float calc = freq * 256 * 65536 / fsampling;
    
    // unsigned int p = (unsigned int) calc;
    // if ((p >> 16) == 0)
    // {
    //     *phase_accum = (unsigned short) p;
    //     *signal_index_inc = 1;
    // }
    // else
    // {
    //     *signal_index_inc = 1 + (p >> 16);
    //     calc = freq * 256 * 65536 / (fsampling * (*signal_index_inc));
    //     p = (unsigned short) calc;
    //     *phase_accum = (unsigned short) p;
    // }
    
}


#define CHANNEL_CONNECTED_GOOD    1
#define CHANNEL_DISCONNECT    2
void Signals_Generate_All_Channels (void)
{
    if (!timer1_seq_ready)
        return;

    timer1_seq_ready = 0;
    // seq_ready_cnt++;
    unsigned char signal_ended = 0;
                
    // get the current SP
    unsigned char s_index = signal_index >> 8;
    unsigned short sp_inphase = *(p_table_inphase + s_index);
    unsigned short sp_outphase = *(p_table_outphase + s_index);

    // index update (modulo 16?)
    unsigned int total_phase = signal_index + phase_accum;
    if (total_phase < 65535)
    {
        signal_index += phase_accum;
    }
    else
    {
        signal_index += phase_accum;    //modulo 16 roundup
        signal_ended = 1;
    }
    
    if (global_signals.treat_in_ch1 == CHANNEL_CONNECTED_GOOD)
    {
#ifdef TESTING_SHOW_INFO_INDEX_SP
        printf("signal_index: %d sp_inphase: %d\n", signal_index, sp_inphase);
#endif
        Signals_Generate_Channel (CH1, sp_inphase);
    }

    if (global_signals.treat_in_ch2 == CHANNEL_CONNECTED_GOOD)
        Signals_Generate_Channel (CH2, sp_inphase);

    if (global_signals.treat_in_ch3 == CHANNEL_CONNECTED_GOOD)
        Signals_Generate_Channel (CH3, sp_outphase);

    if (global_signals.treat_in_ch4 == CHANNEL_CONNECTED_GOOD)
        Signals_Generate_Channel (CH4, sp_outphase);

    // now check channels for errors
#ifdef USE_SOFT_NO_CURRENT
    if (signal_ended)
    {
        if (global_signals.treat_in_ch1 == CHANNEL_CONNECTED_GOOD)
        {
#ifdef TESTING_SHOW_INFO
            printf(" -- ch1 integral: %d threshold: %d --\n", 
                   signal_integral_ch1,
                   signal_no_current_threshold_ch1);
#endif
            if (signal_integral_ch1 < signal_no_current_threshold_ch1)
            {
                if (signal_no_current_cnt_ch1 < NO_CURRENT_THRESHOLD_CNT)
                    signal_no_current_cnt_ch1++;
                else
                {
#ifdef TESTING_SHOW_INFO
                    printf("stop no current on ch1 integral: %d threshold: %d\n", 
                           signal_integral_ch1,
                           signal_no_current_threshold_ch1);
#endif
                    Signals_Stop_Single_Channel(CH1);
                    global_signals.treat_in_ch1 = CHANNEL_DISCONNECT;
                    Error_SetStatus(ERROR_NO_CURRENT, CH1);
                }
            }
        }

        if (global_signals.treat_in_ch2 == CHANNEL_CONNECTED_GOOD)
        {
            if (signal_integral_ch2 < signal_no_current_threshold_ch2)
            {
                if (signal_no_current_cnt_ch2 < NO_CURRENT_THRESHOLD_CNT)
                    signal_no_current_cnt_ch2++;
                else
                {
                    Signals_Stop_Single_Channel(CH2);
                    global_signals.treat_in_ch2 = CHANNEL_DISCONNECT;
                    Error_SetStatus(ERROR_NO_CURRENT, CH2);
                }
            }
        }

        if (global_signals.treat_in_ch3 == CHANNEL_CONNECTED_GOOD)
        {
            if (signal_integral_ch3 < signal_no_current_threshold_ch3)
            {
                if (signal_no_current_cnt_ch3 < NO_CURRENT_THRESHOLD_CNT)
                    signal_no_current_cnt_ch3++;
                else
                {
                    Signals_Stop_Single_Channel(CH3);
                    global_signals.treat_in_ch3 = CHANNEL_DISCONNECT;
                    Error_SetStatus(ERROR_NO_CURRENT, CH3);
                }
            }
        }

        if (global_signals.treat_in_ch4 == CHANNEL_CONNECTED_GOOD)
        {
            if (signal_integral_ch4 < signal_no_current_threshold_ch4)
            {
                if (signal_no_current_cnt_ch4 < NO_CURRENT_THRESHOLD_CNT)
                    signal_no_current_cnt_ch4++;
                else
                {
                    Signals_Stop_Single_Channel(CH4);
                    global_signals.treat_in_ch4 = CHANNEL_DISCONNECT;
                    Error_SetStatus(ERROR_NO_CURRENT, CH4);
                }
            }
        }        
        
        signal_integral_ch1 = 0;
        signal_integral_ch2 = 0;
        signal_integral_ch3 = 0;
        signal_integral_ch4 = 0;
    }
    else
    {
        signal_integral_ch1 += IS_CH1;
        signal_integral_ch2 += IS_CH2;
        signal_integral_ch3 += IS_CH3;
        signal_integral_ch4 += IS_CH4;                
    }
#endif    // USE_SOFT_NO_CURRENT

#ifdef USE_SOFT_OVERCURRENT
    if (global_signals.treat_in_ch1 == CHANNEL_CONNECTED_GOOD)
    {
        unsigned short filter_c = MA8_U16Circular(&signal_ovcp_filter_ch1, IS_CH1);
        if (filter_c > signal_ovcp_threshold_ch1)
        {
#ifdef TESTING_SHOW_INFO
            printf("ch1 current filtered: %d threshold: %d sample: %d index: %d\n",
                   filter_c,
                   signal_ovcp_threshold_ch1,
                   IS_CH1,
                   signal_index);
#endif

            Signals_Stop_Single_Channel(CH1);
            global_signals.treat_in_ch1 = CHANNEL_DISCONNECT;
            Error_SetStatus(ERROR_SOFT_OVERCURRENT, CH1);
        }
    }

    if (global_signals.treat_in_ch2 == CHANNEL_CONNECTED_GOOD)
    {
        unsigned short filter_c = MA8_U16Circular(&signal_ovcp_filter_ch2, IS_CH2);
        if (filter_c > signal_ovcp_threshold_ch2)
        {
            // printf("ch2 current filtered: %d threshold: %d sample: %d index: %d\n",
            //        filter_c,
            //        signal_ovcp_threshold_ch2,
            //        IS_CH2,
            //        signal_index);

            Signals_Stop_Single_Channel(CH2);                
            global_signals.treat_in_ch2 = CHANNEL_DISCONNECT;
            Error_SetStatus(ERROR_SOFT_OVERCURRENT, CH2);
        }
    }

    if (global_signals.treat_in_ch3 == CHANNEL_CONNECTED_GOOD)
    {
        unsigned short filter_c = MA8_U16Circular(&signal_ovcp_filter_ch3, IS_CH3);
        if (filter_c > signal_ovcp_threshold_ch3)
        {
            // printf("ch3 current filtered: %d threshold: %d sample: %d index: %d\n",
            //        filter_c,
            //        signal_ovcp_threshold_ch3,
            //        IS_CH3,
            //        signal_index);

            Signals_Stop_Single_Channel(CH3);                
            global_signals.treat_in_ch3 = CHANNEL_DISCONNECT;
            Error_SetStatus(ERROR_SOFT_OVERCURRENT, CH3);
        }
    }

    if (global_signals.treat_in_ch4 == CHANNEL_CONNECTED_GOOD)        
    {
        unsigned short filter_c = MA8_U16Circular(&signal_ovcp_filter_ch4, IS_CH4);
        if (filter_c > signal_ovcp_threshold_ch4)
        {
            // printf("ch4 current filtered: %d threshold: %d sample: %d index: %d\n",
            //        filter_c,
            //        signal_ovcp_threshold_ch4,
            //        IS_CH4,
            //        signal_index);

            Signals_Stop_Single_Channel(CH4);
            global_signals.treat_in_ch4 = CHANNEL_DISCONNECT;
            Error_SetStatus(ERROR_SOFT_OVERCURRENT, CH4);
        }
    }
#endif
    // end of check channels errors
    
}    

short p_inphase_ch1 [256] = { 0 };
void Signals_Reference_Reset (void)
{
    for (int i = 0; i < 256; i++)
        *(p_inphase_ch1 + i) = 0;
    
}

void Signals_Generate_Single_Channel_OpenLoop (void)
{
    if (!timer1_seq_ready)
        return;

    timer1_seq_ready = 0;
    // seq_ready_cnt++;
    unsigned char signal_ended = 0;
                
    // get the current SP
    unsigned char s_index = signal_index >> 8;
    short ref_inphase = *(p_inphase_ch1 + s_index);

    // index update (modulo 16?)
    if (signal_index < 65535)
        signal_index += phase_accum;
    else
    {
        signal_index += phase_accum;    //modulo 16 roundup
        signal_ended = 1;
    }
    
    if (global_signals.treat_in_ch1 == CHANNEL_CONNECTED_GOOD)
        Signals_Generate_Channel_OpenLoop (CH1, ref_inphase);

    // now check channels for errors
#ifdef USE_SOFT_NO_CURRENT
    if (signal_ended)
    {
        if (global_signals.treat_in_ch1 == CHANNEL_CONNECTED_GOOD)
        {
            if (signal_integral_ch1 < signal_no_current_threshold_ch1)
            {
                if (signal_no_current_cnt_ch1 < NO_CURRENT_THRESHOLD_CNT)
                    signal_no_current_cnt_ch1++;
                else
                {
                    global_signals.treat_in_ch1 = CHANNEL_DISCONNECT;
                    Error_SetStatus(ERROR_NO_CURRENT, CH1);
                }
            }
        }
        
        signal_integral_ch1 = 0;
    }
    else
    {
        signal_integral_ch1 += IS_CH1;
    }
#endif    // USE_SOFT_NO_CURRENT

#ifdef USE_SOFT_OVERCURRENT
        if (global_signals.treat_in_ch1 == CHANNEL_CONNECTED_GOOD)
        {
            unsigned short filter_c = MA8_U16Circular(&signal_ovcp_filter_ch1, IS_CH1);
            if (filter_c > signal_ovcp_threshold_ch1)
            {
                // printf("ch1 current filtered: %d threshold: %d sample: %d index: %d\n",
                //        filter_c,
                //        signal_ovcp_threshold_ch1,
                //        IS_CH1,
                //        signal_index);

                Signals_Stop_Single_Channel(CH1);
                global_signals.treat_in_ch1 = CHANNEL_DISCONNECT;
                Error_SetStatus(ERROR_SOFT_OVERCURRENT, CH1);
            }
        }
#endif
    // end of check channels errors
    
}    


void (* pf_high_left) (unsigned short);
void (* pf_low_right) (unsigned short);
void Signals_Generate_Channel (unsigned char which_channel, unsigned short new_sp)
{
    pi_data_obj_t * p_pi;
    unsigned short sample = 0;
    unsigned short max_c = 0;
    
    switch (which_channel)
    {
    case CH1:
        pf_high_left = TIM8_Update_CH3;
        pf_low_right = TIM8_Update_CH4;
        p_pi = &pi_ch1;
        sample = IS_CH1;
        max_c = ch1_max_current;
        // printf("is_ch1: %d sp_ch1: %d\n", IS_CH1, new_sp);
        break;

    case CH2:
        pf_high_left = TIM8_Update_CH2;
        pf_low_right = TIM8_Update_CH1;
        p_pi = &pi_ch2;
        sample = IS_CH2;
        max_c = ch2_max_current;        
        break;

    case CH3:
        pf_high_left = TIM4_Update_CH2;
        pf_low_right = TIM4_Update_CH3;
        p_pi = &pi_ch3;
        sample = IS_CH3;
        max_c = ch3_max_current;        
        break;

    case CH4:
        pf_high_left = TIM5_Update_CH1;
        pf_low_right = TIM5_Update_CH2;
        p_pi = &pi_ch4;
        sample = IS_CH4;
        max_c = ch4_max_current;        
        // printf("is_ch4: %d sp_ch4: %d\n", IS_CH4, new_sp);        
        break;
            
    }

    // ch1 max current fixt
    // unsigned int sp = new_sp * signal.power * antenna.power;
    // unsigned int sp = new_sp * 100 * 870;    //870 corriente max de antena
    unsigned int sp = new_sp * global_signals.power * max_c;    //870 corriente max de antena    
    sp >>= 10;    //div 1024 compensate for max current
    sp = sp / 100;    //compensate for percentage power 
#ifdef TESTING_SHOW_INFO_SP_OFFSET
    printf("new_sp: %d power: %d max_c: %d sp: %d\n",
           new_sp,
           global_signals.power,
           max_c,
           sp);

    if (sp > 4095)
        printf("sp is wrong!!! %d\n", sp);

#endif
    
    // set duty cycle and save pi current data
    p_pi->sample = sample;
    p_pi->setpoint = sp;
    
    if (sp == 0)
    {
        //fast discharge
        pf_high_left (DUTY_NONE);
        pf_low_right (DUTY_NONE);
        PI_roof_Flush (p_pi);
    }
    else
    {
        // normal emission
        short duty = PI_roof (p_pi);        
        
        if (duty > 0)
        {
            if (duty > DUTY_95_PERCENT)
                duty = DUTY_95_PERCENT;
                        
            pf_high_left (duty);
            pf_low_right (DUTY_ALWAYS);
        }
        else if (duty == 0)
        {
            pf_high_left (DUTY_NONE);
            pf_low_right (DUTY_ALWAYS);
        }
        else    // duty negative
        {
            // printf("duty neg: %d sp: %d sample: %d\n",
            //        duty,
            //        p_pi->setpoint,
            //        p_pi->sample);
            
            // fixt duty
            duty = -duty;
                
            if (duty > DUTY_95_PERCENT)
                duty = DUTY_95_PERCENT;
                                                
            pf_high_left (DUTY_NONE);
            pf_low_right (duty);                        
                
            // fixt duty
            duty = -duty;
        }
        // set the roof on duty
        p_pi->last_d = duty;
    }
}


void Signals_Generate_Channel_OpenLoop (unsigned char which_channel, short new_ref)
{
    switch (which_channel)
    {
    case CH1:
        pf_high_left = TIM8_Update_CH3;
        pf_low_right = TIM8_Update_CH4;
        break;

    case CH2:
        pf_high_left = TIM8_Update_CH2;
        pf_low_right = TIM8_Update_CH1;
        break;

    case CH3:
        pf_high_left = TIM4_Update_CH2;
        pf_low_right = TIM4_Update_CH3;
        break;

    case CH4:
        pf_high_left = TIM5_Update_CH1;
        pf_low_right = TIM5_Update_CH2;
        break;
    }

    if (new_ref == 0)
    {
        //fast discharge
        pf_high_left (DUTY_NONE);
        pf_low_right (DUTY_NONE);
    }
    else
    {
        if (new_ref > 0)
        {
            if (new_ref > DUTY_95_PERCENT)
                new_ref = DUTY_95_PERCENT;
                        
            pf_high_left (new_ref);
            pf_low_right (DUTY_ALWAYS);
        }
        else if (new_ref == 0)
        {
            pf_high_left (DUTY_NONE);
            pf_low_right (DUTY_ALWAYS);
        }
        else    // duty negative
        {
            // fixt duty
            new_ref = -new_ref;
                
            if (new_ref > DUTY_95_PERCENT)
                new_ref = DUTY_95_PERCENT;
                                                
            pf_high_left (DUTY_NONE);
            pf_low_right (new_ref);                        
                
            // fixt duty
            new_ref = -new_ref;
        }
    }
}


//state = 1 set channel
//state = 0 reset channel
void Signals_Set_Reset_Channel_For_Treatment (unsigned char which_channel, unsigned char state)
{
    switch (which_channel)
    {
    case CH1:
        if (state)
            global_signals.treat_in_ch1 = CHANNEL_CONNECTED_GOOD;
        else
            global_signals.treat_in_ch1 = CHANNEL_DISCONNECT;
        
        break;

    case CH2:
        if (state)
            global_signals.treat_in_ch2 = CHANNEL_CONNECTED_GOOD;
        else
            global_signals.treat_in_ch2 = CHANNEL_DISCONNECT;

        break;

    case CH3:
        if (state)
            global_signals.treat_in_ch3 = CHANNEL_CONNECTED_GOOD;
        else
            global_signals.treat_in_ch3 = CHANNEL_DISCONNECT;

        break;

    case CH4:
        if (state)
            global_signals.treat_in_ch4 = CHANNEL_CONNECTED_GOOD;
        else
            global_signals.treat_in_ch4 = CHANNEL_DISCONNECT;

        break;
            
    }        
}

// typical values
// float Vin = 192.0;
// float Rsense = 0.055;
// float Ao = 13.0;
// float La = 0.142;
// float Ra = 11.0;
// float fsampling = 7000.0;
void Signals_Set_Channel_PI_Parameters (unsigned char which_channel, antenna_st * ant)
{
    float Vin = 192.0;
    float fsampling = 7000.0;
    float Rsense = 0.055;
    
    float La = ant->inductance_int + ant->inductance_dec / 100.0;
    La = La / 1000.0;    // convert mHy to Hy

    float Ra = ant->resistance_int + ant->resistance_dec / 100.0;

    float max_antenna_current = ant->current_limit_int + ant->current_limit_dec / 100.0;
    
    // float b0 = Rsense * Ao;
    float b0 = 0.715;    
    b0 = b0 / (La * fsampling);

    float a1 = -1.0 + (Ra + Rsense)/(La * fsampling);

    float a1_pos = -a1;

    // float multi = max_antenna_current * 887. / 1000.;
    float multi = max_antenna_current * 887.;    

    float gain = b0 /(1. - a1_pos);
    float gain_five_perc = 19. /(gain * Vin);
#ifdef TESTING_SHOW_INFO
    printf(" filter gain: %f prop gain: %f gain 5 perc: %f\n", gain, gain * Vin, gain_five_perc);
#endif
    // calc ki from kp and zero eq to pole freq
    float zero_eq_pole = Ra/(La * 6.28);
    float ki_gain = zero_eq_pole * gain_five_perc * 6.28 * 128. * 10. / fsampling;
    
    short ki = (short) ki_gain;    //ki = kp * 2 * np.pi * 20 / fs    
    short kp = (short) (gain_five_perc * 128);
#ifdef TESTING_SHOW_INFO
    printf(" pid values ki: %d kp: %d\n", ki, kp);
    printf(" zero from ki_eff: %f to kp: %fHz\n",
           (float) ki * fsampling,
           (float) ki * fsampling / (6.28 * (float) kp));
#endif
    
    switch (which_channel)
    {
    case CH1:
        global_signals.ki_ch1 = ki;        
        global_signals.kp_ch1 = kp;
        global_signals.max_c_ch1 = (unsigned short) multi;
        break;

    case CH2:
        global_signals.ki_ch2 = ki;
        global_signals.kp_ch2 = kp;
        global_signals.max_c_ch2 = (unsigned short) multi;        
        break;

    case CH3:
        global_signals.ki_ch3 = ki;
        global_signals.kp_ch3 = kp;
        global_signals.max_c_ch3 = (unsigned short) multi;        
        break;

    case CH4:
        global_signals.ki_ch4 = ki;
        global_signals.kp_ch4 = kp;
        global_signals.max_c_ch4 = (unsigned short) multi;        
        break;
    }        
}


void Signals_Stop_All_Channels (void)
{
    signal_index = 0;    //next start will be in phase
    
    HIGH_LEFT_PWM_CH1(DUTY_NONE);
    LOW_RIGHT_PWM_CH1(DUTY_NONE);
    
    HIGH_LEFT_PWM_CH2(DUTY_NONE);
    LOW_RIGHT_PWM_CH2(DUTY_NONE);

    HIGH_LEFT_PWM_CH3(DUTY_NONE);    
    LOW_RIGHT_PWM_CH3(DUTY_NONE);
    
    HIGH_LEFT_PWM_CH4(DUTY_NONE);
    LOW_RIGHT_PWM_CH4(DUTY_NONE);    
}


void Signals_Stop_Single_Channel (unsigned char which_channel)
{
    switch (which_channel)
    {
    case CH1:
        HIGH_LEFT_PWM_CH1(DUTY_NONE);
        LOW_RIGHT_PWM_CH1(DUTY_NONE);
        break;

    case CH2:
        HIGH_LEFT_PWM_CH2(DUTY_NONE);
        LOW_RIGHT_PWM_CH2(DUTY_NONE);
        break;

    case CH3:
        HIGH_LEFT_PWM_CH3(DUTY_NONE);    
        LOW_RIGHT_PWM_CH3(DUTY_NONE);
        break;

    case CH4:
        HIGH_LEFT_PWM_CH4(DUTY_NONE);
        LOW_RIGHT_PWM_CH4(DUTY_NONE);    
        break;
            
    }    
}


//--- end of file ---//
