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
#include <math.h>
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
#include "i2c_slave.h"

#define SEQUENCE_REG    0x00

typedef struct
{
    sys_config_t *config;
    uint8_t *i2c_buffer;
    uint8_t last_sequence;
    bool running;
} sys_runstate_t;

sys_config_t _g_cfg;
sys_runstate_t _g_rs;

FILE uart_str = FDEV_SETUP_STREAM(print_char, NULL, _FDEV_SETUP_RW);

static void io_init(void);
int random_number(const int min, const int max);
void run_sequence(sys_runstate_t *rs, uint8_t seq);
void check_break(sys_runstate_t *rs);

void sequence_00_09(sys_runstate_t *rs, uint8_t brightness);
void sequence_10(sys_runstate_t *rs);
void sequence_11(sys_runstate_t *rs);
void sequence_12_16(sys_runstate_t *rs, uint8_t delay);

int main(void)
{
    sys_runstate_t *rs = &_g_rs;
    sys_config_t *config = &_g_cfg;
    rs->config = config;
    rs->i2c_buffer = TWI_Get_Buffer();

    memset(rs->i2c_buffer, 0, TWI_BUFFER_SIZE);
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

    TWI_Slave_Initialise(0x23 << 1);
    TWI_Start_Transceiver();

    rs->i2c_buffer[SEQUENCE_REG] = config->start_seq;

    // Idle loop
    for (;;)
    {
        rs->running = true;
        rs->last_sequence = rs->i2c_buffer[SEQUENCE_REG];
        run_sequence(rs, rs->i2c_buffer[SEQUENCE_REG]);
    }
}

void run_sequence(sys_runstate_t *rs, uint8_t seq)
{
    switch (seq)
    {
        case 0:
            sequence_00_09(rs, 25);
            break;
        case 1:
            sequence_00_09(rs, 51);
            break;
        case 2:
            sequence_00_09(rs, 76);
            break;
        case 3:
            sequence_00_09(rs, 102);
            break;
        case 4:
            sequence_00_09(rs, 127);
            break;
        case 5:
            sequence_00_09(rs, 153);
            break;
        case 6:
            sequence_00_09(rs, 178);
            break;
        case 7:
            sequence_00_09(rs, 204);
            break;
        case 8:
            sequence_00_09(rs, 229);
            break;
        case 9:
            sequence_00_09(rs, 255);
            break;
        case 10:
            sequence_10(rs);
            break;
        case 11:
            sequence_11(rs);
            break;
        case 12:
            sequence_12_16(rs, 10);
            break;
        case 13:
            sequence_12_16(rs, 8);
            break;
        case 14:
            sequence_12_16(rs, 6);
            break;
        case 15:
            sequence_12_16(rs, 4);
            break;
        case 16:
            sequence_12_16(rs, 2);
            break;
        default:
            printf("\r\nInvalid sequence %d. Resetting...\r\n", rs->config->start_seq);
            reset();
    }
}

// "On" at variable brightness
void sequence_00_09(sys_runstate_t *rs, uint8_t brightness)
{
    for (uint8_t i = 0; i < 4; i++)
        pwm_set_duty(i, brightness);

    while (rs->running)
    {
        check_break(rs);
    }
}

void sequence_10(sys_runstate_t *rs)
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

        check_break(rs);
    }
}

void sequence_11(sys_runstate_t *rs)
{
    uint8_t low_brightness = 0;
    uint8_t full_brightness = 0x40;
    uint8_t step_size = (full_brightness - low_brightness) / 5;

    uint8_t channel1_current = 0;
    uint8_t channel2_current = 0;
    uint8_t channel3_current = 0;
    //uint8_t channel4_current = 0;

    uint8_t channel1_max = 12;
    uint8_t channel2_max = 11;
    uint8_t channel3_max = 10;
    //uint8_t channel4_max = 9;

    pwm_set_duty(3, 0xff);

    while (rs->running)
    {
        uint8_t channel_flag = 0x00;

        if (channel1_current++ >= channel1_max)
        {
            channel1_current = 0;
            channel_flag |= 0x01;
        }

        if (channel2_current++ >= channel2_max)
        {
            channel2_current = 0;
            channel_flag |= 0x02;
        }

        if (channel3_current++ >= channel3_max)
        {
            channel3_current = 0;
            channel_flag |= 0x04;
        }

        // if (channel4_current++ >= channel4_max)
        // {
        //     channel4_current = 0;
        //     channel_flag |= 0x08;
        // }


        for (uint8_t i = 1; i < 5; i++)
        {
            if (channel_flag & 0x01)
                pwm_set_duty(0, low_brightness + (i * step_size));
            if (channel_flag & 0x02)
                pwm_set_duty(1, low_brightness + (i * step_size));
            if (channel_flag & 0x04)
                pwm_set_duty(2, low_brightness + (i * step_size));
            // if (channel_flag & 0x08)
            //     pwm_set_duty(3, low_brightness + (i * step_size));

            _delay_ms(5);
        }

        if (channel_flag & 0x01)
            pwm_set_duty(0, full_brightness);
        if (channel_flag & 0x02)
            pwm_set_duty(1, full_brightness);
        if (channel_flag & 0x04)
            pwm_set_duty(2, full_brightness);
        // if (channel_flag & 0x08)
        //     pwm_set_duty(3, full_brightness);

        _delay_ms(25);

        for (uint8_t i = 4; i > 0; i--)
        {
            if (channel_flag & 0x01)
                pwm_set_duty(0, low_brightness + (i * step_size));
            if (channel_flag & 0x02)
                pwm_set_duty(1, low_brightness + (i * step_size));
            if (channel_flag & 0x04)
                pwm_set_duty(2, low_brightness + (i * step_size));
            // if (channel_flag & 0x08)
            //     pwm_set_duty(3, low_brightness + (i * step_size));

            _delay_ms(5);
        }

        pwm_set_duty(0, low_brightness);
        pwm_set_duty(1, low_brightness);
        pwm_set_duty(2, low_brightness);
        // pwm_set_duty(3, low_brightness);

        check_break(rs);
    }
}

void sequence_12_16(sys_runstate_t *rs, uint8_t delay)
{
    uint16_t num_samples = 1000;
    float x_step = 1.0f / num_samples;

    uint8_t channel1 = 0;
    uint8_t channel2 = 0;
    uint8_t channel3 = 0;
    uint8_t channel4 = 0;

    while(rs->running)
    {
        for (uint16_t i = 0; i < num_samples; i++)
        {
            float sine = (float)sin(i * x_step * 2 * M_PI);

            if (sine >= 0)
                channel1 = (uint8_t)((float)255 * sine);
            if (sine <= 0)
                channel2 = (uint8_t)((float)255 * -sine);

            float sine_shifted = (float)sin((i + num_samples / 4) * x_step * 2 * M_PI);

            if (sine_shifted >= 0)
                channel3 = (uint8_t)((float)255 * sine_shifted);
            if (sine_shifted <= 0)
                channel4 = (uint8_t)((float)255 * -sine_shifted);

            if (channel1 < 5)
                channel1 = 0;

            if (channel2 < 5)
                channel2 = 0;

            if (channel3 < 5)
                channel3 = 0;

            if (channel4 < 5)
                channel4 = 0;

            pwm_set_duty(0, channel1);
            pwm_set_duty(2, channel2);
            pwm_set_duty(1, channel3);
            pwm_set_duty(3, channel4);

            //printf("%03d %03d %03d %03d\r\n", channel1, channel2, channel3, channel4);

            for (int d = 0; d < delay; d++)
                _delay_ms(1);

            check_break(rs);

            if (!rs->running)
                break;
        }
    }
}

void check_break(sys_runstate_t *rs)
{
    CLRWDT();

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

    if (rs->last_sequence != rs->i2c_buffer[SEQUENCE_REG])
        rs->running = false;
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
