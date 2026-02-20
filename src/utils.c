//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### UTILS.C ###############################
//---------------------------------------------

// Includes --------------------------------------------------------------------
#include "utils.h"

#include <string.h>
#include <stdlib.h>


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
//devuelve los numeros de un string, en la posicion number
//devuele la cantidad de cifras leidas
unsigned char StringIsANumber (char * pn, unsigned short * number)
{
    unsigned char i;
    char new_number [6] = {0};

    //no mas de 6 caracteres
    for (i = 0; i < 6; i++)
    {
        if ((*(pn + i) < '0') || (*(pn + i) > '9'))
            break;

        new_number[i] = *(pn + i);
    }

    if (i > 0)
        *number = atoi(new_number);

    return i;
}


// get the numeric value from a string before the delimiter character ex. ','
unsigned short GetValue (unsigned char * pn, char delimiter)
{
	unsigned char i;
	unsigned char colon = 0;
	unsigned short new_val = 0xffff;

	//me fijo la posiciones de la , o ;
	for (i = 0; i < 6; i++)
	{
		if (*(pn + i) == delimiter)
		{
			colon = i;
			i = 6;
		}
	}

	if ((colon == 0) || (colon >= 5))
		return 0;

	switch (colon)
	{
		case 1:
			new_val = *pn - '0';
			break;

		case 2:
			new_val = (*pn - '0') * 10 + (*(pn + 1) - '0');
			break;

		case 3:
			new_val = (*pn - '0') * 100 + (*(pn + 1) - '0') * 10 + (*(pn + 2) - '0');
			break;

		case 4:
			new_val = (*pn - '0') * 1000 + (*(pn + 1) - '0') * 100 + (*(pn + 2) - '0') * 10 + (*(pn + 2) - '0');
			break;

	}
	return new_val;
}


//check if all the chars in string are numbers, answer num qtty
unsigned char StringCheckNumbers (char * str, unsigned char len)
{
    unsigned char qtty;

    for (qtty = 0; qtty < len; qtty++)
    {
        if ((*(str + qtty) < '0') || (*(str + qtty) > '9'))
            break;
    }

    return qtty;
}


unsigned char StringCorrelate (char * str_orig, char * str_cmp)
{
    unsigned char cnt = 0;
    unsigned char len = strlen(str_orig);

    for (int i = 0; i < len; i++)
    {
	if (*(str_orig + i) == *(str_cmp + i))
	    cnt++;
    }

    return cnt;
}
//--- end of file ---//
