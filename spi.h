#include <stdbool.h>
#ifndef SPI
#define SPI
/**
 * @brief Function called on Chip Select toggle.
 * @details Enables if Chip Select goes low, else disables.
 * @param[in] state the current port state.
 */
void SpiCS(unsigned char state);

/**
 * @brief Function called on Clock toggle.
 * @details Shifts out on rising edges, samples on falling.
 * @param[in] state the current port state.
 */
void SpiSCK(unsigned char state);

/**
 * @brief SPI Transaction.
 * @details Blocking send and receive.
 * @param[in] bytes Expected length (master may not honour).
 * @param[in] blank Write nothing (write ptr is ignored).
 * @param[out] write Buffer to write into.
 * @param[in] read Buffer to read from.
 */

bool SpiTransaction(unsigned char bytes, unsigned char blank, unsigned char *write,
                    unsigned char *read);

#endif
