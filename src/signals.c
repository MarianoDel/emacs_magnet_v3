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
#include "hard.h"
// #include "stm32f0xx.h"
#include "tim.h"
#include "dsp.h"
#include "adc.h"
#include "pwm.h"

// #include "uart.h"
// #include "gpio.h"
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
#define CH1    0
#define CH2    1
#define CH3    2
#define CH4    3

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
// treatment_e treatment_state = TREATMENT_INIT_FIRST_TIME;
// signals_struct_t signal_to_gen;
// gen_signal_state_e gen_signal_state = GEN_SIGNAL_INIT;
// unsigned char global_error = 0;

// unsigned short * p_signal;
// unsigned short * p_signal_running;

// short d = 0;
// short ez1 = 0;
// short ez2 = 0;
unsigned short phase_accum = 0;

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
void Signal_UpdatePointerReset (void);
resp_t Signal_UpdatePointer (void);
void Signal_DrawingReset (void);
resp_t Signal_Drawing (void);
void Signal_OffsetCalculate (void);

void Signal_Generate_Phase_0_90_120 (void);
void Signal_Generate_Phase_180 (void);
void Signal_Generate_Phase_240 (void);

void Signals_Generate_Channel (unsigned char which_channel, unsigned short new_sp);

// Module Functions ------------------------------------------------------------
// void StopTreatment (void)
// {
//     if (treatment_state != TREATMENT_STANDBY)
//         treatment_state = TREATMENT_STOPPING;
// }

// error_t GetErrorStatus (void)
// {
// 	error_t error = ERROR_OK;

// 	if (global_error & ERROR_OVERTEMP_MASK)
// 		error = ERROR_OVERTEMP;
// 	else if (global_error & ERROR_OVERCURRENT_MASK)
// 		error = ERROR_OVERCURRENT;
// 	else if (global_error & ERROR_NO_CURRENT_MASK)
// 		error = ERROR_NO_CURRENT;
// 	else if (global_error & ERROR_SOFT_OVERCURRENT_MASK)
// 		error = ERROR_SOFT_OVERCURRENT;

// 	return error;
// }

// void SetErrorStatus (error_t e)
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

// //recibe tipo de senial
// //setea senial y offset
// resp_t SetSignalTypeAndOffset (signal_type_t a, signal_offset_t offset)
// {
//     if ((treatment_state != TREATMENT_INIT_FIRST_TIME) && (treatment_state != TREATMENT_STANDBY))
//         return resp_error;

//     if (offset <= TWO_HUNDRED_FORTY_DEG_OFFSET)
//         signal_to_gen.offset = offset;
//     else
//         return resp_error;

//     if (a == SQUARE_SIGNAL)
//     {
//         p_signal = (unsigned short *) s_square_3A;

//         pid_param_p = PID_SQUARE_P;
//         pid_param_i = PID_SQUARE_I;
//         pid_param_d = PID_SQUARE_D;

//         signal_to_gen.signal = a;
//     }
// #if (defined USE_PROTECTION_WITH_INT) && (defined INT_SPEED_RESPONSE)
//     else if (a == TRIANGULAR_SIGNAL)
//     {
//         p_signal = (unsigned short *) s_triangular_6A;

//         pid_param_p = PID_TRIANGULAR_P;
//         pid_param_i = PID_TRIANGULAR_I;
//         pid_param_d = PID_TRIANGULAR_D;
        
//         signal_to_gen.signal = a;
//     }
// #else
//     else if (a == TRIANGULAR_SIGNAL)
//     {
//         p_signal = (unsigned short *) s_triangular_3A;

//         pid_param_p = PID_TRIANGULAR_P;
//         pid_param_i = PID_TRIANGULAR_I;
//         pid_param_d = PID_TRIANGULAR_D;

//         signal_to_gen.signal = a;        
//     }
// #endif
//     else if (a == SINUSOIDAL_SIGNAL)
//     {
//         p_signal = (unsigned short *) s_sinusoidal_3A;

//         pid_param_p = PID_SINUSOIDAL_P;
//         pid_param_i = PID_SINUSOIDAL_I;
//         pid_param_d = PID_SINUSOIDAL_D;

//         signal_to_gen.signal = a;
//     }
//     else
//         return resp_error;
        
//     return resp_ok;
// }

//recibe referencia a la estructura de senial
//recibe tipo de senial
// resp_t SetSignalType (signals_struct_t * s, signal_type_t a)
// {
//     //TODO: despues cargar directamente los k
//     if ((treatment_state != TREATMENT_INIT_FIRST_TIME) && (treatment_state != TREATMENT_STANDBY))
//         return resp_error;

//     if (a == SQUARE_SIGNAL)
//         p_signal = (unsigned short *) s_cuadrada_1_5A;

// #if (defined USE_PROTECTION_WITH_INT) && (defined INT_SPEED_RESPONSE)
//     if (a == TRIANGULAR_SIGNAL)
//         p_signal = (unsigned short *) s_triangular_6A;
// #else
//     if (a == TRIANGULAR_SIGNAL)
//         p_signal = (unsigned short *) s_triangular_1_5A;    
// #endif

//     if (a == SINUSOIDAL_SIGNAL)
//         p_signal = (unsigned short *) s_senoidal_1_5A;

//     // signal_to_gen.signal = a;
//     s->signal = a;

//     return resp_ok;
// }

//setea la frecuencia y el timer con el que se muestrea
//por default o error es simepre de 1500Hz -> seniales de 10Hz
// resp_t SetFrequency (unsigned char entero, unsigned char decimal)
// {
//     if ((treatment_state != TREATMENT_INIT_FIRST_TIME) && (treatment_state != TREATMENT_STANDBY))
//         return resp_error;

//     if (decimal >= 100)
//         return resp_error;
    
//     if ((entero >= FREQ_ALLOWED_MIN) && (entero <= FREQ_ALLOWED_MAX))
//     {
//         signal_to_gen.freq_int = entero;
//         signal_to_gen.freq_dec = decimal;
//     }

//     return resp_ok;
// }

// resp_t SetPower (unsigned char a)
// {
//     if ((treatment_state != TREATMENT_INIT_FIRST_TIME) && (treatment_state != TREATMENT_STANDBY))
//         return resp_error;

//     if (a > 100)
//         signal_to_gen.power = 100;
//     else if (a < 10)
//         signal_to_gen.power = 10;
//     else
//         signal_to_gen.power = a;

//     return resp_ok;
// }

// //verifica que se cumplan con todos los parametros para poder enviar una senial coherente
// resp_t AssertTreatmentParams (void)
// {
//     resp_t resp = resp_error;

//     if ((signal_to_gen.power > 100) || (signal_to_gen.power < 10))
//         return resp;

//     //reviso frecuencia a generar
//     if (signal_to_gen.freq_dec >= 100)
//         return resp;
    
//     if ((signal_to_gen.freq_int < FREQ_ALLOWED_MIN) &&
//         (signal_to_gen.freq_int > FREQ_ALLOWED_MAX))
//         return resp;

//     if (signal_to_gen.signal > SINUSOIDAL_SIGNAL)
//         return resp;

//     if (signal_to_gen.offset > TWO_HUNDRED_FORTY_DEG_OFFSET)
//         return resp;
    
//     //TODO: revisar tambien puntero  senial!!!!
//     return resp_ok;
// }

// void SendAllConf (void)
// {
//     char b [64];

//     //muestro el canal
//     sprintf(b, "channel: %s\n", GetOwnChannel());
//     Usart1Send(b);

//     //muestro la senial
//     switch (signal_to_gen.signal)
//     {
//     case SQUARE_SIGNAL:
//         Usart1Send("signal: SQUARE\n");        
//         break;

//     case TRIANGULAR_SIGNAL:
//         Usart1Send("signal: TRIANGULAR\n");
//         break;

//     case SINUSOIDAL_SIGNAL:
//         Usart1Send("signal: SINUSOIDAL\n");
//         break;

//     default:
//         Usart1Send("signal: error !not loaded!\n");
//         break;
//     }
    
//     //muestro la frecuencia
//     sprintf(b, "freq: %d.%02dHz\n",
//             signal_to_gen.freq_int,
//             signal_to_gen.freq_dec);
    
//     Usart1Send(b);

//     //muestro el offset
//     switch (signal_to_gen.offset)
//     {
//     case ZERO_DEG_OFFSET:
//         Usart1Send("offset: 0deg\n");        
//         break;

//     case NINTY_DEG_OFFSET:
//         Usart1Send("offset: 90deg\n");
//         break;

//     case HUNDRED_TWENTY_DEG_OFFSET:
//         Usart1Send("offset: 120deg\n");
//         break;

//     case HUNDRED_EIGHTY_DEG_OFFSET:
//         Usart1Send("offset: 180deg\n");
//         break;

//     case TWO_HUNDRED_FORTY_DEG_OFFSET:
//         Usart1Send("offset: 240deg\n");
//         break;
        
//     default:
//         Usart1Send("offset: !not loaded!\n");
//         break;
//     }
    
//     //muestro el t1
//     sprintf(b, "t1[100us]: %d\n", signal_to_gen.t1);
//     Usart1Send(b);

//     //muestro la potencia
//     sprintf(b, "power: %d\n\n", signal_to_gen.power);
//     Usart1Send(b);
// }

// //reset a antes de la generacion de seniales
// void GenerateSignalReset (void)
// {
//     gen_signal_state = GEN_SIGNAL_INIT;
// }

/////////////////////////////////////////////////////////////////
// FUNCIONES SIGNAL_GENERATE_PHASE                             //
// son 3, se encargan de dibujar la senial teniendo en cuenta: //
// * sincronismo                                               //
// * fases seleccionada                                        //
//                                                             //
// Funciones:                                                  //
//  void Signal_Generate_Phase_0_90_120 (void)                  //
//  void Signal_Generate_Phase_180 (void)                      //      
//  void Signal_Generate_Phase_240 (void)                      //
//                                                             //
/////////////////////////////////////////////////////////////////

// Funcion que llama el manager para generar la senial en el canal
// utiliza la senial de synchro desde el puerto serie
// para dibujar la senial llama a Signal_Drawing()
// necesito conocer la fase a generar (hardcoded en la funcion)
// los tiempos de espera, dependen de la frecuencia y de la fase
// el control de soft_overcurrent debiera salir de aca, ya que conozco cuando dibujo o cuando no
// siempre fast discharge hasta que tiene que generar que pasa a normal discharge
// cuando se termine de generar el que llama a esta funcion debera poner normal discharge
// void Signal_Generate_Phase_0_90_120 (void)
// {

//     switch (gen_signal_state)
//     {
//     case GEN_SIGNAL_INIT:
//         SIGNAL_PWM_NORMAL_DISCHARGE;
//         gen_signal_state = GEN_SIGNAL_WAIT_FOR_SYNC;

//         //sync
//         sync_on_signal = 0;

//         //no current
// #ifdef USE_SOFT_NO_CURRENT
//         current_integral_running = 0;
//         current_integral_ended = 0;
// #endif
//         break;

//     case GEN_SIGNAL_WAIT_FOR_SYNC:
//         if (sync_on_signal)
//         {
// #ifdef LED_SHOW_SYNC_SIGNAL
//             if (LED)
//                 LED_OFF;
//             else
//                 LED_ON;
// #endif

//             sync_on_signal = 0;

//             TIM16->CNT = 0;
//             gen_signal_state = GEN_SIGNAL_WAIT_T1;
//         }
//         break;

//     case GEN_SIGNAL_WAIT_T1:
//         if (TIM16->CNT > signal_to_gen.t1)
//         {
//             SIGNAL_PWM_NORMAL_DISCHARGE;
//             sequence_ready_reset;
            
//             Signal_DrawingReset ();
//             gen_signal_state = GEN_SIGNAL_DRAWING;
//         }
//         break;
            
//     case GEN_SIGNAL_DRAWING:
//         //en este bloque tomo la nueva muestra del ADC
//         //hago update de la senial antes de cada PID
//         //luego calculo el PID y los PWM que correspondan
//         if (sequence_ready)
//         {
//             sequence_ready_reset;    //aprox 7KHz synchro con pwm

// #ifdef LED_SHOW_SEQUENCE
//             if (LED)
//                 LED_OFF;
//             else
//                 LED_ON;
// #endif
            
//             if (Signal_Drawing() == resp_ended) //resuelvo lo referido al final de la senial
//                 gen_signal_state = GEN_SIGNAL_DRAWING_ENDED;
//             else
//             {
//                 //resuelvo lo referido a la senial cuando estoy dibujando
// #ifdef USE_SOFT_NO_CURRENT
//                 current_integral_running += I_Sense;
// #endif                

// #ifdef USE_SOFT_OVERCURRENT
//                 soft_overcurrent_max_current_in_cycles = MA8Circular(I_Sense);
// #endif
//             }
//         }
//         break;

//     case GEN_SIGNAL_DRAWING_ENDED:

//         SIGNAL_PWM_FAST_DISCHARGE;
//         gen_signal_state = GEN_SIGNAL_WAIT_FOR_SYNC;

// #ifdef USE_SOFT_NO_CURRENT
//         current_integral = current_integral_running;
//         current_integral_running = 0;
//         current_integral_ended = 1;
// #endif
//         break;
            
//     case GEN_SIGNAL_STOPPED_BY_INT:		//lo freno la interrupcion
//         break;

//     default:
//         gen_signal_state = GEN_SIGNAL_INIT;
//         break;
//     }
    
// }
    
// // Senial especial de 180 grados de defasaje, en la que el synchro
// // justo cuando estoy terminando de dibujar la senial o apenas terminado
// void Signal_Generate_Phase_180 (void)
// {

//     switch (gen_signal_state)
//     {
//     case GEN_SIGNAL_INIT:
//         SIGNAL_PWM_NORMAL_DISCHARGE;
//         gen_signal_state = GEN_SIGNAL_WAIT_FOR_SYNC;

//         //sync
//         sync_on_signal = 0;

//         //no current
// #ifdef USE_SOFT_NO_CURRENT
//         current_integral_running = 0;
//         current_integral_ended = 0;
// #endif
//         break;

//     case GEN_SIGNAL_WAIT_FOR_SYNC:
//         if (sync_on_signal)
//         {
// #ifdef LED_SHOW_SYNC_SIGNAL
//             if (LED)
//                 LED_OFF;
//             else
//                 LED_ON;
// #endif

//             sync_on_signal = 0;

//             TIM16->CNT = 0;
//             gen_signal_state = GEN_SIGNAL_WAIT_T1;
//         }
//         break;

//     case GEN_SIGNAL_WAIT_T1:
//         if (TIM16->CNT > signal_to_gen.t1)
//         {
//             SIGNAL_PWM_NORMAL_DISCHARGE;
//             sequence_ready_reset;
            
//             Signal_DrawingReset();
//             gen_signal_state = GEN_SIGNAL_DRAWING;
//         }
//         break;
            
//     case GEN_SIGNAL_DRAWING:
//         //en este bloque tomo la nueva muestra del ADC
//         //hago update de la senial antes de cada PID
//         //luego calculo el PID y los PWM que correspondan
//         if (sequence_ready)
//         {
//             sequence_ready_reset;    //aprox 7KHz synchro con pwm

// #ifdef LED_SHOW_SEQUENCE
//             if (LED)
//                 LED_OFF;
//             else
//                 LED_ON;
// #endif
            
//             if (Signal_Drawing() == resp_ended)
//                 gen_signal_state = GEN_SIGNAL_DRAWING_ENDED;
//             else
//             {
//                 //resuelvo lo referido a la senial cuando estoy dibujando                
// #ifdef USE_SOFT_NO_CURRENT
//                 current_integral_running += I_Sense;
// #endif
                
// #ifdef USE_SOFT_OVERCURRENT
//                 soft_overcurrent_max_current_in_cycles = MA8Circular(I_Sense);
// #endif                
//             }
//         }
//         break;

//     case GEN_SIGNAL_DRAWING_ENDED:

//         SIGNAL_PWM_FAST_DISCHARGE;
//         gen_signal_state = GEN_SIGNAL_WAIT_FOR_SYNC;
        
// #ifdef USE_SOFT_NO_CURRENT
//         current_integral = current_integral_running;
//         current_integral_running = 0;
//         current_integral_ended = 1;
// #endif
//         break;
            
//     case GEN_SIGNAL_STOPPED_BY_INT:		//lo freno la interrupcion
//         break;

//     default:
//         gen_signal_state = GEN_SIGNAL_INIT;
//         break;
//     }

//     //el synchro en general me llega al final de GEN_SIGNAL_DRAWING
//     if ((sync_on_signal) &&
//         (gen_signal_state != GEN_SIGNAL_WAIT_FOR_SYNC) &&
//         (gen_signal_state == GEN_SIGNAL_DRAWING))
//     {
//         //no le doy ack al sync
//         gen_signal_state = GEN_SIGNAL_DRAWING_ENDED;
//     }

// }

// // Senial especial, defasaje 240 grados, el synchro llega justo cuando estoy dibujando la senial
// // de todas formas espero el primer sync para arrancar
// // uso sync_on_signal como un doble flag
// void Signal_Generate_Phase_240 (void)
// {

//     switch (gen_signal_state)
//     {
//     case GEN_SIGNAL_INIT:
//         SIGNAL_PWM_NORMAL_DISCHARGE;
//         gen_signal_state = GEN_SIGNAL_WAIT_FOR_FIRST_SYNC;

//         //sync
//         sync_on_signal = 0;

//         //no current
// #ifdef USE_SOFT_NO_CURRENT
//         current_integral_running = 0;
//         current_integral_ended = 0;
// #endif
//         break;
        
//     case GEN_SIGNAL_WAIT_FOR_FIRST_SYNC:
//         if (sync_on_signal)
//         {
//             sync_on_signal = 2;
//             TIM16->CNT = 0;
//             gen_signal_state = GEN_SIGNAL_WAIT_T1;

// #ifdef LED_SHOW_SYNC_SIGNAL
//             if (LED)
//                 LED_OFF;
//             else
//                 LED_ON;
// #endif            
//         }
//         break;

//     case GEN_SIGNAL_WAIT_T1:
//         if ((TIM16->CNT > signal_to_gen.t1) && (sync_on_signal == 2))
//         {            
//             SIGNAL_PWM_NORMAL_DISCHARGE;
//             sequence_ready_reset;
//             sync_on_signal = 0;
            
//             Signal_DrawingReset ();
//             gen_signal_state = GEN_SIGNAL_DRAWING;
//         }
//         break;
            
//     case GEN_SIGNAL_DRAWING:
//         //en este bloque tomo la nueva muestra del ADC
//         //hago update de la senial antes de cada PID
//         //luego calculo el PID y los PWM que correspondan
//         if (sequence_ready)
//         {
//             sequence_ready_reset;    //aprox 7KHz synchro con pwm

// #ifdef LED_SHOW_SEQUENCE
//             if (LED)
//                 LED_OFF;
//             else
//                 LED_ON;
// #endif
            
//             if (Signal_Drawing() == resp_ended)
//                 gen_signal_state = GEN_SIGNAL_DRAWING_ENDED;
//             else
//             {
//                 //resuelvo lo referido a la senial cuando estoy dibujando
// #ifdef USE_SOFT_NO_CURRENT
//                 current_integral_running += I_Sense;
// #endif                                

// #ifdef USE_SOFT_OVERCURRENT
//                 soft_overcurrent_max_current_in_cycles = MA8Circular(I_Sense);
// #endif
//             }
//         }
//         break;

//     case GEN_SIGNAL_DRAWING_ENDED:

//         SIGNAL_PWM_FAST_DISCHARGE;
//         gen_signal_state = GEN_SIGNAL_WAIT_T1;
        
// #ifdef USE_SOFT_NO_CURRENT
//         current_integral = current_integral_running;
//         current_integral_running = 0;
//         current_integral_ended = 1;
// #endif
//         break;
            
//     case GEN_SIGNAL_STOPPED_BY_INT:		//lo freno la interrupcion
//         break;

//     default:
//         gen_signal_state = GEN_SIGNAL_INIT;
//         break;
//     }

//     //el synchro en general me llega en el medio de GEN_SIGNAL_DRAWING
//     //uso sync_on_signal como doble flag
//     if ((sync_on_signal == 1) && (gen_signal_state != GEN_SIGNAL_WAIT_FOR_FIRST_SYNC))
//     {
//         sync_on_signal = 2;
//         TIM16->CNT = 0;

// #ifdef LED_SHOW_SYNC_SIGNAL
//             if (LED)
//                 LED_OFF;
//             else
//                 LED_ON;
// #endif        
//     }
    
// }

// typedef enum {
// 	NORMAL_DISCHARGE = 0,
// 	TAU_DISCHARGE,
// 	FAST_DISCHARGE

// } drawing_state_t;

// drawing_state_t drawing_state = NORMAL_DISCHARGE;

// void Signal_DrawingReset (void)
// {
//     drawing_state = NORMAL_DISCHARGE;
//     d = 0;
//     ez1 = 0;
//     ez2 = 0;

//     Signal_UpdatePointerReset();
// }

// //llamar para cada punto a dibujar
// //calculo PID con puntero anterior y actualizo el puntero
// resp_t Signal_Drawing (void)
// {
//     resp_t resp = resp_continue;
    
//     switch (drawing_state)
//     {
//     case NORMAL_DISCHARGE:
//         d = PID_roof ((*p_signal_running * signal_to_gen.power / 100),
//                       I_Sense,
//                       d,
//                       &ez1,
//                       &ez2);
                    
//         //reviso si necesito cambiar a descarga por tau
//         if (d < 0)
//         {
//             HIGH_LEFT_PWM(0);
//             drawing_state = TAU_DISCHARGE;
//             d = 0;	//limpio para pid descarga
//         }
//         else
//         {
//             if (d > DUTY_95_PERCENT)		//no pasar del 95% para dar tiempo a los mosfets
//                 d = DUTY_95_PERCENT;
            
//             HIGH_LEFT_PWM(d);
//         }

//         if (Signal_UpdatePointer() != resp_continue)
//             resp = resp_ended;

//         break;

//     case TAU_DISCHARGE:		//la medicion de corriente sigue siendo I_Sense
//         d = PID_roof ((*p_signal_running * signal_to_gen.power / 100),
//                       I_Sense,
//                       d,
//                       &ez1,
//                       &ez2);

//         //reviso si necesito cambiar a descarga rapida
//         if (d < 0)
//         {
//             if (-d < DUTY_100_PERCENT)
//                 LOW_RIGHT_PWM(DUTY_100_PERCENT + d);
//             else
//                 LOW_RIGHT_PWM(0);    //descarga maxima

//             drawing_state = FAST_DISCHARGE;
//         }
//         else
//         {
//             //esto es normal
//             if (d > DUTY_95_PERCENT)		//no pasar del 95% para dar tiempo a los mosfets
//                 d = DUTY_95_PERCENT;

//             HIGH_LEFT_PWM(d);
//             drawing_state = NORMAL_DISCHARGE;
//         }

//         if (Signal_UpdatePointer() != resp_continue)
//             resp = resp_ended;

//         break;

//     case FAST_DISCHARGE:		//la medicion de corriente ahora esta en I_Sense_negado
//         d = PID_roof ((*p_signal_running * signal_to_gen.power / 100),
//                       I_Sense_negado,
//                       d,
//                       &ez1,
//                       &ez2);

//         //reviso si necesito cambiar a descarga por tau o normal
//         if (d < 0)
//         {
//             if (-d < DUTY_100_PERCENT)
//                 LOW_RIGHT_PWM(DUTY_100_PERCENT + d);
//             else
//                 LOW_RIGHT_PWM(0);    //descarga maxima
//         }
//         else
//         {
//             //vuelvo a TAU_DISCHARGE
//             LOW_RIGHT_PWM(DUTY_ALWAYS);
//             drawing_state = TAU_DISCHARGE;
//         }

//         if (Signal_UpdatePointer() != resp_continue)
//             resp = resp_ended;

//         break;

//     default:
//         break;
//     }
//     return resp;
// }

// inline void Signal_UpdatePointerReset (void)
// {
//     p_signal_running = p_signal;
// }

// resp_t Signal_UpdatePointer (void)
// {
//     resp_t resp = resp_continue;
//     //si la senial esta corriendo hago update de senial y un par de chequeos
//     //senial del adc cuando convierte la secuencia disparada por TIM1 a 2000Hz 6000Hz o 12000Hz

//     //-- Signal Update --//
//     if ((p_signal_running) < (p_signal + SIZEOF_SIGNALS))
//     {
//         p_signal_running += 1;
//     }
//     else    //termino la senial, aviso
//     {                        
//         resp = resp_ended;
//     }

//     return resp;
// }

// //calculo el offset de la senial, T1 y el sampling
// //el sampling lo seteo en el timer TIM1
// //ticks[us] = 1/freq * 1/2 * 1/100 * 1e6 
// //TODO: HUNDRED_EIGHTY T1 = T * 180 / 360
// void Signal_OffsetCalculate (void)
// {
//     unsigned int ticks = 1000000;
//     unsigned int offset = 1000000;
//     unsigned short freq = 0;
//     unsigned short freq_2 = 0;
    

//     //calculo cantidad de ticks para sampling como 100 puntos en medio ciclo
//     //sampling tick 1us
//     freq = signal_to_gen.freq_int * 100;
//     freq += signal_to_gen.freq_dec;
//     freq_2 = freq * 2;
    
//     ticks = ticks / freq_2;
//     TIM1_ChangeTick((unsigned short) ticks);

//     //calculo el t1 en ticks de 100us segun offset
//     offset = offset / freq;
//     switch (signal_to_gen.offset)
//     {
//     case ZERO_DEG_OFFSET:
//         offset = 0;
//         break;

//     case NINTY_DEG_OFFSET:
//         offset = offset * 90;
//         offset = offset / 360;
//         break;

//     case HUNDRED_TWENTY_DEG_OFFSET:
//         offset = offset * 120;
//         offset = offset / 360;
//         break;

//     case HUNDRED_EIGHTY_DEG_OFFSET:
//         offset = offset * 180;
//         offset = offset / 360;        
//         break;

//     case TWO_HUNDRED_FORTY_DEG_OFFSET:
//         offset = offset * 240;
//         offset = offset / 360;        
//         break;
//     }
//     signal_to_gen.t1 = (unsigned short) offset;
    
// }

// //hubo sobrecorriente, me llaman desde la interrupcion
// void Overcurrent_Shutdown (void)
// {
// #ifdef LED_SHOW_INT
//     LED_ON;
// #endif

//     //primero freno todos los PWM
//     HIGH_LEFT_PWM(0);
//     LOW_RIGHT_PWM(0);

//     DISABLE_TIM3;

//     //freno la generacionde la senial
//     gen_signal_state = GEN_SIGNAL_STOPPED_BY_INT;

//     //ahora aviso del error
//     SetErrorStatus(ERROR_OVERCURRENT);

//     //meto la generacion en Overcurrent
//     timer_signals = 10;
//     treatment_state = TREATMENT_STOPPING2;
// }


const unsigned short * p_table_inphase;
const unsigned short * p_table_outphase;
pi_data_obj_t pi_ch1;
pi_data_obj_t pi_ch2;
pi_data_obj_t pi_ch3;
pi_data_obj_t pi_ch4;
unsigned short signal_index = 0;
unsigned short ch1_max_current = 0;
unsigned short ch2_max_current = 0;
unsigned short ch3_max_current = 0;
unsigned short ch4_max_current = 0;

// just for test, im still thinking how to manage this
signals_struct_t global_signals = {
    // general all channels things
    // .signal = SINUSOIDAL_SIGNAL,
    .signal = SQUARE_SIGNAL,
    // .signal = TRIANGULAR_SIGNAL,

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
    // power * mean * integ (sizeof signal = 256) / 2
    unsigned int mean_current = global_signals.power * signal_mean * 128;
    mean_current = mean_current / 10000;    //adjust power and mean

    signal_no_current_threshold_ch1 = mean_current * ch1_max_current;    //antenna current peak value
    signal_no_current_threshold_ch2 = mean_current * ch2_max_current;    //antenna current peak value
    signal_no_current_threshold_ch3 = mean_current * ch3_max_current;    //antenna current peak value
    signal_no_current_threshold_ch4 = mean_current * ch4_max_current;    //antenna current peak value    

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


unsigned char treat_in_ch1 = 1;
unsigned char treat_in_ch2 = 1;
unsigned char treat_in_ch3 = 1;
unsigned char treat_in_ch4 = 1;
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
    if (signal_index < 65535)
        signal_index += phase_accum;
    else
    {
        signal_index += phase_accum;    //modulo 16 roundup
        signal_ended = 1;
    }
    
    if (treat_in_ch1 == CHANNEL_CONNECTED_GOOD)
        Signals_Generate_Channel (CH1, sp_inphase);

    if (treat_in_ch2 == CHANNEL_CONNECTED_GOOD)
        Signals_Generate_Channel (CH2, sp_inphase);

    if (treat_in_ch3 == CHANNEL_CONNECTED_GOOD)
        Signals_Generate_Channel (CH3, sp_outphase);

    if (treat_in_ch4 == CHANNEL_CONNECTED_GOOD)
        Signals_Generate_Channel (CH4, sp_outphase);

    // now check channels for errors
#ifdef USE_SOFT_NO_CURRENT
    if (signal_ended)
    {
        if (treat_in_ch1 == CHANNEL_CONNECTED_GOOD)
        {
            if (signal_integral_ch1 < signal_no_current_threshold_ch1)
            {
                if (signal_no_current_cnt_ch1 < NO_CURRENT_THRESHOLD_CNT)
                    signal_no_current_cnt_ch1++;
                else
                {
                    treat_in_ch1 = CHANNEL_DISCONNECT;
                    ErrorSetStatus(ERROR_NO_CURRENT, CH1);
                }
            }
        }

        if (treat_in_ch2 == CHANNEL_CONNECTED_GOOD)
        {
            if (signal_integral_ch2 < signal_no_current_threshold_ch2)
            {
                if (signal_no_current_cnt_ch2 < NO_CURRENT_THRESHOLD_CNT)
                    signal_no_current_cnt_ch2++;
                else
                {
                    treat_in_ch2 = CHANNEL_DISCONNECT;
                    ErrorSetStatus(ERROR_NO_CURRENT, CH2);
                }
            }
        }

        if (treat_in_ch3 == CHANNEL_CONNECTED_GOOD)
        {
            if (signal_integral_ch3 < signal_no_current_threshold_ch3)
            {
                if (signal_no_current_cnt_ch3 < NO_CURRENT_THRESHOLD_CNT)
                    signal_no_current_cnt_ch3++;
                else
                {
                    treat_in_ch3 = CHANNEL_DISCONNECT;
                    ErrorSetStatus(ERROR_NO_CURRENT, CH3);
                }
            }
        }

        if (treat_in_ch4 == CHANNEL_CONNECTED_GOOD)
        {
            if (signal_integral_ch4 < signal_no_current_threshold_ch4)
            {
                if (signal_no_current_cnt_ch4 < NO_CURRENT_THRESHOLD_CNT)
                    signal_no_current_cnt_ch4++;
                else
                {
                    treat_in_ch4 = CHANNEL_DISCONNECT;
                    ErrorSetStatus(ERROR_NO_CURRENT, CH4);
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
        if (treat_in_ch1 == CHANNEL_CONNECTED_GOOD)
        {
            unsigned short filter_c = MA8_U16Circular(&signal_ovcp_filter_ch1, IS_CH1);
            if (filter_c > signal_ovcp_threshold_ch1)
            {
                printf("ch1 current filtered: %d threshold: %d sample: %d index: %d\n",
                       filter_c,
                       signal_ovcp_threshold_ch1,
                       IS_CH1,
                       signal_index);

                Signals_Stop_Single_Channel(CH1);
                treat_in_ch1 = CHANNEL_DISCONNECT;
                ErrorSetStatus(ERROR_SOFT_OVERCURRENT, CH1);
            }
        }

        if (treat_in_ch2 == CHANNEL_CONNECTED_GOOD)
        {
            unsigned short filter_c = MA8_U16Circular(&signal_ovcp_filter_ch2, IS_CH2);
            if (filter_c > signal_ovcp_threshold_ch2)
            {
                printf("ch2 current filtered: %d threshold: %d sample: %d index: %d\n",
                       filter_c,
                       signal_ovcp_threshold_ch2,
                       IS_CH2,
                       signal_index);

                Signals_Stop_Single_Channel(CH2);                
                treat_in_ch2 = CHANNEL_DISCONNECT;
                ErrorSetStatus(ERROR_SOFT_OVERCURRENT, CH2);
            }
        }

        if (treat_in_ch3 == CHANNEL_CONNECTED_GOOD)
        {
            unsigned short filter_c = MA8_U16Circular(&signal_ovcp_filter_ch3, IS_CH3);
            if (filter_c > signal_ovcp_threshold_ch3)
            {
                printf("ch3 current filtered: %d threshold: %d sample: %d index: %d\n",
                       filter_c,
                       signal_ovcp_threshold_ch3,
                       IS_CH3,
                       signal_index);

                Signals_Stop_Single_Channel(CH3);                
                treat_in_ch3 = CHANNEL_DISCONNECT;
                ErrorSetStatus(ERROR_SOFT_OVERCURRENT, CH3);
            }
        }

        if (treat_in_ch4 == CHANNEL_CONNECTED_GOOD)        
        {
            unsigned short filter_c = MA8_U16Circular(&signal_ovcp_filter_ch4, IS_CH4);
            if (filter_c > signal_ovcp_threshold_ch4)
            {
                printf("ch4 current filtered: %d threshold: %d sample: %d index: %d\n",
                       filter_c,
                       signal_ovcp_threshold_ch4,
                       IS_CH4,
                       signal_index);

                Signals_Stop_Single_Channel(CH4);
                treat_in_ch4 = CHANNEL_DISCONNECT;
                ErrorSetStatus(ERROR_SOFT_OVERCURRENT, CH4);
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

    // if (sp > 4095)
    //     printf("sp is wrong!!! %d\n", sp);
    
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


void Signals_Stop_All_Channels (void)
{
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
