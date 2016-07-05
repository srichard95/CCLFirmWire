/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "test.h"
#include "console.h"
#include "CanComm.h"
#include "measure.h"

#define ANIMATION_SPEED 300

void SwitchOffLeds(){
  palClearPad(GPIOB, GPIOB_LED3);
  palClearPad(GPIOB, GPIOB_LED1);
  palClearPad(GPIOB, GPIOB_LED2);
}

void SwitchOnAnimation(){

  chThdSleepMilliseconds(ANIMATION_SPEED);
  SwitchOffLeds();
  palSetPad(GPIOB, GPIOB_LED3);

  chThdSleepMilliseconds(ANIMATION_SPEED);
  SwitchOffLeds();
  palSetPad(GPIOB, GPIOB_LED2);

  chThdSleepMilliseconds(ANIMATION_SPEED);
  SwitchOffLeds();
  palSetPad(GPIOB, GPIOB_LED1);

  chThdSleepMilliseconds(ANIMATION_SPEED);
  SwitchOffLeds();
  palSetPad(GPIOB, GPIOB_LED2);

  chThdSleepMilliseconds(ANIMATION_SPEED);
  SwitchOffLeds();
  palSetPad(GPIOB, GPIOB_LED3);

  chThdSleepMilliseconds(ANIMATION_SPEED);
}

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  consoleInit();
  chThdSleepMilliseconds(100);



  palClearPad(GPIOB, GPIOB_LED3);
  palClearPad(GPIOB, GPIOB_LED1);
  palClearPad(GPIOB, GPIOB_LED2);

  SwitchOnAnimation();
  init_measures();
  CanCommInit();

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state, when the button is
   * pressed the test procedure is launched.
   */
  while (true) {
    consoleStart();
    chThdSleepMilliseconds(200);
  }
}
