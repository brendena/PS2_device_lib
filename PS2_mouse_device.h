#pragma once

/*
good resources on keyboards
http://www.computer-engineering.org/ps2protocol/
*/
/*
bunch of the commands
https://wiki.osdev.org/PS/2_Mouse
*/

/*
If you using a intelligent mouse.  There will be a few microsecond delays between sends
*/

#define ID_STANDARD_PS2_MOUSE 0x00
#define ID_INTELLIMOUSE_W_SCROLL 0x03
#define ID_INTELLIMOUSE_W_SCROLL_BUTTONS 0x04

enum PS2_MOUSE_COMMANDS
{
    //F0 might be get scan code???
    PS2_M_COMMAND_SET_SCALLING_1_to_1 = 0xE6,
    PS2_M_COMMAND_SET_SCALLING_2_to_1 = 0xE7,
    PS2_M_COMMAND_SET_RESOLUTION = 0xE8,
    PS2_M_COMMAND_DEVICE_TYPE = 0xF2,
    PS2_M_COMMAND_SAMPLE_RATE = 0xF3,
    PS2_M_COMMAND_ENABLE_DEVICE = 0xF4,
    
};



typedef struct PS2_MOUSE_BYTE_1
{
    unsigned char leftBtn : 1;
    unsigned char rightBtn : 1;
    unsigned char middleBtn : 1;
    unsigned char always1   : 1;
    unsigned char xSignBit : 1;
    unsigned char ySignBit : 1;
    unsigned char xOverflow : 1;
    unsigned char yOverflow : 1;
}PS2_MOUSE_BYTE_1;

typedef union PS2_MB_1
{
    PS2_MOUSE_BYTE_1 keys;
    unsigned char value;
} PS2_MB_1;


typedef struct PS2_MOUSE_BYTE_4
{
    unsigned char z : 4;
    unsigned char btn4 : 1;
    unsigned char btn5 : 1;
    unsigned char reserve   : 2;
}PS2_MOUSE_BYTE_4;

typedef union PS2_MB_4
{
    PS2_MOUSE_BYTE_4 keys;
    unsigned char value;
} PS2_MB_4;

typedef struct PS2_mouse_data {
    PS2_MB_1 btnAndSigns;
    unsigned char x;
    unsigned char y;
    PS2_MB_4 zAndBtn;
} PS2_mouse_data;

void ps2_mouse_device_init(unsigned char pio, unsigned char gpio);
void ps2_tx_mouse(PS2_mouse_data * mouseData);