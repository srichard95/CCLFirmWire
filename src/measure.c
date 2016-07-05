/*
 * measure.c
 *
 *  Created on: 2016 júl. 4
 *      Author: srich
 */

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "measure.h"

#define ADC_GRP1_BUF_DEPTH      8
int curr_adc;

/*
* Calibration of NTC temperature from -55 C to 155 C in 5 C steps
*/
#define MEAS_NTCCAL_NUM 43
#define MEAS_NTCCAL_START -55
#define MEAS_NTCCAL_STEP 5
const adcsample_t measNTCcalib[] = {
 4054, 4036, 4011, 3978, 3934, 3877, 3804, 3713, 3602, 3469,
 3313, 3136, 2939, 2726, 2503, 2275, 2048, 1828, 1618, 1424,
 1245, 1085,  942,  816,  706,  611,  528,  458,  397,  344,
  300,  261,  228,  199,  175,  153,  135,  120,  106,   94,
   83,   74,   66
} ;


static int32_t measValue[MEAS_NUM_CH +2];
static adcsample_t samples[MEAS_NUM_CH * ADC_GRP1_BUF_DEPTH];

static const ADCConversionGroup adcgrpcfg = {
  FALSE,
  MEAS_NUM_CH,
  NULL,
  NULL,
  0,                                                                       /* CR1 */
  0,                                                         /* CR2 */
  NULL,
  ADC_SMPR2_SMP_AN4(ADC_SAMPLE_1P5) | ADC_SMPR2_SMP_AN5(ADC_SAMPLE_1P5) |
  ADC_SMPR2_SMP_AN2(ADC_SAMPLE_1P5) | ADC_SMPR2_SMP_AN3(ADC_SAMPLE_1P5) |
  ADC_SMPR2_SMP_AN0(ADC_SAMPLE_1P5) | ADC_SMPR2_SMP_AN1(ADC_SAMPLE_1P5),
  ADC_SQR1_NUM_CH(MEAS_NUM_CH),
  NULL,
  ADC_SQR3_SQ5_N(ADC_CHANNEL_IN4) | ADC_SQR3_SQ6_N(ADC_CHANNEL_IN5) |
  ADC_SQR3_SQ3_N(ADC_CHANNEL_IN2) | ADC_SQR3_SQ4_N(ADC_CHANNEL_IN3) |
  ADC_SQR3_SQ1_N(ADC_CHANNEL_IN0) | ADC_SQR3_SQ2_N(ADC_CHANNEL_IN1)
};


int16_t measInterpolateNTC(adcsample_t rawvalue){
  int16_t value = 0;
  adcsample_t left, right;
  int i;

  if(rawvalue >= measNTCcalib[0]){
    value = MEAS_NTCCAL_START;
  }
  else if(rawvalue < measNTCcalib[MEAS_NTCCAL_NUM - 1]){
    value = MEAS_NTCCAL_START + (MEAS_NTCCAL_NUM - 1) * MEAS_NTCCAL_STEP;
  }
  else {
    i = 0;
    left = measNTCcalib[i];
    right = measNTCcalib[i + 1];
    while(rawvalue < right && i < MEAS_NTCCAL_NUM - 1){
      i++;
      left = right;
      right = measNTCcalib[i + 1];
    }
    value = MEAS_NTCCAL_START +
            i * MEAS_NTCCAL_STEP +
            (MEAS_NTCCAL_STEP * (left - rawvalue)) / (left - right);
  }
  return value;
}


static THD_WORKING_AREA(waSampleThread, 512);
static THD_FUNCTION(SampleThread, arg) {
  chRegSetThreadName("SampleThread");
  systime_t time;
  time = chVTGetSystemTime();
  while(true)
  {
    time += MS2ST(SAMPLE_FREQUENCY_MS);

    int avg, ch, i;
    double temp = 0;

    adcConvert(&ADCD1, &adcgrpcfg, samples, ADC_GRP1_BUF_DEPTH);

    for(ch = 0; ch < MEAS_NUM_CH; ch++) {
      avg = 0;
      for(i = 0; i < ADC_GRP1_BUF_DEPTH; i++) {
        avg += samples[ch + MEAS_NUM_CH * i];
      }
      avg /= ADC_GRP1_BUF_DEPTH;

      switch(ch)
      {
        case MEAS_NTC1:
        case MEAS_NTC2:
        case MEAS_NTC3:
          avg = measInterpolateNTC(avg);
          break;
        case MEAS_CURR1:
        case MEAS_CURR2:
        case MEAS_CURR3:
          temp = avg - NULL_AMPER_ADC;
          temp /= AMP_PER_ADC;
          temp = -temp;
          avg = (int)(temp * 10);
          break;
      }
      chSysLock();
      measValue[ch] = (int16_t)avg;
      chSysUnlock();
    }

    if(measGetValue(MEAS_CURR1) >= LED_ON_AMP)
      palSetPad(GPIOB, GPIOB_LED1);
    else
      palClearPad(GPIOB, GPIOB_LED1);

    if(measGetValue(MEAS_CURR2) >= LED_ON_AMP)
      palSetPad(GPIOB, GPIOB_LED2);
    else
      palClearPad(GPIOB, GPIOB_LED2);

    if(measGetValue(MEAS_CURR3) >= LED_ON_AMP)
      palSetPad(GPIOB, GPIOB_LED3);
    else
      palClearPad(GPIOB, GPIOB_LED3);
    chThdSleepUntil(time);
  }
}


int16_t measGetValue(enum measChannels ch){
      return measValue[ch];
}


void init_measures()
{
  adcStart(&ADCD1, NULL);
  chThdCreateStatic(waSampleThread, sizeof(waSampleThread), NORMALPRIO, SampleThread, NULL);
}

void cmd_getAdcValues(BaseSequentialStream *chp, int argc, char *argv[]) {

  (void)argc;
  (void)argv;
  chprintf(chp, "\x1B\x63");
  chprintf(chp, "\x1B[2J");
  while (chnGetTimeout((BaseChannel *)chp, TIME_IMMEDIATE) == Q_TIMEOUT) {
    chprintf(chp, "\x1B\x63");
    chprintf(chp, "\x1B[2J");

    chprintf(chp, "CHANNELS: %d, BUFFER: %d\r\n", MEAS_NUM_CH, ADC_GRP1_BUF_DEPTH);
    chprintf(chp, "NTC1: %d\r\n", measGetValue(MEAS_NTC1));
    chprintf(chp, "NTC2: %d\r\n", measGetValue(MEAS_NTC2));
    chprintf(chp, "NTC3: %d\r\n", measGetValue(MEAS_NTC3));
    chprintf(chp, "CURR1: %d\r\n", measGetValue(MEAS_CURR1));
    chprintf(chp, "CURR2: %d\r\n", measGetValue(MEAS_CURR2));
    chprintf(chp, "CURR3: %d\r\n", measGetValue(MEAS_CURR3));

    chThdSleepMilliseconds(200);
  }

}
