/*!
*   \sensors.c
*
*   Purpose:            Read various sensors (soil moisture, temp, etc.) and
                        make the data available for use
*
*   Created:            01/11/2017  Morgan Powell
*   Last Modified:
*
*/

/*-----------------------------------------------------------------------------------------------*/
/* Includes                                                                                      */
/*-----------------------------------------------------------------------------------------------*/
/* Standard includes. */
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Hardware library includes. */
#include "hw_memmap.h"
#include "hw_types.h"
#include "hw_sysctl.h"
#include "hw_timer.h"
#include "hw_ints.h"
#include "timer.h"
#include "sysctl.h"
#include "gpio.h"
#include "grlib.h"
#include "rit128x96x4.h"
#include "adc.h"
#include "interrupt.h"

/* My app includes. */
#include "screen.h"
#include "lcd_message.h"
#include "my_adc.h"

/*-----------------------------------------------------------------------------------------------*/
/* Defines                                                                                       */
/*-----------------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------------------------*/
/* Macros                                                                                        */
/*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*/
/* Types                                                                                         */
/*-----------------------------------------------------------------------------------------------*/
typedef struct _Sensor * Sensor;

typedef struct _Sensor
{
    char* name;
    int index;
    int outdex;
    int capacity;
    int* values;
} _Sensor;


/*-----------------------------------------------------------------------------------------------*/
/* Function Prototypes                                                                           */
/*-----------------------------------------------------------------------------------------------*/
CircularBuffer CircularBuffer_Create(int capacity);
void CircularBuffer_Destroy(CircularBuffer);
BOOL CircularBuffer_IsEmpty(CircularBuffer);
BOOL CircularBuffer_IsFull(CircularBuffer);
BOOL CircularBuffer_Put(CircularBuffer, int);
int CircularBuffer_Get(CircularBuffer);
int CircularBuffer_Capacity(CircularBuffer);
void CircularBuffer_Print(CircularBuffer);
BOOL CircularBuffer_VerifyIntegrity(CircularBuffer);

/*-----------------------------------------------------------------------------------------------*/
/* Constants                                                                                     */
/*-----------------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------------------------*/
/* Variables                                                                                     */
/*-----------------------------------------------------------------------------------------------*/
static xSemaphoreHandle xADC_ISRready;
static xSemaphoreHandle xBuffer_Gatekeeper;
static xSignalsBuffer xSignals;

/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/
/* Implementation                                                                                */
/*===============================================================================================*/
/*===============================================================================================*/
/*===============================================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/* Private Functions:					                                                         */
/*-----------------------------------------------------------------------------------------------*/
void vGetSensorValues( void *pvParameters)
{

}

void vSensor_Init(){}
void vSensor_Read(){}
void vSensor_
static void vInitSensors()
{

}


/*-----------------------------------------------------------------------------------------------*/
/* Public Functions                                                                              */
/*-----------------------------------------------------------------------------------------------*/
xQueueHandle vStartSensorModule()
{
    /* Create the queue used by ... */
    xQueueHandle xSensorQueue = xQueueCreate( _QUEUE_SIZE, sizeof( xQueueItem ) );

    vInitSensors();

    xTaskCreate( vGetSensorValues, ( signed portCHAR * ) "sensor read", _TASK_STACK_SIZE, ( void * ) xSensorQueue, tskIDLE_PRIORITY, NULL );

    return ( void * ) xOLEDQueue;
}

/**************************************************************************************************
*   sensor.c
**************************************************************************************************/
