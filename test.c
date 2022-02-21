#include <xc.h>

#pragma config FOSC = INTOSCIO  //oscillator selection; use internal oscillator
#pragma config WDTE = OFF       //disable watchdog Timer
#pragma config PWRTE = OFF      //power-up timer disabled
#pragma config MCLRE = OFF      //use MCLR pin (internally tied to Vdd) as a digital input
#pragma config BOREN = OFF      //disable Brown-Out Reset
#pragma config CPD = OFF        //data code protection disabled
#pragma config CP = OFF         //code protection disabled

__bit LEDstatus;

#define MISO GPIObits.GP0
#define OUTPUT 0
#define _XTAL_FREQ 8000000

/* hal */
#define MISO GPIObits.GP0
#define MOSI GPIObits.GP1
#define CLK GPIObits.GP2
#define MAINS GPIObits.GP3
#define CS GPIObits.GP4
#define LAMP GPIObits.GP5
#define _XTAL_FREQ 8000000

#define OUTPUT 0
#define INPUT 1
#define INTERNAL 0

unsigned int period = 0;

unsigned int calcPeriod(unsigned int period) {
  return 0xffff - ((_XTAL_FREQ / 4000000) * period);
}

void setDuty(unsigned int duty) {
  period = calcPeriod((unsigned long)10000 * duty / 0x3ff);
}

void init(void) {
  OSCCONbits.IRCF = 0b111;
  OSCCONbits.SCS = 1;
  LEDstatus = 1; // initialize LED off (active-low)

  /* hal */
  TRISIObits.TRISIO0 = OUTPUT;
  TRISIObits.TRISIO1 = INPUT;
  TRISIObits.TRISIO2 = INPUT;
  TRISIObits.TRISIO3 = INPUT;
  TRISIObits.TRISIO4 = INPUT;
  TRISIObits.TRISIO5 = OUTPUT;

  ANSEL = 0;

  /* pullups */
  OPTION_REGbits.nGPPU = 1;
  WPUbits.WPU1 = 1;
  WPUbits.WPU2 = 1;
  WPUbits.WPU4 = 1;

  INTCONbits.GPIE = 1; /* interrupt on change on input pins */
  IOCbits.IOC3 = 1;    /* interrupt on change on mains pin */
  /* INTCONbits.INTE = 1; /\* interrupt on clock pin *\/ */

  T1CON = 0b00110000;    // Timer 1 off; 1:8 prescaler; use internal clock
  T1CONbits.T1CKPS1 = 0; // bits 5-4  Prescaler Rate Select bits
  T1CONbits.T1CKPS0 = 0; // bit 4

  TMR1 = period;

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
    __delay_ms(100);
    i %= 0x3ff;
  }

  return;
}

void __interrupt() ISR() {
  if (PIR1bits.TMR1IF==1) {
    LAMP = 0;
    PIR1bits.TMR1IF=0;          //clear interrupt flag
    T1CONbits.TMR1ON = 0; // turn on Timer 1
  }
  if (INTCONbits.GPIE && INTCONbits.GPIF) {
    TMR1 = period;
    T1CONbits.TMR1ON = 1; // turn on Timer 1
    LAMP = 1;
    INTCONbits.GPIF = 0;
  }
}
