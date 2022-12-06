#include "PS2_pio_com.h"
#include "atkPico.h"

PS2_Device_PIO ps2KDev;

unsigned int resetHappend = 0;
unsigned int ackBack = 0;
unsigned short sentCommand = 0;



void PS2_device_send(unsigned char message){
    pio_sm_put_blocking(ps2KDev.pioBlock,ps2KDev.sm ,ps2_add_parity(message));
}


void ps2_tx_blocking(unsigned char message, unsigned char extension, unsigned char release)
{
    while(sentCommand !=0){
        sleep_ms(1);
    }
    if(extension){
        PS2_device_send(PS2_EXTENSION_FLAG);
    }
    if(release){
        PS2_device_send(PS2_RELASE_FLAG);
    }
    PS2_device_send(message);
    
}

void ps2_rx_keyboard_event(void){
    static unsigned char sentDataPos = 0;

    pio_interrupt_clear(ps2KDev.pioBlock, 0);
    printf("----got here\n");
    uint16_t code = *((io_rw_16*)&ps2KDev.pioBlock->rxf[ps2KDev.sm] + 1);
    code = code >> 6; //remove start bit and no data
    code &= 0xff; //then strip the stop and parity 

    if(sentDataPos == 0)
    {
        if(code == PS2_COMMANDS_SET_LEDS){
            printf("got here\n");
            PS2_device_send(PS2_ACK_RESPONSE);
            sentDataPos = 1;
        }
        else if(code == 0xFE){
            printf("repeat\n");
            PS2_device_send(0x0);
        }
        else {
            printf("don't know %d\n",code);
            PS2_device_send(PS2_ACK_RESPONSE);
        }
    }
    else if(sentDataPos == 1)
    {
        if(PS2_COMMANDS_SET_LEDS == sentCommand){
            //this will be your LED your using
            PS2_device_send(PS2_ACK_RESPONSE);
        }
        else{
            printf("don't know what happened send 2 - %d\n",code);
            PS2_device_send(PS2_ACK_RESPONSE);
        }
    }
    else
    {
        printf("don't know what happened??? %d\n", code);
        PS2_device_send(PS2_ACK_RESPONSE);
    }
}


void ps2_keyboard_device_init(unsigned char pio, unsigned char gpio)
{
    ps2KDev.pioNum = pio;
    ps2KDev.gpioStart = gpio;
    ps2_device_init(&ps2KDev, &ps2_rx_keyboard_event);

    sleep_ms(200); //wait for the proper time to send the bat message
    PS2_device_send(0xAA);
}