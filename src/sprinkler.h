/*!
*   \sprinkler.h
*
*   Purpose:            An automated watering system using soil moisture levels,
*                       weather forecasts, and scheduling to keep my vege garden
*                       watered!
*
*   Created:            01/11/2017  Morgan Powell
*   Last Modified:
*
*/


#ifndef SPRINKLER_H
#define SPRINKLER_H

#include <stdbool.h>
/*-----------------------------------------------------------------------------------------------*/
/*  Defines                                                                                      */
/*-----------------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------------------------*/
/*  Typedefs                                                                                     */
/*-----------------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------------------------*/
/*  Function Prototypes                                                                          */
/*-----------------------------------------------------------------------------------------------*/
void sprinkler_Initialise();
bool sprinkler_TurnOn();
void sprinkler_TurnOff();




#endif /* SPRINKLER_H */

/**************************************************************************************************
*   End of header file sprinkler.h
**************************************************************************************************/
