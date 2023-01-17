#include <stdbool.h>
#ifndef SPI
#define SPI
/**
 * @brief SPI Transaction.
 * @details Blocking send and receive.
 * @param[out] write Buffer to write into.
 * @param[in] read Buffer to read from.
 * @return true if anything sent else false.
 */
bool SpiTransaction(char *mosi, const char *miso, unsigned char len);
#endif
