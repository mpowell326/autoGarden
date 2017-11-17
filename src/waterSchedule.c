/*!
*   \waterSchedule.c
*
*   Purpose:            Provide a scheduling system add ercurring and one-off
*                       watering events to.
*
*   Created:            13/11/2017  Morgan Powell
*   Last Modified:
*
*/


/*-----------------------------------------------------------------------------------------------*/
/* Includes                                                                                      */
/*-----------------------------------------------------------------------------------------------*/
/* Standard includes. */
#include <stdio.h>
#include <time.h>

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
#include "sprinkler.h"

/*-----------------------------------------------------------------------------------------------*/
/* Defines                                                                                       */
/*-----------------------------------------------------------------------------------------------*/
#define MAX_EVENT_DESCTIPTION               ( 100 )
#define WATER_SCHEDULE_TASK_STACK_SIZE            ( configMINIMAL_STACK_SIZE )
#define WATER_SCHEDULE_QUEUE_SIZE                 ( 5 )
/*-----------------------------------------------------------------------------------------------*/
/* Macros                                                                                        */
/*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*/
/* Types                                                                                         */
/*-----------------------------------------------------------------------------------------------*/
// struct event {
//     int id;
//     tm start_time;
//     tm duration;
//     bool recurring;
//     char description[MAX_EVENT_DESCTIPTION]
// }

typedef  water_schedule_t;

typedef enum _event{      //TODO: Remove unused items
    ADD,
    REMOVE,
    LIST,
} _event_t;


typedef struct                  //TODO: Remove unused items
{
    _event_t event;
    int num;

} xWaterSchedule_QueueItem;


/*-----------------------------------------------------------------------------------------------*/
/* Function Prototypes                                                                           */
/*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*/
/* Constants                                                                                     */
/*-----------------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------------------------*/
/* Variables                                                                                     */
/*-----------------------------------------------------------------------------------------------*/

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
water_schedule_t pvWaterSchedule_init()
{
    // sprinkler_Initialise();

    // Create array of len MINS_IN_DAY to hold pointers to events for the day.
    // water_schedule_t *day_schedule = malloc(sizeof(*pEvent) * MINS_IN_DAY);
    water_schedule_t day_schedule;
    return day_schedule;
}

void vWaterSchedule_PollTask(void *pvParameters)
{
    water_schedule_t schedule           =  (water_schedule_t) pvParameters;

    while(1)
    {
        // time_t current_time = time(NULL);
        // current_minute = time2minuteinday(current_time);

        // if( sprinkler_isOn == true )
        // {
        //
        // }
        // if (schedule[current_minute] != NULL)
        // {
            sprinkler_TurnOn();
        // }
    }
}

void vWaterSchedule_UpdateEventsTask(void *pvParameters)
{
    // ((struct params){.schedule=schedule,.queue=xWaterSchedule_Queue})
    // xQueueHandle xWaterSchedule_Queue   = (struct {water_schedule_t schedule, xQueueHandle queue }) pvParameters).queue;
    // water_schedule_t schedule           = (struct params{water_schedule_t schedule, xQueueHandle queue }) pvParameters.schedule;

    xWaterSchedule_QueueItem xWaterSchedule_Item;


    while(1)
    {
        /* Wait for a message to arrive and update schedule with event. */
        // if( xQueueReceive( xWaterSchedule_Queue, &xWaterSchedule_Item, 0 ) )
        // {
            // pv
        // }
    }
}
/*-----------------------------------------------------------------------------------------------*/
/* Public Functions                                                                              */
/*-----------------------------------------------------------------------------------------------*/
xQueueHandle pvWaterSchedule_StartModule()
{
    water_schedule_t schedule   = pvWaterSchedule_init();
    // water_schedule_t schedule = xWaterSchedule_init();

    /* Create the queue used by the Schedule task to add new events. */
    xQueueHandle xWaterSchedule_Queue = xQueueCreate( WATER_SCHEDULE_QUEUE_SIZE, sizeof( xWaterSchedule_QueueItem ) );


    xTaskCreate( vWaterSchedule_PollTask,       ( signed portCHAR * ) "waterschedulePollTask",  WATER_SCHEDULE_TASK_STACK_SIZE, (void *) schedule, tskIDLE_PRIORITY+1, NULL );
    // xTaskCreate( vWaterSchedule_UpdateEvents,   ( signed portCHAR * ) "waterscheduleUpdateTask", WATER_SCHEDULE_TASK_STACK_SIZE, \
                                                                        (void *) ((struct {water_schedule_t schedule, xQueueHandle queue }){schedule,xWaterSchedule_Queue}), tskIDLE_PRIORITY+1, NULL );


    return ( void * ) xWaterSchedule_Queue;
}
//
// void pvWaterSchedule_AddEvent(event)
// {
//
// }
//
// void pvWaterSchedule_RemoveEvent()
// {
//
// }

// water_schedule_t * xWaterSchedule_ListEvents()
// {
//
// }

/**************************************************************************************************
*   waterSchedule.c
**************************************************************************************************/
