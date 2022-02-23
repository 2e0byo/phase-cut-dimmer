#include "hal.h"
#include <xc.h>
#include  "spi.h"

static unsigned char *inBuffer;
static const unsigned char *outBuffer;
static unsigned char count = 0;
static unsigned char scratch;
static unsigned char mask = 0x80;
static unsigned char blankBfr = 0;
bool writeBlank;

static void enable(void) {
  MISO = 0;
  MISOTRIS = OUTPUT;
}

static void disable(void) {
  MISO = 0;
  MISOTRIS = INPUT;
}

void clockinout(void) {
  static __bit last;

  while (!CS) {
    while (!CS && SCK == last)
      ;
    if (CS)
      return;

    last ^= 1;
    if (last) { /* low to high transition */

      MISO = *outBuffer & mask ? 1 : 0;
    } else { /* high to low transition */

      if (MOSI) {
        *inBuffer |= mask;
      } else {
      }

      mask >>= 1;

      if (!mask) { /* word transition. */
        mask = 0x80;

        if (count) {
          ++inBuffer;
          --count;
          if (writeBlank == false)
            ++outBuffer;
        } else {
          inBuffer = &scratch;
        }
      }
    }
  }
}

static const unsigned char tmpBuffer[] = {0,1,2,3,4,5};

void SpiTransaction(unsigned char bytes,
                    unsigned char blank,
                    unsigned char *write,
                    unsigned char *read) {
  count = bytes - 1;
  inBuffer = read;
  outBuffer = blank? &blankBfr: write;
  writeBlank = blank;

  while (CS)
    ;
  enable();
  clockinout();
  disable();

}
