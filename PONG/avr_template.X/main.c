/*
 * File:   main.c
 * Author: loady
 *
 * Created on March 18, 2024, 2:43 PM
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/builtins.h>
#include <avr/builtins.h>
#include <avr/wdt.h>
#include "max7219.h"
#include "pong.h"



int main(void) {            
    setup();
    play();
}
