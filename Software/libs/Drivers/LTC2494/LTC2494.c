#include "LTC2494.h"

// Internal Command Constants
#define LTC2494_CMD_SELECT     0xA0 // 101: Enable, 0: Single-ended/Diff, 0: Sign
#define LTC2494_CMD_CONFIG     0x80 // 1: Config select, 00: Speed/External clock

bool ltc2494_transceive(ltc2494_spi_transfer_fn transfer,
                        const ltc2494_config_t *config,
                        int32_t *raw_result) {

    uint8_t tx[3] = {0};
    uint8_t rx[3] = {0};

    /* --- Prepare Input Word 1 (Channel Selection) --- */
    // Bit 7-5: 101 (Enable/Channel select)
    // Bit 4: 0 (Differential mode)
    // Bit 3: Polarity (Even/Odd)
    // Bit 2-0: Channel address
    tx[0] = LTC2494_CMD_SELECT | (config->polarity_swap << 3) | (config->channel_pair & 0x07);

    /* --- Prepare Input Word 2 (Config/Gain/Rejection) --- */
    // Bit 7: 1 (Select Configuration)
    // Bit 6-5: 00 (Internal Clock, 1x Speed)
    // Bit 4-2: Gain
    // Bit 1-0: Rejection mode
    tx[1] = LTC2494_CMD_CONFIG | (config->gain << 2) | (config->rejection);

    tx[2] = 0x00; // Third byte is don't care for input, used for reading output

    // Perform full-duplex SPI transfer
    if (!transfer(tx, rx, 3)) {
        return false;
    }

    /* --- Process Output Data --- */
    // The LTC2494 returns 16 bits of data plus status bits.
    // [SIG][MSB][B14]...[LSB][Sub-LSBs]

    // Check for Overflow (SIG and MSB high) or Underflow (SIG and MSB low)
    bool sig = (rx[0] & 0x80) != 0;
    bool msb = (rx[0] & 0x40) != 0;

    if (sig == msb) {
        // Result is out of range or EOC (End of Conversion) not yet reached
        return false;
    }

    // Extract 16-bit result (Shift to align the 16 bits starting from MSB)
    // The result starts at Bit 6 of rx[0] down to Bit 7 of rx[2]
    uint32_t assembled = ((uint32_t)(rx[0] & 0x3F) << 10) |
                         ((uint32_t)rx[1] << 2) |
                         ((uint32_t)rx[2] >> 6);

    *raw_result = (int32_t)assembled;

    return true;
}
