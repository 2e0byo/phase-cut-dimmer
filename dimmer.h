/**
 * @brief Set duty.
 * @details Use this fn to set duty: it calculates things.
 * @param[in] duty Duty to set (max 1023).
 */
void setDuty(unsigned int duty);

/**
 * @var duty
 * @brief The current duty (max 1023).
 */
unsigned int duty;

/**
 * @brief ISR for the dimmer.
 * @details Call this fn from your global ISR.
 */
void dimmerISR(void);
