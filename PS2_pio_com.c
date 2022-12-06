#include "PS2_com.pio.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "atkPico.h"
#include <stdio.h>
#include "PS2_pio_com.h"


void ps2_device_init(PS2_Device_PIO * device, void  (*callBack)(void)) {
    device->pioBlock = device->pioNum ? pio1 : pio0;
    device->irq = device->pioNum ? PIO1_IRQ_0 : PIO0_IRQ_0;
    
    //short hands for things in *device
    unsigned char gpio    =  device->gpioStart;
    unsigned char dataPin = device->gpioStart;
    unsigned char clkPin  = device->gpioStart + 1;
    PIO  pio = device->pioBlock;
    uint irq = device->irq;
    uint sm;

    

// add internal pull up pin
    gpio_pull_up(dataPin);
    gpio_pull_up(clkPin);
    //setup pio
    device->sm = sm = pio_claim_unused_sm(pio, true);
    uint offset = pio_add_program(pio, &ps2_com_program);
    pio_sm_config c = ps2_com_program_get_default_config(offset);

    sm_config_set_set_pins(&c, gpio,2);
    sm_config_set_jmp_pin(&c, clkPin); // set the EXECCTRL_JMP_PIN

//configure out section PS2
    // Set the out pins
    sm_config_set_out_pins(&c, gpio, 2);
// Shift 9 bits to the right, autopush enabled
    sm_config_set_out_shift(&c, true, true, 9); //eight data bytes plus parity

    sm_config_set_sideset_pins(&c, clkPin);

    //pio_sm_set_pins(atk_pio,atk_sm,0xff); //set pins to high

//configure in section PS2
    // Set the base input pin. pin index 0 is DAT, index 1 is CLK
    sm_config_set_in_pins(&c, gpio);

//start the pins as input at start
    pio_sm_set_consecutive_pindirs(pio, sm, gpio, 2, false);
    // Shift 8 bits to the right, autopush enabled
    sm_config_set_in_shift(&c, true, true, 10);


//init the gpio
    pio_gpio_init(pio, dataPin);
    pio_gpio_init(pio, clkPin);

    // We don't expect clock faster than 16.7KHz and want no less
    // than 8 SM cycles per keyboard clock.
    float div = (float)clock_get_hz(clk_sys) / (8 * 16700);
    sm_config_set_clkdiv(&c, div);


    pio_set_irq0_source_enabled(pio, pis_interrupt0, true);
    irq_set_exclusive_handler(irq, callBack);
    irq_set_enabled(irq, true);

    sleep_ms(100);//some devices have a little jitter at start
    // Ready to go
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);



}
