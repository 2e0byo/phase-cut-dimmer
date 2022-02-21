#include <xc.h>
#include "dimmer.h"
#include "hal.h"

#pragma config FOSC = INTOSCIO  //oscillator selection; use internal oscillator
#pragma config WDTE = OFF       //disable watchdog Timer
#pragma config PWRTE = OFF      //power-up timer disabled
#pragma config MCLRE = OFF      //use MCLR pin (internally tied to Vdd) as a digital input
#pragma config BOREN = OFF      //disable Brown-Out Reset
#pragma config CPD = OFF        //data code protection disabled
#pragma config CP = OFF         //code protection disabled


void init(void) {
  OSCCONbits.IRCF = 0b111;
  OSCCONbits.SCS = 1;

  /* hal */
  MISOTRIS = OUTPUT;
  MOSITRIS = INPUT;
  CLKTRIS = INPUT;
  CSTRIS = INPUT;
  LAMPTRIS = OUTPUT;
  MAINSTRIS = INPUT;

  ANSEL = 0;

  /* pullups */
  OPTION_REGbits.nGPPU = 1;
  WPU = 0xff;

  INTCONbits.GPIE = 1; /* interrupt on change on input pins */
  MAINSIOC = 1;
  /* INTCONbits.INTE = 1; /\* interrupt on clock pin *\/ */

  T1CON = 0b00110000;    // Timer 1 off; 1:8 prescaler; use internal clock
  T1CONbits.T1CKPS1 = 0; // bits 5-4  Prescaler Rate Select bits
  T1CONbits.T1CKPS0 = 0; // bit 4

  INTCONbits.PEIE = 1; // enable peripheral interrupts (i.e., Timer 1)
  PIR1bits.TMR1IF = 0; // clear Timer 1 interrupt flag
  PIE1bits.TMR1IE = 1; // enable Timer 1 interrupts

  T1CONbits.TMR1ON = 0; // turn off Timer 1

  INTCONbits.GIE = 1; // enable all interrupts
}

void main(void)
{
  init();

  unsigned int i = 0;
  while (1) {
    setDuty(i++);
    __delay_ms(10);
    i %= 0x3ff;
  }

  return;
}

void __interrupt() ISR() {
  if (PIR1bits.TMR1IF || INTCONbits.GPIF) dimmerISR();
}
