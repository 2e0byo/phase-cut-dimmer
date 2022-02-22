#include "dimmer.h"
#include "hal.h"
#include "spi.h"
#include <xc.h>

#pragma config FOSC = INTOSCIO // oscillator selection; use internal oscillator
#pragma config WDTE = OFF      // disable watchdog Timer
#pragma config PWRTE = OFF     // power-up timer disabled
#pragma config MCLRE =                                                         \
    OFF // use MCLR pin (internally tied to Vdd) as a digital input
#pragma config BOREN = OFF // disable Brown-Out Reset
#pragma config CPD = OFF   // data code protection disabled
#pragma config CP = OFF    // code protection disabled

volatile unsigned char last;

void init(void) {
  OSCCONbits.IRCF = 0b111;
  OSCCONbits.SCS = 1;

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

  INTCONbits.GPIE = 1; /* interrupt on change on input pins */
  /* MAINSIOC = 1; */
  /* TODO MAINSIOC disabled */

  CSIOC = 1; /* interrupt on change on CS. */

  T1CON = 0b00110000;    // Timer 1 off; 1:8 prescaler; use internal clock
  T1CONbits.T1CKPS1 = 0; // bits 5-4  Prescaler Rate Select bits
  T1CONbits.T1CKPS0 = 0; // bit 4

  INTCONbits.PEIE = 1; // enable peripheral interrupts (i.e., Timer 1)
  PIR1bits.TMR1IF = 0; // clear Timer 1 interrupt flag
  PIE1bits.TMR1IE = 1; // enable Timer 1 interrupts

  T1CONbits.TMR1ON = 0; // turn off Timer 1

  last = GPIO;
  INTCONbits.GIE = 1; // enable all interrupts
}

/* void main(void) { */
/*   init(); */
/*   unsigned char cmd; */
/*   unsigned char len; */
/*   unsigned char payload[6]; */
/*   unsigned char read[6]; */
/*   unsigned int val; */
/*   unsigned char *payloadPtr = payload; */

/*   while (1) { */
/*     payloadPtr = payload; */
/*     SpiTransaction(6, read, payload); */
/*     /\* cmd = *(++payloadPtr); *\/ */
/*     /\* len = *(++payloadPtr); *\/ */
/*     cmd = payload[0]; */
/*     len = payload[1]; */
/*     switch (cmd) { */
/*     case 'p':                   /\* parrot: for testing. *\/ */
/*       /\* SpiTransaction(len, payloadPtr, read); *\/ */
/*       LAMP = 1; */
/*       SpiTransaction(len, &payload[2], read); */
/*       SpiTransaction(3, read, payload); */
/*       LAMP = 0; */
/*     /\* case 'r': *\/ */
/*     /\*   *(++payloadPtr) = (unsigned char) duty &0xff; *\/ */
/*     /\*   *(--payloadPtr) = (unsigned char) duty >> 8; *\/ */
/*     /\*   SpiTransaction(2, payloadPtr, read); *\/ */
/*     /\* case 's': *\/ */
/*     /\*   val = *(++payloadPtr); *\/ */
/*     /\*   val |= (unsigned int) (--payloadPtr) << 8; *\/ */
/*     /\*   setDuty(val); *\/ */
/*     /\*   SpiTransaction(2, payloadPtr, read); *\/ */
/*     } */
/*   } */
/*   return; */
/* } */

void blip(unsigned char x) {
  static __bit start;
  start = LAMP;
  for (unsigned char i = 0; i < x; i++) {
    LAMP = !start;
    LAMP = start;
  }
}

void main(void) {
  init();
  unsigned char cmd;
  unsigned char len;
  volatile unsigned char payload[6];
  volatile unsigned char write[6];
  volatile unsigned char read[6];
  volatile unsigned char blank[6];
  for (unsigned char i = 0; i < 6; i++)
    blank[i] = 0;

  LAMP = 0;
  for (unsigned char i = 0; i < 6; i++)
    payload[i] = 0;
  while (1) {
    /* LAMP = 1; */
    SpiTransaction(6, blank, payload);
    /* LAMP = 0; */
    cmd = payload[0];
    len = payload[1];

    switch (cmd) {
    case 'p':
      /* blip(len == 2); */
      SpiTransaction(len, &payload[2], read);
    }

    /* blank[0] = cmd; */
    /* SpiTransaction(3, blank, read); */
    /* switch (cmd) { */
    /* case 'p': */
    /*   LAMP = 1; */
    /*   SpiTransaction(len, &payload[2], read); */
    /*   LAMP = 0; */
    /*   break; */
    /* default: */
    /*   SpiTransaction(2,payload, read); */

    /* } */
  }
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
    if (changed & CS_mask)
      SpiCS(state);
    if (changed & SCK_mask)
      SpiSCK(state);
    last = state;
    INTCONbits.GPIF = 0; /* clear flag */
  }
}
