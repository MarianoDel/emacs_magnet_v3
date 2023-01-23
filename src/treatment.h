//---------------------------------------------------------
// #### PROYECTO STRETCHER MAGNETO F103 - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### TREATMENT.H ############################################
//---------------------------------------------------------

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TREATMENT_H_
#define __TREATMENT_H_
#include "hard.h"    //para respuestas


//--- Exported types ---//
typedef enum {
	SQUARE_SIGNAL = 0,
	TRIANGULAR_SIGNAL,
	SINUSOIDAL_SIGNAL

} signal_type_t;

typedef enum {
	CH1_ENABLE = 1,
	CH2_ENABLE,
	CH3_ENABLE,
	CH1_DISABLE,
	CH2_DISABLE,
	CH3_DISABLE

} ch_in_treatment_t;

//esta esta relacionada con F030 src/signal.h
//si se modifica cambiar las dos!!!
typedef struct {
	signal_type_t signal;
	unsigned char freq_int;
	unsigned char freq_dec;    
	// unsigned char freq_table_inc;
	unsigned char power;
	unsigned char synchro_needed;

	//internals
	// unsigned short kprop;
	// unsigned short kinteg;
	// unsigned short kderv;

} signals_struct_t;

typedef enum {
    UPDWN_AUTO = 0,
    UPDWN_MANUAL

} updwn_e;

typedef struct {
    signals_struct_t treatment_signal;
    unsigned short treatment_time;
    unsigned char channels_in_treatment;
    unsigned short timer_synchro;
    updwn_e updwn_conf;
    
} treatment_conf_t;

typedef enum {
    TREATMENT_STANDBY = 0,
    TREATMENT_STARTING,
    TREATMENT_RUNNING,
    TREATMENT_PAUSED,
    TREATMENT_WITH_ERRORS,
    TREATMENT_STOPPING,
    TREATMENT_STOPPING2,
    MAIN_IN_BRIDGE_MODE

} treatment_t;

typedef enum {
	ERROR_OK = 0,
	ERROR_OVERCURRENT,
	ERROR_NO_CURRENT,
	ERROR_SOFT_OVERCURRENT,
	ERROR_OVERTEMP

} error_t;


//TIPO de descarga y estado de signal
typedef enum
{
	INIT_DISCHARGE = 0,
	NORMAL_DISCHARGE,
	TAU_DISCHARGE,
	FAST_DISCHARGE,
	STOPPED_BY_INT

} discharge_state_t;


//--- Exported constants ---//
#define ENABLE_CH1_FLAG    0x41
#define ENABLE_CH2_FLAG    0x42
#define ENABLE_CH3_FLAG    0x44

#define DISABLE_CH1_FLAG    0x81
#define DISABLE_CH2_FLAG    0x82
#define DISABLE_CH3_FLAG    0x84

//timer synchro cuenta cada 100us
#define TIMER_SYNCHRO_MIN     100    //minimo en 100Hz
#define TIMER_SYNCHRO_MAX     2000    //maximo en 5Hz
#define FREQ_MIN_ALLOWED    5
#define FREQ_MAX_ALLOWED    100


//--- Exported macro ---//
//esto es 100 mas el ajuste
#define K_SYNCHRO_ADJUST    108

#define ERROR_OVERCURRENT_MASK			0x01
#define ERROR_NO_CURRENT_MASK				0x02
#define ERROR_OVERTEMP_MASK				0x04
#define ERROR_SOFT_OVERCURRENT_MASK		0x08
#define ERROR_FLUSH_MASK					0xff

#define SIZEOF_OVERCURRENT_BUFF			8



//--- Exported functions ---//
resp_t TreatmentSetSignalType (signal_type_t);
signal_type_t TreatmentGetSignalType (void);
resp_t TreatmentSetFrequency (unsigned char, unsigned char);
void TreatmentGetFrequency (unsigned char *, unsigned char *);
resp_t TreatmentSetPower (unsigned char);
unsigned char TreatmentGetPower (void);
resp_t TreatmentSetTime (unsigned char, unsigned char, unsigned char);
resp_t TreatmentSetTimeinMinutes (unsigned short);
unsigned short TreatmentGetTime (void);
void TreatmentGetAllConf (char *);
resp_t TreatmentAssertParams (void);
void TreatmentSetChannelsFlag (unsigned char);
unsigned char TreatmentGetChannelsFlag (void);
unsigned short TreatmentGetSynchroTimer (void);
void TreatmentUpdateMainState (treatment_t);
treatment_t TreatmentGetMainState (void);

void TreatmentSetUpDwn (updwn_e);
updwn_e TreatmentGetUpDwn (void);

#endif
//--- END OF FILE ---//
