#include "spi.h"
#include "hal.h"
#include <xc.h>

static const char devNull = '*';

static void enable(void) {
  MISO = 0;
  MISOTRIS = OUTPUT;
}

static void disable(void) {
  MISOTRIS = INPUT;
  MISO = 0;
}

bool SpiTransaction(char *mosiPtr, const char *misoPtr, unsigned char len) {
  /* CS is active low */
  if (CS)
    disable();
  while (CS)
    CLRWDT();
  enable();

  static __bit last;
  last = 0;
  unsigned char mask = 0x80;
  while (!CS) {
    while (!CS && SCK == last)
      ;
    if (CS)
      return false;

    last ^= 1;
    if (last) { /* low to high transition */
      MISO = *misoPtr & mask ? 1 : 0;
    } else { /* high to low transition */
      /* must be an edge, as we waited at the beginning */

      if (MOSI)
        *mosiPtr |= mask;

      mask >>= 1;

      if (!mask) { /* byte */
        if (*mosiPtr == '#')
          return true;
        mask = 0x80;

        if (len) {
          ++mosiPtr;
          ++misoPtr;
          --len;
        } else {
          misoPtr = &devNull;
        }
      }
    }
  }
  return false;
}
