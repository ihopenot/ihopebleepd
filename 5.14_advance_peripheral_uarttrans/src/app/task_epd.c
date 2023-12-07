#include <stdio.h>
#include <string.h>

#include <xdc/std.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>

#include "board.h"
//#include <ti/drivers/uart/UARTCC26XX.h>
#include "task_epd.h"


// if defined, not calling actual epd function, just test protocol
//#define EPD_DRY_RUN

#ifndef EPD_DRY_RUN
#include "epd2in13.h"
#endif
 
 // for debug
//#include "inc/sdi_task.h"

#include "hw_uart.h"

#include "util.h"

uint8_t VERSION_MAJOR = 0;
uint8_t VERSION_MINOR = 2;

#define ASSERT_INIT \
  if (!epd_initialized) { \
    epd_resp_frame[1] = 0xfc; \
    return; \
  }
 
#define ASSERT_MIN_LEN(val, min_len) \
	if (val < min_len)               \
	{                                \
    epd_resp_frame[1] = 0xfe; \
		return;                    \
	}
 
enum {
  EPD_CMD_INIT = 0x0,
  EPD_CMD_CLEAR = 0x1,
  EPD_CMD_WRITE_RAM = 0x2,
  EPD_CMD_UPDATE = 0x3,
  EPD_CMD_PREPARE_BLK = 0x4,
  EPD_CMD_PREPARE_RED = 0x5,
  EPD_CMD_SHUTDOWN = 0x6,
};
 
 
 
#define EPD_TASK_PRIORITY                     2
#define EPD_TASK_STACK_SIZE                   900
Task_Struct EPDTask;
Char EPDTaskStack[EPD_TASK_STACK_SIZE];

// cc2640 received epd command frame from andoid app
// first byte is length, second is command, third and follow are command data if any
static uint8_t epd_rx_frame[256];
uint8_t rx_fram_len = 0;

static uint8_t epd_resp_frame[6];
uint8_t resp_fram_len = 1;

EpdResponseCallback respCallback = NULL;

// Event used to control the EPD thread
Event_Struct EPDEvent;
Event_Handle hEPDEvent;

#define EPDTASK_EVENT_RX_REQUEST      Event_Id_00 


#define EPDTASK_EVENT_ALL ( EPDTASK_EVENT_RX_REQUEST  )
                            
                                         

static void handle_cmd();       
void TaskEPD_taskFxn(UArg a0, UArg a1);
static void post_epd_response(uint8_t *buf, uint16_t len);


void TaskEPD_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = EPDTaskStack;
  taskParams.stackSize = EPD_TASK_STACK_SIZE;
  taskParams.priority = EPD_TASK_PRIORITY;

  Task_construct(&EPDTask, TaskEPD_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      TaskEPD_taskInit
 *
 * @brief   ���ڳ�ʼ��
 *
 * @param   None
 *
 * @return  None.
 */
void TaskEPD_taskInit(void)
{
    HWUART_Printf("TaskEPD_taskInit\r\n");
    
#ifndef EPD_DRY_RUN
     epd_hw_init();
#endif
    
    
}

/*********************************************************************
 * @fn      TaskEPD_taskFxn
 *
 * @brief   ����������
 *
 * @param   None
 *
 * @return  None.
 */
void TaskEPD_taskFxn(UArg a0, UArg a1)
{ 
  Event_Params evParams;
  Event_Params_init(&evParams);
  Event_construct(&EPDEvent, &evParams);
  hEPDEvent = Event_handle(&EPDEvent);
  
  TaskEPD_taskInit();

  while(1)
  {
    UInt events;
    events = Event_pend(hEPDEvent,Event_Id_NONE, EPDTASK_EVENT_ALL, BIOS_WAIT_FOREVER);
    
    if(events & EPDTASK_EVENT_RX_REQUEST)
    {
      handle_cmd();
      
      
      // debug
   //   Util_delay_ms(10*1000);
      // send response
      if (resp_fram_len)
      {
        post_epd_response(epd_resp_frame, resp_fram_len);
      }
      
    }
       
  }
}


void EPDTask_RegisterResponseCallback(EpdResponseCallback callback)
{
    respCallback = callback;
}

//void epd_on_rx_cmd(const uint8_t *buf, int len)
//{
//    memcpy(epd_rx_frame, buf, len);
//    Event_post(hEPDEvent, EPDTASK_EVENT_RX_REQUEST);
//}



void post_epd_response(uint8_t *buf, uint16_t len)
{
    
    if (respCallback != NULL) {
        // 0x10 is the event id , should be the same in peripheral_uarttrans.c
        respCallback(0x10, buf, len);
    }

}

bool epd_initialized = false;
void handle_cmd()
{
  uint8_t cmd = epd_rx_frame[0];
  
  resp_fram_len  = 2;
  epd_resp_frame[0] = cmd;
  epd_resp_frame[1] = 0;

  switch(cmd) {
    case EPD_CMD_INIT:
      ASSERT_MIN_LEN(rx_fram_len, 2);
      if (epd_rx_frame[1] <= EPD_MODE_BEGIN || epd_rx_frame[1] >= EPD_MODE_END) {
        epd_resp_frame[1] = 0xff;
        return;
      }
      EPD_Init_With_Mode(epd_rx_frame[1]);
      epd_initialized = true;
      break;
    case EPD_CMD_CLEAR:
      ASSERT_MIN_LEN(rx_fram_len, 2);
      EPD_Clear(epd_rx_frame[1]);
      break;
    case EPD_CMD_PREPARE_BLK:
      ASSERT_INIT;
      EPD_2IN13_PrepareBlkRAM();
      break;
    case EPD_CMD_PREPARE_RED:
      ASSERT_INIT;
      EPD_2IN13_PrepareRedRAM();
      break;
    case EPD_CMD_WRITE_RAM:
      ASSERT_INIT;
      EPD_2IN13_WriteRAM(epd_rx_frame + 1, rx_fram_len - 1);
      break;
    case EPD_CMD_UPDATE:
      ASSERT_INIT;
      EPD_Display();
      epd_initialized = false;
      break;
    default:
      HWUART_Printf("unknown cmd\r\n");
      epd_resp_frame[1] = 0xfd;
      break;
  }

}

void EPDTask_parseCommand(uint8_t *pMsg, uint8_t length)
{
  memcpy(epd_rx_frame, pMsg, length);
  rx_fram_len = length;
    Event_post(hEPDEvent, EPDTASK_EVENT_RX_REQUEST);
  
}