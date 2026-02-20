//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### UTILS.H ###############################
//---------------------------------------------
#ifndef _UTILS_H_
#define _UTILS_H_

// Exported Types Constants and Macros -----------------------------------------


// Module Exported Functions ---------------------------------------------------
unsigned char StringIsANumber (char * pn, unsigned short * number);
unsigned short GetValue (unsigned char * pn, char delimiter);
unsigned char StringCheckNumbers (char * str, unsigned char len);
unsigned char StringCorrelate (char * str_orig, char * str_cmp);


#endif    /* _UTILS_H_ */

//--- end of file ---//

