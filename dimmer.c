#include "dimmer.h"
#include "hal.h"
#include <xc.h>

static unsigned int period = 0;

unsigned int duty = 0;

static unsigned int calcPeriod(unsigned int period) {
  return 0xffff - ((_XTAL_FREQ / 4000000) * period);
}

void setDuty(unsigned int val) {
  if (val > 1023)
    val = 1023;
  unsigned int buf;
  buf = val? calcPeriod((unsigned long)10000 * val / 0x3ff) : 0;
  di();
  period = buf;
  ei();
  duty = val;
}


void __interrupt() dimmerISR(void) {
  if (PIR1bits.TMR1IF) {
    LAMP = 0;
    T1CONbits.TMR1ON = 0; // turn off Timer 1
    PIR1bits.TMR1IF = 0;  /* clear flag */
  }
  if (INTCONbits.INTF) {

    T1CONbits.TMR1ON = 0; // turn off Timer 1
    PIR1bits.TMR1IF = 0;  /* clear flag */
    INTCONbits.INTF = 0; /* clear flag */
    OPTION_REGbits.INTEDG ^= 1; /* swap interrupt edge */

    if (!period) {
      LAMP = 0;                 /* clear conflict state */
      return;
    }

    TMR1 = period;
    T1CONbits.TMR1ON = 1; // turn on Timer 1
    LAMP = 1;
  }
}
