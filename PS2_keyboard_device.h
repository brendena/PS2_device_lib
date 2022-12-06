#pragma once

void ps2_keyboard_device_init(unsigned char pio, unsigned char gpio);
void ps2_tx_blocking(unsigned char message, unsigned char extension, unsigned char release);