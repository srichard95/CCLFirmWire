/*
 * measure.h
 *
 *  Created on: 2016 júl. 4
 *      Author: srich
 */

#ifndef INCLUDE_MEASURE_H_
#define INCLUDE_MEASURE_H_

enum measChannels
{
  MEAS_NTC1,
  MEAS_NTC2,
  MEAS_NTC3,
  MEAS_CURR1,
  MEAS_CURR2,
  MEAS_CURR3,
  MEAS_NUM_CH
};

#define SAMPLE_FREQUENCY_MS 50

#define NULL_AMPER_ADC   2040
#define AMP_PER_ADC      43.3

#define LED_ON_AMP 10

/*
 * 5A @ 68
 */

void init_measures();
void cmd_getAdcValues(BaseSequentialStream *chp, int argc, char *argv[]);
int16_t measGetValue(enum measChannels ch);

#endif /* INCLUDE_MEASURE_H_ */
