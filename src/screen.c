/*!
*   \screen.c
*
*   Purpose:            Contains implemtation for displaying data and drawing signals
*						on the stellaris's OLED display.
*
*   Created:            11/08/2016  Morgan Powell
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
#include "sysctl.h"
#include "gpio.h"
#include "grlib.h"
#include "rit128x96x4.h"

#include "bitmap.h"
#include "screen.h"
#include "lcd_message.h"
#include "bmp.h"

/*-----------------------------------------------------------------------------------------------*/
/* Defines                                                                                       */
/*-----------------------------------------------------------------------------------------------*/
/*  y=95 ^
         |
         |
         |
         |
    y=0  |------------>
        x=0           x=127
*/
#define OLED_WIDTH                          ( 127 )
#define OLED_HEIGHT                         ( 95 )
#define BORDER_WIDTH                        ( 1 )


#define TEXT_HEIGHT                         ( 9 )
#define TEXT_BRIGHTNESS                     ( 15 )



/* Layout defintions */
#define HEADER_HEIGHT                       (TEXT_HEIGHT +1)*2

#define BORDER_TOP                          (HEADER_HEIGHT+1)
#define BORDER_BOT                          (OLED_HEIGHT)




#define SCREEN_REFRESH_RATE                 10      //Hz
/* Constants used when writing strings to the display. */
#define mainMAX_ROWS_128                    ( TEXT_HEIGHT * 14 )
#define mainMAX_ROWS_96                     ( TEXT_HEIGHT * 10 )
#define mainMAX_ROWS_64                     ( TEXT_HEIGHT * 7 )


#define ulSSI_FREQUENCY                     ( 3500000UL )



/* Dimensions the buffer into which the jitter time is written. */
#define mainMAX_MSG_LEN                     25

/* The OLED task uses the sprintf function so requires a little more stack too. */
#define mainOLED_TASK_STACK_SIZE            ( configMINIMAL_STACK_SIZE + 50 )

/* The maximum number of message that can be waiting for display at any one
time. */
#define mainOLED_QUEUE_SIZE                 ( 1 )


/*-----------------------------------------------------------------------------------------------*/
/* Macros                                                                                        */
/*-----------------------------------------------------------------------------------------------*/
#define LINE(y)             ( (y) * (TEXT_HEIGHT + 1) )
#define COL(x)              ( ((x) == 0) ? (0) : (OLED_WIDTH/2 + 1)  )


//To adjust for the screen pixel location being inverted
#define X(x)                ( (x) )
#define Y(y)                ( OLED_HEIGHT - (y) )

/*-----------------------------------------------------------------------------------------------*/
/* Types                                                                                         */
/*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*/
/* Function Prototypes                                                                           */
/*-----------------------------------------------------------------------------------------------*/
/* Functions to access the OLED. */
void ( *vOLEDInit )( unsigned portLONG ) =  RIT128x96x4Init;
void ( *vOLEDStringDraw )( const portCHAR *, unsigned portLONG, unsigned portLONG, unsigned portCHAR ) = RIT128x96x4StringDraw;
void ( *vOLEDImageDraw )( const unsigned portCHAR *, unsigned portLONG, unsigned portLONG, unsigned portLONG, unsigned portLONG ) = RIT128x96x4ImageDraw;
void ( *vOLEDClear )( void ) = RIT128x96x4Clear;

/*-----------------------------------------------------------------------------------------------*/
/* Constants                                                                                     */
/*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*/
/* Variables                                                                                     */
/*-----------------------------------------------------------------------------------------------*/
/* The welcome text. */
static const portCHAR * const pcWelcomeMessage = "   ENCE463: Assg 1";
static Canvas  Graph =
{
    .x0 = 2,//BORDER_WIDTH+1,                                  // 2
    .y0 = BORDER_WIDTH,                                  // 1
    .xmax = OLED_WIDTH - BORDER_WIDTH,                   // 127-1 = 126
    .ymax = OLED_HEIGHT - (HEADER_HEIGHT+BORDER_WIDTH)   // 95 -(20+1) =74          width = 123, height=74-1
};
static unsigned char pcGraphBuffer[ ((OLED_WIDTH - BORDER_WIDTH)-BORDER_WIDTH) * (( OLED_HEIGHT-(HEADER_HEIGHT+BORDER_WIDTH)) - BORDER_WIDTH) ];

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
static void vScreenClrSect(xmin, xmax, ymin, ymax)
{
    int row;
    int col;
    for(row=ymin;row<=ymax;row++)
    {
        for(col=xmin;col<=xmax;col++)
        {
            vOLEDImageDraw(g_blankScreen, col, row, 4,4 );
        }
    }
}


static void vScreen_drawLayout()
{
    int row;
    int col;

    // Draw left and right borders
    for(row=BORDER_TOP; row <= BORDER_BOT; row++)
    {
        vOLEDImageDraw( pixel_L_lght, 0, row, 2, 1 );
    }
    for(row=BORDER_TOP; row <= BORDER_BOT; row++)
    {
        vOLEDImageDraw( pixel_R_lght, OLED_WIDTH, row, 2, 1);
    }
    // Draw top and bottom borders
    vOLEDImageDraw( hor_line_lgt, 0, BORDER_TOP, OLED_WIDTH, 1 );
    vOLEDImageDraw( hor_line_lgt, 0, BORDER_BOT, OLED_WIDTH, 1 );
}


static char vScreen_getPixel(int x, int brightness)
{
    if(brightness == 15){
        if( x % 2 == 0)
        {
            return 0xF0;
        }
        else
        {
            return 0x0F;
        }
    }
    else{
        if( x % 2 == 0)
        {
            return 0x30;
        }
        else
        {
            return 0x03;
        }
    }
}


static void vScreen_plotGraph(int voltage,int brightness)
{
    static int t = 1, y;
    unsigned char pixel[1];

    y = (Graph.height-4)*voltage/3000+2;
    pixel[0] = vScreen_getPixel(t, brightness);


    vOLEDImageDraw( pixel, t/2, Y(y), 2, 1 );

    if( ++t >= 2*Graph.xmax)
    {
        t = Graph.x0;
        vOLEDImageDraw(g_blankScreen, Graph.x0, Y(Graph.ymax-1), Graph.width+1, Graph.height );
        pixel[0] = 0x00;
    }

}


static void vScreen_drawGraph(int* volt_array, int brightness)
{
    static unsigned char bitmap[62*73];
    int x,y;

    for (x=0;x<=(62*73);x++)
    {
        bitmap[x] = 0x00;
    }
    for( x=0; x<Graph.width/2; x++)
    {
        // left pixel in byte
        y = (Graph.height-4)* volt_array[2*x] /3000+2;
        bitmap[Graph.width/2 * (Graph.height-y) + (Graph.height-y) + x] |= vScreen_getPixel(2*x, brightness);

        //right pixel in byte
        y = (Graph.height-4)* volt_array[2*x+1] /3000+2;
        bitmap[Graph.width/2 * (Graph.height-y) + (Graph.height-y) + x] |= vScreen_getPixel(2*x+1, brightness);
    }

    vOLEDImageDraw( bitmap, Graph.x0, Y(Graph.ymax-1), Graph.width, Graph.height );
}


static void vScreenDisplayMessage( char *str )
{
    static unsigned portLONG ulMaxY = mainMAX_ROWS_96;
    static unsigned portLONG ulY = mainMAX_ROWS_96;
    static portCHAR cMessage[ mainMAX_MSG_LEN ];


    /* Write the message on the next available row. (i.e. scrolling messages) */
    ulY += TEXT_HEIGHT;
    if( ulY >= ulMaxY )
    {
        ulY = TEXT_HEIGHT;
        vScreenClrSect(0,127,LINE(3),LINE(9)+TEXT_HEIGHT);
        vOLEDStringDraw( pcWelcomeMessage, 0, 0, TEXT_BRIGHTNESS );
        ulY = LINE(3);
    }

    /* Display the message */
    sprintf( cMessage, "%s", str);
    vOLEDStringDraw( str, 0, ulY, TEXT_BRIGHTNESS );



}


static void vScreenUpdateTask( void *pvParameters)
{
    xQueueHandle xOLEDQueue = (xQueueHandle) pvParameters;
    xScreenQueueItem xScreenItem;

    portCHAR cMessage[ 100 ];

    // Wait for splash screen to display for 1 second
    vTaskDelay(1000);
    vOLEDClear();

    // Draw the default screen layout
    vScreen_drawLayout();


    while(1)
    {
        /* Wait for a message to arrive that requires displaying. */
        if( xQueueReceive( xOLEDQueue, &xScreenItem, 0 ) ) {

            switch( xScreenItem.event )
            {
                case MESSAGE:
                    sprintf( cMessage, "%d", xScreenItem.num);
                    vScreenDisplayMessage(cMessage);
                    break;

                case MESSAGE_ADC:
                    sprintf( cMessage, "ADC0: %04d",  xScreenItem.adc0_avg);
                    vOLEDStringDraw( cMessage, COL(0), LINE(0), TEXT_BRIGHTNESS );
                    sprintf( cMessage, "   -+ %04d", xScreenItem.adc0_amp);
                    vOLEDStringDraw( cMessage, COL(0), LINE(1), TEXT_BRIGHTNESS );

                    sprintf( cMessage, "ADC1: %04d", xScreenItem.adc1_avg);
                    vOLEDStringDraw( cMessage, COL(1), LINE(0), 4 );
                    sprintf( cMessage, "   -+ %04d", xScreenItem.adc1_amp);
                    vOLEDStringDraw( cMessage, COL(1), LINE(1), 4 );


                    vScreen_plotGraph(xScreenItem.adc0,15);
                    vScreen_plotGraph(xScreenItem.adc1,4);

                    // TODO: Implement new display method to allow changing timscale
                    // vScreen_drawGraph(xScreenItem.wave1, 4);
                    // vScreen_drawGraph(xScreenItem.wave0, 15);
                    break;

                case MESSAGE_ADC0:
                    sprintf( cMessage, "ADC0: %04d", xScreenItem.num);
                    vOLEDStringDraw( cMessage, COL(0), LINE(0), TEXT_BRIGHTNESS );
                    break;

                case MESSAGE_ADC0_AMP:
                    sprintf( cMessage, "   -+ %d", xScreenItem.num);
                    vOLEDStringDraw( cMessage, COL(0), LINE(1), TEXT_BRIGHTNESS );
                    break;

                case MESSAGE_ADC1:
                    sprintf( cMessage, "ADC1: %04d", xScreenItem.num);
                    vOLEDStringDraw( cMessage, COL(1), LINE(0), TEXT_BRIGHTNESS );
                    break;

                case MESSAGE_ADC1_AMP:
                    sprintf( cMessage, "   -+ %d", xScreenItem.num);
                    vOLEDStringDraw( cMessage, COL(1), LINE(1), TEXT_BRIGHTNESS );
                    break;

                case WAVEFORM_0:
                    vScreen_plotGraph(xScreenItem.num,15);
                    break;

                case WAVEFORM_1:
                    vScreen_plotGraph(xScreenItem.num,4);
                    break;
            }
        }
        vTaskDelay(100/SCREEN_REFRESH_RATE );
    }
}


static void vScreenInitTask()
{
    const unsigned portCHAR *pucImage = pucBasicBitmap;

    /* Initialise the OLED */
    vOLEDInit( ulSSI_FREQUENCY );

    // Set the size of the graph window and link to bitmap buffer
    Graph.content = pcGraphBuffer;
    Graph.width = Graph.xmax-Graph.x0;
    Graph.height = Graph.ymax-Graph.y0;

    //Draw splash screen
    RIT128x96x4_BMP(X(64),Y(10), g_horse);
    vOLEDImageDraw(g_blankScreen, 0, 0, 4,4 );
}


/*-----------------------------------------------------------------------------------------------*/
/* Public Functions                                                                              */
/*-----------------------------------------------------------------------------------------------*/
xQueueHandle pvStartScreenModule()
{
    /* Create the queue used by the OLED task.  Messages for display on the OLED
    are received via this queue. */
    xQueueHandle xOLEDQueue = xQueueCreate( mainOLED_QUEUE_SIZE, sizeof( xScreenQueueItem ) );

    vScreenInitTask();

    xTaskCreate( vScreenUpdateTask, ( signed portCHAR * ) "screenUpdate", mainOLED_TASK_STACK_SIZE, ( void * ) xOLEDQueue, tskIDLE_PRIORITY+1, NULL );

    return ( void * ) xOLEDQueue;
}

/**************************************************************************************************
*   screen.c
**************************************************************************************************/
