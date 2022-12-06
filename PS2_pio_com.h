#pragma once
#include <stdio.h>
#include "hardware/pio.h"
#include "ps2Defines.h"

typedef struct PS2_Device_PIO_ {
    //external set
    unsigned char pioNum;
    unsigned char gpioStart; //start gpio pin
    void * params; 
    
    //set internal
    uint sm;
    uint irq;
    PIO pioBlock;
} PS2_Device_PIO;

void ps2_device_init(PS2_Device_PIO * device, void  (*callBack)(void));
