#include "hal.h"
#include <xc.h>
#include  "spi.h"

static unsigned char *inBuffer;
static const unsigned char *outBuffer;
static unsigned char count = 0;
static unsigned char devNull;
static unsigned char mask = 0x80;
static unsigned char blankBfr = '#';
bool writeBlank;

static void enable(void) {
  MISO = 0;
  MISOTRIS = OUTPUT;
}

static void disable(void) {
  MISO = 0;
  MISOTRIS = INPUT;
}

bool clockinout(void) {
  static __bit last;
  unsigned int clockCount = 0;
  last = 0;

  while (!CS) {
    /* Wait for change. */
    while (!CS && SCK == last)
      ;
    if (CS)
      return !clockCount;

    /* last ^= 1; */
    last = SCK;
    if (last) { /* low to high transition */
      MISO = *outBuffer & mask ? 1 : 0;
    } else { /* high to low transition */
      /* must be an edge, as we waited at the beginning */
      ++clockCount;

      if (MOSI) {
        *inBuffer |= mask;
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
          inBuffer = &devNull;
        }
      }
    }
  }
  return !clockCount;
}


bool SpiTransaction(unsigned char bytes,
                    unsigned char blank,
                    unsigned char *write,
                    unsigned char *read) {
  count = bytes - 1;
  inBuffer = read;
  outBuffer = blank? &blankBfr: write;
  writeBlank = blank;
  bool ret;

  while (CS)
    CLRWDT();
  enable();
  ret = clockinout();
  disable();
  return ret;

}
