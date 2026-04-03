#ifndef LTC2494_H
#define LTC2494_H

#include <stdint.h>
#include <stdbool.h>

/*
 * Critical Engineering Notes
 * Latency:
 * Because this is a Delta-Sigma ADC, the data you read back is always the result
 * of the previous conversion. If you change the channel in the config struct,
 * the value returned in that same function call belongs to the old channel.
 *
 * CS Pin Polarity:
 * The LTC2494 uses nCS to initiate the conversion.
 * When nCS goes high, the conversion starts. When it goes low, the data is shifted out.
 *
 * Timing:
 * Ensure your SPI clock speed does not exceed the maximum frequency specified in the
 * datasheet (usually ~2MHz depending on main clock
 */


/** * @brief Gain selection for the internal PGA.
 */
typedef enum {
    LTC2494_GAIN_1   = 0x0,
    LTC2494_GAIN_8   = 0x1,
    LTC2494_GAIN_16  = 0x2,
    LTC2494_GAIN_32  = 0x3,
    LTC2494_GAIN_64  = 0x4,
    LTC2494_GAIN_128 = 0x5,
    LTC2494_GAIN_256 = 0x6
} ltc2494_gain_t;

/**
 * @brief Rejection mode for line frequency noise.
 */
typedef enum {
    LTC2494_REJECT_50_60HZ = 0x0,
    LTC2494_REJECT_50HZ    = 0x1,
    LTC2494_REJECT_60HZ    = 0x2
} ltc2494_rejection_t;

/**
 * @brief Configuration structure for a conversion cycle.
 */
typedef struct {
    uint8_t channel_pair;    // 0-7 for differential pairs
    bool polarity_swap;      // false: CH+ is even, true: CH+ is odd
    ltc2494_gain_t gain;
    ltc2494_rejection_t rejection;
} ltc2494_config_t;

/* Function pointer type for hardware abstraction (e.g., SPI_TransmitReceive) */
typedef bool (*ltc2494_spi_transfer_fn)(uint8_t *tx_data, uint8_t *rx_data, uint16_t len);

/**
 * @brief Reads the previous conversion result and triggers a new one.
 * @param transfer Function pointer to your SPI HAL.
 * @param config Pointer to the configuration for the NEXT conversion.
 * @param raw_result Output pointer for the 16-bit ADC result.
 * @return true if successful and data is valid (no overflow/underflow).
 */
bool ltc2494_transceive(ltc2494_spi_transfer_fn transfer,
                        const ltc2494_config_t *config,
                        int32_t *raw_result);

#endif // LTC2494_H
