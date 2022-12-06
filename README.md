# README
This is a PIO lib to add support PS2 device support.

## Supported PS2 devices
* keyboard
* mouse - (currently just basic, working on extended set)

## Notes
Currently i use defines from another [project that involves PS2](https://github.com/brendena/atkPico) 

## PIO notes
* output takes in 9 bytes - 8 data , 1 parity
* input takes in 10 bytes - 8 data, 1 parity, 1 stop byte.




## good example project
https://github.com/techpaul/PS2KeyAdvanced