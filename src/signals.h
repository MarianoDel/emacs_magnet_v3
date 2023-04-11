//------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### SIGNALS.H ################################
//------------------------------------------------
#ifndef _SIGNALS_H_
#define _SIGNALS_H_
// #include "comm.h"		//para respuestas
#include "hard.h"
#include "signals_defs.h"
#include "answers_defs.h"


// Module Exported Types Constants and Macros ----------------------------------
// typedef enum {
// 	SQUARE_SIGNAL = 0,
// 	TRIANGULAR_SIGNAL,
// 	SINUSOIDAL_SIGNAL

// } signal_type_e;

typedef enum {
	ZERO_DEG_OFFSET = 0,
        NINTY_DEG_OFFSET,
	HUNDRED_TWENTY_DEG_OFFSET,
        HUNDRED_EIGHTY_DEG_OFFSET,
        TWO_HUNDRED_FORTY_DEG_OFFSET

} signal_offset_e;


typedef struct {
    // general all channels things
    signal_type_e signal;

    unsigned char freq_int;
    unsigned char freq_dec;

    unsigned char power;    

    // by channel things    
    unsigned short kp_ch1;
    unsigned short ki_ch1;
    unsigned short kp_ch2;
    unsigned short ki_ch2;
    unsigned short kp_ch3;
    unsigned short ki_ch3;
    unsigned short kp_ch4;
    unsigned short ki_ch4;

    unsigned short max_c_ch1;
    unsigned short max_c_ch2;
    unsigned short max_c_ch3;
    unsigned short max_c_ch4;

    unsigned char treat_in_ch1;
    unsigned char treat_in_ch2;
    unsigned char treat_in_ch3;
    unsigned char treat_in_ch4;
    
} signals_struct_t;

typedef enum {
	TREATMENT_INIT_FIRST_TIME = 0,
	TREATMENT_STANDBY,
	TREATMENT_START_TO_GENERATE,
	TREATMENT_GENERATING,
	TREATMENT_STOPPING,
	TREATMENT_STOPPING2,
        TREATMENT_JUMPER_PROTECTED,
        TREATMENT_JUMPER_PROTECT_OFF

} treatment_e;

typedef enum {
	ERROR_OK = 0,
	ERROR_OVERCURRENT,
	ERROR_NO_CURRENT,
	ERROR_SOFT_OVERCURRENT,
	ERROR_OVERTEMP

} error_e;


//TIPO de descarga y estado de signal
typedef enum
{
	GEN_SIGNAL_INIT = 0,
        GEN_SIGNAL_WAIT_FOR_SYNC,
        GEN_SIGNAL_WAIT_FOR_FIRST_SYNC,        
        GEN_SIGNAL_WAIT_T1,
        GEN_SIGNAL_DRAWING,
        GEN_SIGNAL_WAIT_T2,
	GEN_SIGNAL_STOPPED_BY_INT,
        GEN_SIGNAL_DRAWING_ENDED

} gen_signal_state_e;


//--- Exported constants ---//
#define TAU_LR_MILLIS    9

//--- Exported macro ---//
// #define SIZEOF_SIGNALS		100

#define ERROR_OVERCURRENT_MASK			0x01
#define ERROR_NO_CURRENT_MASK				0x02
#define ERROR_OVERTEMP_MASK				0x04
#define ERROR_SOFT_OVERCURRENT_MASK		0x08
#define ERROR_FLUSH_MASK					0xff

#define CURRENT_INTEGRAL_MAX_ERRORS        SIGNALS_ADMITED_WITH_NO_CURRENT
#define CURRENT_INTEGRAL_THRESHOLD         350    //generalmente da de 300 a 329 @10Hz

#define FREQ_ALLOWED_MIN    5
#define FREQ_ALLOWED_MAX    100

#define SAMPLE_TIME_10HZ    500
#define SAMPLE_TIME_30HZ    166
#define SAMPLE_TIME_60HZ    83

#define FlushErrorStatus() SetErrorStatus(ERROR_FLUSH_MASK)

#define SIGNAL_PWM_FAST_DISCHARGE do {\
        HIGH_LEFT_PWM (0);\
        LOW_RIGHT_PWM (0);\
    } while (0)

#ifdef VER_2_0
#define SIGNAL_PWM_NORMAL_DISCHARGE do {\
        HIGH_LEFT_PWM (0);\
        LOW_RIGHT_PWM (DUTY_ALWAYS);\
    } while (0)
#endif

#if (defined VER_1_0) || (defined VER_1_1)
#define SIGNAL_PWM_NORMAL_DISCHARGE do {\
        HIGH_LEFT_PWM(0);
        LOW_LEFT_PWM(0);
        HIGH_RIGHT_PWM(0);
        LOW_RIGHT_PWM(DUTY_ALWAYS);
    } while (0)
#endif

//--- Exported functions ---//
// resp_e SetSignalType (signals_struct_t *, signal_type_t);
resp_e SetSignalTypeAndOffset (signal_type_e, signal_offset_e);
resp_e SetFrequency (unsigned char, unsigned char);
resp_e SetPower (unsigned char);
void GenerateSignalReset (void);

resp_e AssertTreatmentParams (void);
resp_e StartTreatment (void);
void StopTreatment (void);
error_e GetErrorStatus (void);
void SetErrorStatus (error_e);
void SendAllConf (void);
void TreatmentManager (void);
void Overcurrent_Shutdown (void);
void TreatmentManager_IntSpeed (void);

treatment_e GetTreatmentState (void);
gen_signal_state_e GetGenSignalState (void);

void Signals_Setup_All_Channels (void);
void Signals_Generate_All_Channels (void);
void Signals_Generate_Channel (unsigned char which_channel, unsigned short new_sp);
void Signals_Stop_All_Channels (void);
void Signals_Stop_Single_Channel (unsigned char which_channel);

void Signals_Generate_Channel_OpenLoop (unsigned char which_channel, short new_ref);
void Signals_Set_Reset_Channel_For_Treatment (unsigned char which_channel, unsigned char state);

#endif    /* _SIGNALS_H_ */

//--- end of file ---//

