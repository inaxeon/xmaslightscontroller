/*
 *   File:   main.c
 *   Author: Matt
 *
 *   Created on 11 May 2018, 12:13
 * 
 *   This is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 2 of the License, or
 *   (at your option) any later version.
 *   This software is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *   You should have received a copy of the GNU General Public License
 *   along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "project.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "iopins.h"
#include "config.h"
#include "cmd.h"
#include "usart.h"
#include "util.h"
#include "pwm.h"

typedef struct
{
    sys_config_t *config;
} sys_runstate_t;

sys_config_t _g_cfg;
sys_runstate_t _g_rs;

FILE uart_str = FDEV_SETUP_STREAM(print_char, NULL, _FDEV_SETUP_RW);

static void io_init(void);

int main(void)
{
    sys_runstate_t *rs = &_g_rs;
    sys_config_t *config = &_g_cfg;
    rs->config = config;

    io_init();
    g_irq_enable();

    usart1_open(USART_CONT_RX, (((F_CPU / UART1_BAUD) / 16) - 1)); // Console

    stdout = &uart_str;

    printf("\r\n");

    load_configuration(config);

    printf("Starting up...\r\n");

    CLRWDT();

    pwm_init();

    while(1)
    {
        _delay_ms(250);
        _delay_ms(250);
    pwm_set_duty(0, 0x00);
    pwm_set_duty(1, 0xFF);
    pwm_set_duty(2, 0x00);
    pwm_set_duty(3, 0xFF);
_delay_ms(250);
        _delay_ms(250);
    pwm_set_duty(0, 0xFF);
    pwm_set_duty(1, 0x00);
    pwm_set_duty(2, 0xFF);
    pwm_set_duty(3, 0x00);
    }

    // Idle loop
    for (;;)
    {
        //timeout_check();
        cmd_process(config);
        CLRWDT();
    }
}


static void io_init(void)
{
#ifdef _LEONARDO_
    // Disable USB, because the bootloader has probably left it on
    USBCON &= ~_BV(USBE);
#endif

    PCICR |= _BV(PCIE1);
}
