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

#include "hard.h"
#include "antennas_defs.h"


// Module Exported Types Constants and Macros ----------------------------------


// Exported Module Functions ---------------------------------------------------
void Signals_Setup_All_Channels (void);
void Signals_Generate_All_Channels (void);
void Signals_Generate_Channel (unsigned char which_channel, unsigned short new_sp);
void Signals_Stop_All_Channels (void);
void Signals_Stop_Single_Channel (unsigned char which_channel);

void Signals_Generate_Channel_OpenLoop (unsigned char which_channel, short new_ref);
void Signals_Set_Reset_Channel_For_Treatment (unsigned char which_channel, unsigned char state);
void Signals_Set_Channel_PI_Parameters (unsigned char which_channel, antenna_st * ant);


#endif    /* _SIGNALS_H_ */

//--- end of file ---//

