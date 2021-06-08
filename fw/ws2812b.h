#pragma once

#include <stdbool.h>
#include <stdint.h>

// Must wait at least 50us between each call.
// Returns false if interrupted.
bool ws2812b_write(const uint8_t *begin, const uint8_t *end);
