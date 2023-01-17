#include "dimmer.h"
#include "hal.h"
#include "spi.h"
#include <xc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#pragma config FOSC = INTOSCIO // oscillator selection; use internal oscillator
#pragma config WDTE = ON      // enable watchdog Timer
#pragma config PWRTE = OFF     // power-up timer disabled
#pragma config MCLRE =                                            \
  OFF // use MCLR pin (internally tied to Vdd) as a digital input
#pragma config BOREN = OFF // disable Brown-Out Reset
#pragma config CPD = OFF   // data code protection disabled
#pragma config CP = OFF    // code protection disabled


void init(void) {
  OSCCONbits.IRCF = 0b111;
  OSCCONbits.SCS = 1;
  WDTCON = 0b01001;             /* 512 ms */

  /* __delay_ms(100); */
  /* hal */
  MISOTRIS = INPUT; /* in high Z state */
  MOSITRIS = INPUT;
  SCKTRIS = INPUT;
  CSTRIS = INPUT;
  LAMPTRIS = OUTPUT;
  MAINSTRIS = INPUT;

  ANSEL = 0;
  /* If I want a blasted comparator I'll *ask* for one! */
  CMCON0bits.CM = 0b111;

  /* pullups */
  OPTION_REGbits.nGPPU = 1;
  WPU = 0xff;

  INTCONbits.INTE = 1;          /* interrupt on pin 2 */


  T1CON = 0b00110000;    // Timer 1 off; 1:8 prescaler; use internal clock
  T1CONbits.T1CKPS1 = 0; // bits 5-4  Prescaler Rate Select bits
  T1CONbits.T1CKPS0 = 0; // bit 4

  INTCONbits.PEIE = 1; // enable peripheral interrupts (i.e., Timer 1)
  PIR1bits.TMR1IF = 0; // clear Timer 1 interrupt flag
  PIE1bits.TMR1IE = 1; // enable Timer 1 interrupts

  T1CONbits.TMR1ON = 0; // turn off Timer 1

  INTCONbits.GIE = 1; // enable all interrupts
}



char mosi[] = {0, 0, 0, 0, 0, 0};
char miso[] = {0, 0, 0, 0, 0, 0};

void SpiError(const unsigned char err) {
  char *ptr = miso;
  while (ptr < &miso[6]) {
    *ptr++ = '!';
    *ptr++ = err;
  }
}

void main(void) {
  init();
  unsigned char cmd;
  unsigned char len;
  unsigned char *payloadPtr;
  unsigned int val;
  char *final;

  LAMP = 0;

  while (1) {
    if (!SpiTransaction(mosi, miso, 5))
      continue;
    cmd = mosi[0];

    switch (cmd) {
    case 'p':
      len = mosi[1];
      if (!len || len > 4)
        SpiError('l');
      else
        memcpy(miso, &mosi[2], len);
      break;

    case 'r':
      sprintf(&miso[0], "%4u", duty);
      break;

    case 's':
      val = (unsigned int) strtol(&mosi[1], &final, 10);
      setDuty(val);
      sprintf(miso, "%4u", duty);
      break;

    default:
      SpiError(cmd);
    }
    memset(mosi, 0, sizeof(mosi));
  }
  return;
}
