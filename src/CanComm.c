/*
 * CanComm.c
 *
 *  Created on: 2016 jún. 6
 *      Author: srich
 */

#include "ch.h"
#include "hal.h"
#include "CanComm.h"
#include "measure.h"

#define MIN_CAN_EID         0x40

static uint16_t id;
int counter=0;
CANTxFrame txmsg, txmsg2;

static const CANConfig cancfg = {
 CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
 CAN_BTR_SJW(0) | CAN_BTR_TS2(1) |
 CAN_BTR_TS1(8) | CAN_BTR_BRP(11)
};
/*
static const CANConfig cancfg = {
  CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
  CAN_BTR_LBKM | CAN_BTR_SJW(0) | CAN_BTR_TS2(1) |
  CAN_BTR_TS1(8) | CAN_BTR_BRP(6)
};*/
/*
static const CANConfig cancfg = {
  CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
  CAN_BTR_SJW(0) | CAN_BTR_TS2(1) |
  CAN_BTR_TS1(8) | CAN_BTR_BRP(5)
};*/


static THD_WORKING_AREA(waSendingThread, 256);
static THD_FUNCTION(SendingThread, arg) {
  chRegSetThreadName("Sending Thread");
  systime_t time;
  time = chVTGetSystemTime();
  while(true)
  {
    time += MS2ST(DATAFREQ);

    chThdSleepUntil(time);
  }
}

/*
 * Receiver thread.
 */
/*
static THD_WORKING_AREA(can_rx_wa, 256);
static THD_FUNCTION(can_rx, p) {
  event_listener_t el;
  CANRxFrame rxmsg;

  (void)p;
  chRegSetThreadName("receiver");
  chEvtRegister(&CAND1.rxfull_event, &el, 0);
  while(!chThdShouldTerminateX()) {
    if (chEvtWaitAnyTimeout(ALL_EVENTS, MS2ST(100)) == 0)
      continue;
    while (canReceive(&CAND1, CAN_ANY_MAILBOX, &rxmsg, TIME_IMMEDIATE) == MSG_OK) {

    }
  }
  chEvtUnregister(&CAND1.rxfull_event, &el);
}*/

/*
 * Transmitter thread.
 */
void cmd_getID(BaseSequentialStream *chp, int argc, char *argv[]) {

  chprintf(chp, "txmsg: %02x\r\n", txmsg.EID);
}

static THD_WORKING_AREA(can_tx_wa, 256);
static THD_FUNCTION(can_tx, p) {

  chRegSetThreadName("CanTransmit");
  systime_t time;
  time = chVTGetSystemTime();
  while(true)
  {



    time += MS2ST(SAMPLE_FREQUENCY_MS);

      chSysLock();
      txmsg.IDE = CAN_IDE_EXT;

         txmsg.EID = 0;
         txmsg.EID = 0x01;
         txmsg.EID += id << 8;

         txmsg.RTR = CAN_RTR_DATA;
         txmsg.DLC = 8;
         txmsg.data16[0] = measGetCurr(0);
         txmsg.data16[1] = measGetCurr(1);
         txmsg.data16[2] = measGetCurr(2);
         txmsg.data16[3] = 0x00000000;


       canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, MS2ST(100));

       counter++;
       if (counter==5)
       {
           txmsg2.IDE = CAN_IDE_EXT;

          txmsg2.EID = 0;
           txmsg2.EID = 0x02;
           txmsg2.EID += id << 8;

           txmsg2.RTR = CAN_RTR_DATA;
           txmsg2.DLC = 8;
           txmsg2.data8[0] = (int8_t)measGetValue(MEAS_NTC1);
           txmsg2.data8[1] = (int8_t)measGetValue(MEAS_NTC2);
           txmsg2.data8[2] = (int8_t)measGetValue(MEAS_NTC3);
           txmsg2.data8[3] = 0x00000000;
           txmsg2.data8[4] = 0x00000000;
           txmsg2.data8[5] = 0x00000000;
           txmsg2.data8[6] = 0x00000000;
           txmsg2.data8[7] = 0x00000000;


//barna, piros, narancs, fekete

         canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg2, MS2ST(100));
         counter=0;
       }
         chSysUnlock();


    chThdSleepUntil(time);
  }




}


void CanCommInit(){

  id = MIN_CAN_EID;

    id += (palReadPad(GPIOB, GPIOB_ADDR1) == 0) ? 0x01 : 0x00;
    id += (palReadPad(GPIOA, GPIOA_ADDR2) == 0) ? 0x02 : 0x00;
    id += (palReadPad(GPIOA, GPIOA_ADDR3) == 0) ? 0x04 : 0x00;


  /*
   * Activates the CAN driver 1.
   */
  canStart(&CAND1, &cancfg);

  /*
   * Starting the transmitter and receiver threads.
   */
  //chThdCreateStatic(can_rx_wa, sizeof(can_rx_wa), NORMALPRIO+7, can_rx, NULL);
  //chThdCreateStatic(waSendingThread, sizeof(waSendingThread), NORMALPRIO + 7, SendingThread, NULL);
  chThdCreateStatic(can_tx_wa, sizeof(can_tx_wa), NORMALPRIO + 7, can_tx, NULL);
}
