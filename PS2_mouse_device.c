#include "pico/stdlib.h"
#include "PS2_pio_com.h"
#include "PS2_mouse_device.h"
#include "atkPico.h"

const PS2_mouse_data blankMouseData = {0};

PS2_Device_PIO ps2MDev;
PS2_mouse_data mouseDataOverflow;
unsigned char mouseMode = ID_STANDARD_PS2_MOUSE; 
unsigned char sendingDataOut = 0;
unsigned char enabled = 0;

unsigned char prevSampleRates[3];

int64_t delay =  (1000 * 1000) / 80;

void ps2_mouse_tx_single(unsigned char send){
    pio_sm_put_blocking(ps2MDev.pioBlock,ps2MDev.sm ,ps2_add_parity(send));
}

void ps2_mouse_ack()
{
    ps2_mouse_tx_single(PS2_ACK_RESPONSE);
}

void ps2_mouse_reset_response()
{
    ps2_mouse_tx_single(PS2_BAT_RESPONSE);
    ps2_mouse_tx_single(mouseMode); 
}

void ps2_tx_mouse_block(PS2_mouse_data * mouseData)
{
    mouseData->btnAndSigns.keys.always1 = 1;
    ps2_mouse_tx_single(mouseData->btnAndSigns.value);
    ps2_mouse_tx_single(mouseData->x);
    ps2_mouse_tx_single(mouseData->y);

    if(mouseMode == ID_INTELLIMOUSE_W_SCROLL || 
       mouseMode == ID_INTELLIMOUSE_W_SCROLL_BUTTONS)
    {
        ps2_mouse_tx_single(mouseData->zAndBtn.value);
    }
}



int64_t ps2_mouse_tx_buffered_send_callback(alarm_id_t id, void *user_data)
{
    
    if(memcmp(&mouseDataOverflow,&blankMouseData) == 0)
    {
        sendingDataOut = 0;
        return 0;
    }
    printf("sending data\n");
    ps2_tx_mouse_block(&mouseDataOverflow);
    return delay;
}

int64_t ps2_delayed_reset_response(alarm_id_t id, void *user_data)
{
    ps2_mouse_reset_response();
    return 0;
}

void ps2_rx_mouse_event(void)
{
    static unsigned char sentDataPos = 0;
    static unsigned short sentCommand = 0;

    //printf("MOUSE [irq] %d\n",ps2MDev.pioBlock->irq &0b1111);


    pio_interrupt_clear(ps2MDev.pioBlock, ps2MDev.sm);

    if(pio_sm_is_rx_fifo_empty(ps2MDev.pioBlock, ps2MDev.sm)){
        return;
    }

    uint16_t code = *((io_rw_16*)&ps2MDev.pioBlock->rxf[ps2MDev.sm] + 1);
    code = code >> 6; //remove start bit and no data
    code &= 0xff; //then strip the stop and parity 
    
    printf("----codes %x, %d\n",code,sentDataPos);
    if(sentDataPos == 0)
    {
        sentCommand = code;
        if(code == PS2_COMMANDS_RESET){
            ps2_mouse_ack(); 
            if(add_alarm_in_us(1000 * 500,ps2_delayed_reset_response,NULL,1) < 0){
                printf("Failed to add timer\n");
            }
        }
        else if(code == PS2_M_COMMAND_SET_SCALLING_1_to_1)
        {
            ps2_mouse_ack();
        }
        else if(code == PS2_M_COMMAND_SET_SCALLING_2_to_1)
        {
            ps2_mouse_ack();
        }
        else if(code == PS2_M_COMMAND_SET_RESOLUTION)
        {
            sentDataPos++;
            ps2_mouse_ack();    
        }
        else if(code == PS2_M_COMMAND_DEVICE_TYPE)
        {
            ps2_mouse_ack(); 
            ps2_mouse_tx_single(mouseMode);
        }
        else if(code == PS2_M_COMMAND_SAMPLE_RATE){
            printf("sample rate changed\n");
            sentDataPos = 1;
            ps2_mouse_ack();
        }
        else if(code == PS2_M_COMMAND_ENABLE_DEVICE)
        {
            enabled = 1;
            ps2_mouse_ack();
        }
        else{
            printf("unkown command %x\n",code);
        }
    }
    else if(sentDataPos == 1)
    {
        sentDataPos = 0;
        if(sentCommand == PS2_M_COMMAND_SAMPLE_RATE)
        {
            ps2_mouse_ack();
        }
        else if(sentCommand == PS2_M_COMMAND_SET_RESOLUTION)
        {
            ps2_mouse_ack();    
        }
        else{
            printf("got somewhere i don't know \n");
        }
    }

}

void ps2_tx_mouse(PS2_mouse_data * mouseData)
{
    if(enabled)
    {
        if(sendingDataOut == 0)
        {
            ps2_tx_mouse_block(mouseData);
            if(add_alarm_in_us(delay,ps2_mouse_tx_buffered_send_callback,NULL,1) < 0){
                printf("Failed to add timer\n");
            }
            else{
                sendingDataOut = 1;
            }
        }
        else{
            //should merge this 
            memcmp(&mouseDataOverflow, mouseData, sizeof(PS2_mouse_data));
        }
    }
}

void ps2_mouse_device_init(unsigned char pio, unsigned char gpio)
{

    ps2MDev.pioNum = pio;
    ps2MDev.gpioStart = gpio;
    sleep_ms(100);
    ps2_device_init(&ps2MDev,&ps2_rx_mouse_event);

    sleep_ms(400); //wait for the proper time to send the bat message
    ps2_mouse_reset_response();
    
}