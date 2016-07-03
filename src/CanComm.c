/*
 * CanComm.c
 *
 *  Created on: 2016 jún. 6
 *      Author: srich
 */

#include "ch.h"
#include "hal.h"
#include "CanComm.h"



static const CANConfig cancfg = {
 CAN_MCR_ABOM,
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
static THD_WORKING_AREA(can_tx_wa, 256);
static THD_FUNCTION(can_tx, p) {
  CANTxFrame txmsg;

  (void)p;
  chRegSetThreadName("transmitter");
  txmsg.IDE = CAN_IDE_EXT;
  txmsg.EID = 0x01234567;
  txmsg.RTR = CAN_RTR_DATA;
  txmsg.DLC = 8;
  txmsg.data32[0] = 0x55AA55AA;
  txmsg.data32[1] = 0x00FF00FF;

  while (!chThdShouldTerminateX()) {
    txmsg.EID = 0x01234567;
    canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, MS2ST(100));
    txmsg.EID++;
    canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, MS2ST(100));
    txmsg.EID++;
    canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, MS2ST(100));
    chThdSleepMilliseconds(200);
  }
}


void CanCommInit(){
  /*
   * Activates the CAN driver 1.
   */
  canStart(&CAND1, &cancfg);

  /*
   * Starting the transmitter and receiver threads.
   */
  //chThdCreateStatic(can_rx_wa, sizeof(can_rx_wa), NORMALPRIO+7, can_rx, NULL);
  //chThdCreateStatic(waSendingThread, sizeof(waSendingThread), NORMALPRIO + 7, SendingThread, NULL);
  //chThdCreateStatic(can_tx_wa, sizeof(can_tx_wa), NORMALPRIO + 7, can_tx, NULL);
}
