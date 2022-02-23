#include "dimmer.h"
#include "hal.h"
#include "spi.h"
#include <xc.h>

#pragma config FOSC = INTOSCIO // oscillator selection; use internal oscillator
#pragma config WDTE = OFF      // disable watchdog Timer
#pragma config PWRTE = OFF     // power-up timer disabled
#pragma config MCLRE =                                            \
  OFF // use MCLR pin (internally tied to Vdd) as a digital input
#pragma config BOREN = OFF // disable Brown-Out Reset
#pragma config CPD = OFF   // data code protection disabled
#pragma config CP = OFF    // code protection disabled

volatile unsigned char last;

void init(void) {
  OSCCONbits.IRCF = 0b111;
  OSCCONbits.SCS = 1;

  __delay_ms(100);
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

  /* INTCONbits.GPIE = 1; /\* interrupt on change on input pins *\/ */
  /* MAINSIOC = 1; */

  /* CSIOC = 1; /\* interrupt on change on CS. *\/ */

  T1CON = 0b00110000;    // Timer 1 off; 1:8 prescaler; use internal clock
  T1CONbits.T1CKPS1 = 0; // bits 5-4  Prescaler Rate Select bits
  T1CONbits.T1CKPS0 = 0; // bit 4

  INTCONbits.PEIE = 1; // enable peripheral interrupts (i.e., Timer 1)
  PIR1bits.TMR1IF = 0; // clear Timer 1 interrupt flag
  /* PIE1bits.TMR1IE = 1; // enable Timer 1 interrupts */

  T1CONbits.TMR1ON = 0; // turn off Timer 1

  last = GPIO;
  /* INTCONbits.GIE = 1; // enable all interrupts */
}



unsigned char payload[6];
unsigned char read[6];

void SpiError(void) {
  for (unsigned char *ptr = payload; ptr < &payload[5]; ptr++)
    *ptr = '!';
  SpiTransaction(6, false, payload, read);
}

void main(void) {
  init();
  unsigned char cmd;
  unsigned char len;
  unsigned char *payloadPtr;
  unsigned int val;

  LAMP = 0;

  /* for (unsigned int i=0; i<1022; i++) { */
  /*   setDuty(i); */
  /*   __delay_ms(3); */
  /* } */
  /* for (unsigned int i = 1023; i >0; i--) { */
  /*   setDuty(i); */
  /*   __delay_ms(3); */
  /* } */

  while (1) {
    for (unsigned char *ptr = payload; ptr < &payload[5]; ptr++) {
      *ptr = 0;
    }
    payloadPtr = payload;
    SpiTransaction(6, true, read, payload);
    cmd = *payloadPtr;

    switch (cmd) {
    case 'p':
      len = *(++payloadPtr);
      if (!len || len > 4)
        SpiError();
      else
        SpiTransaction(len, false, ++payloadPtr, read);
      break;
    }
    /* SpiTransaction(6, false, payload, read); */
  }
    /*   /\* case 'r': *\/ */
    /*   /\*   *payloadPtr++ = (unsigned char) duty >> 8; *\/ */
    /*   /\*   *payloadPtr++ = (unsigned char) duty &0xff; *\/ */
    /*   /\*   /\\* payload[2] = (unsigned char) duty & 0xff; *\\/ *\/ */
    /*   /\*   /\\* payload[3] = (unsigned char) 0; *\\/ *\/ */
    /*   /\*   /\\* payload[2] = payloadPtr == &payload[2]; *\\/ *\/ */
    /*   /\*   SpiTransaction(2, &(payload[2]), read); *\/ */
    /*   /\*   break; *\/ */
    /*   /\* case 's': *\/ */
    /*   /\*   val = (unsigned int)  *payloadPtr++ << 8; *\/ */
    /*   /\*   val |= (unsigned int) *payloadPtr++; *\/ */
    /*   /\*   *payloadPtr++ = (unsigned char)duty >> 8; *\/ */
    /*   /\*   *payloadPtr++ = (unsigned char)duty & 0xff; *\/ */
    /*   /\*   setDuty(val); *\/ */
    /*   /\*   SpiTransaction(2, &(payload[2]), read); *\/ */
    /*   /\*   break; *\/ */
    /*   default: */
    /*     SpiError(); */
    /*   } */
    /* } */
    return;
}

void __interrupt() ISR() {
  if (PIR1bits.TMR1IF) {
    dimmerISR();
    PIR1bits.TMR1IF = 0; /* clear flag */
  }

  if (INTCONbits.GPIF) {
    unsigned char state = GPIO;
    unsigned char changed = last ^ state;
    if (changed & MAINS_mask)
      dimmerISR();
    last = state;
    INTCONbits.GPIF = 0; /* clear flag */
  }
}
