#ifndef HAL
#define HAL
#include <xc.h>

#define MISO GPIObits.GP0
#define OUTPUT 0
#define _XTAL_FREQ 8000000

/* hal */
#define MISO GPIObits.GP0
#define MISOTRIS TRISIObits.TRISIO0
#define MOSI GPIObits.GP1
#define MOSITRIS TRISIObits.TRISIO1
#define CLK GPIObits.GP2
#define CLKTRIS TRISIObits.TRISIO2
#define MAINS GPIObits.GP3
#define MAINSTRIS TRISIObits.TRISIO3
#define MAINSIOC IOCbits.IOC3
#define CS GPIObits.GP4
#define CSTRIS TRISIObits.TRISIO4
#define LAMP GPIObits.GP5
#define LAMPTRIS TRISIObits.TRISIO5
#define _XTAL_FREQ 8000000

/* helpers */
#define OUTPUT 0
#define INPUT 1
#define INTERNAL 0
#endif
