#include "dimmer.h"
#include "hal.h"
#include <xc.h>

static unsigned int period = 0;

static unsigned int calcPeriod(unsigned int period) {
  return 0xffff - ((_XTAL_FREQ / 4000000) * period);
}

void setDuty(unsigned int duty) {
  period = duty? calcPeriod((unsigned long)10000 * duty / 0x3ff) : 0;
}

void dimmerISR(void) {
  if (PIR1bits.TMR1IF == 1) {
    LAMP = 0;
    PIR1bits.TMR1IF = 0;  // clear interrupt flag
    T1CONbits.TMR1ON = 0; // turn on Timer 1
  }
  if (INTCONbits.GPIF) {
    INTCONbits.GPIF = 0;        /* clear flag */
    if (!period) return;

    TMR1 = period;
    T1CONbits.TMR1ON = 1; // turn on Timer 1
    LAMP = 1;
  }
}