#include "hal.h"
#include <xc.h>

static unsigned char *inBuffer;
static unsigned char *outBuffer;
static unsigned char maxBytes = 0;
static unsigned char count = 0;
static unsigned char scratch;
static unsigned char bit = 0x80;
static unsigned char transactionAvailable = 0;

static void enable(void) {
  SCKIOC = 1; /* enable interrupt on clock change */
  MISOTRIS = OUTPUT;
}

static void disable(void) {
  SCKIOC = 0;
  MISOTRIS = INPUT;
  ++transactionAvailable;
}

void SpiCS(unsigned char state) {
  if (state & CS_mask)
    disable();
  else
    enable();
}

void SpiSCK(unsigned char state) {
  if (state & SCK_mask) { /* low to high transition */
    MISO = *outBuffer & bit ? 1 : 0;
  } else { /* high to low transition */
    if (MOSI)
      *inBuffer |= bit;

    bit >>= 1;

    if (!bit) { /* word transition. */
      bit = 0x80;

      if (count < maxBytes) {
        ++inBuffer;
        ++outBuffer;
        ++count;
      } else {
        inBuffer = &scratch;
      }
    }
  }
}

void SpiTransaction(unsigned char bytes, unsigned char *write,
                    unsigned char *read) {
  count = 0;
  maxBytes = bytes - 1;
  inBuffer = read;
  outBuffer = write;
  while (!transactionAvailable)
    ;
  transactionAvailable = 0;
}
