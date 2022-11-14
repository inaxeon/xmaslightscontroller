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
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "iopins.h"
#include "config.h"
#include "usart.h"
#include "util.h"
#include "pwm.h"

typedef struct
{
    sys_config_t *config;
    bool running;
} sys_runstate_t;

sys_config_t _g_cfg;
sys_runstate_t _g_rs;

FILE uart_str = FDEV_SETUP_STREAM(print_char, NULL, _FDEV_SETUP_RW);

static void io_init(void);
int random_number(const int min, const int max);
void run_sequence(sys_runstate_t *rs, uint8_t seq);
void check_ctrld(void);

void sequence_00(sys_runstate_t *rs);
void sequence_01(sys_runstate_t *rs);

int main(void)
{
    sys_runstate_t *rs = &_g_rs;
    sys_config_t *config = &_g_cfg;
    rs->config = config;

    wdt_enable(WDTO_2S);
    io_init();
    g_irq_enable();

    usart1_open(USART_CONT_RX, (((F_CPU / UART1_BAUD) / 16) - 1)); // Console

    stdout = &uart_str;

    printf("\r\n");

    load_configuration(config);
    configuration_bootprompt(config);

    printf("Press Ctrl+D at any time to reset\r\n");

    CLRWDT();

    pwm_init();
    pwm_enable();

    // Idle loop
    for (;;)
    {
        rs->running = true;
        run_sequence(rs, config->start_seq);
    }
}

void run_sequence(sys_runstate_t *rs, uint8_t seq)
{
    switch (seq)
    {
        case 0:
            sequence_00(rs);
            break;
        case 1:
            sequence_01(rs);
            break;
        default:
            printf("\r\nInvalid sequence %d. Resetting...\r\n", rs->config->start_seq);
            reset();
    }
}

// "On" at 1/4 brightness
void sequence_00(sys_runstate_t *rs)
{
    uint8_t full_brightness = 0x40;

    for (uint8_t i = 0; i < 4; i++)
        pwm_set_duty(i, full_brightness);

    while (rs->running)
    {
        CLRWDT();
        check_ctrld();
    }
}


// "Sparkle" between 40% and 100% brightness
void sequence_01(sys_runstate_t *rs)
{
    while (rs->running)
    {
        uint8_t channel = (uint8_t)random_number(0, 4);
        uint8_t full_brightness = 0xFF;
        uint8_t low_brightness = 0x40;
        uint8_t step_size = (full_brightness - low_brightness) / 5;
        uint16_t delay = (uint16_t)random_number(50, 400);

        for (uint8_t i = 1; i < 5; i++)
        {
            pwm_set_duty(channel, low_brightness + (i * step_size));
            _delay_ms(10);
        }

        pwm_set_duty(channel, full_brightness);
        _delay_ms(50);

        for (uint8_t i = 4; i > 0; i--)
        {
            pwm_set_duty(channel, low_brightness + (i * step_size));
            _delay_ms(10);
        }

        pwm_set_duty(channel, low_brightness);

        for (int i = 0; i < delay; i++)
            _delay_ms(1);

        CLRWDT();
        check_ctrld();
    }
}

void check_ctrld(void)
{
    if (console_data_ready())
    {
        char c = console_get();
        if (c == 4)
        {
            printf("\r\nCtrl+D received. Resetting...\r\n");
            while (console_busy());
            reset();
        }
    }
}

int random_number(const int min, const int max)
{
    return rand() % (max - min) + min;
}

static void io_init(void)
{
    //PCICR |= _BV(PCIE1);

    IO_LOW(PWM0A);
    IO_LOW(PWM0B);
    IO_LOW(PWM1A);
    IO_LOW(PWM1B);
    IO_LOW(DIR0);
    IO_LOW(DIR1);

    IO_OUTPUT(PWM0A);
    IO_OUTPUT(PWM0B);
    IO_OUTPUT(PWM1A);
    IO_OUTPUT(PWM1B);
    IO_OUTPUT(DIR0);
    IO_OUTPUT(DIR1);
}
