/*
 *   File:   pwm.c
 *   Author: Matt
 *
 *   Created on 12 November 2022, 05:43
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
#include <stdbool.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "pwm.h"
#include "iopins.h"
#include "util.h"

ISR(TIMER0_OVF_vect)
{
    IO_TOGGLE(DIRA);
}

ISR(TIMER1_OVF_vect)
{
    IO_TOGGLE(DIRB);
}

void pwm_init(void)
{
    IO_OUTPUT(PWM0A);
    IO_OUTPUT(PWM0B);
    IO_OUTPUT(PWM1A);
    IO_OUTPUT(PWM1B);
    IO_OUTPUT(DIRA);
    IO_OUTPUT(DIRB);

    IO_LOW(DIRA);
    IO_LOW(DIRB);

    // Timer 0 setup
    TCCR0A = _BV(WGM00) | _BV(WGM01);
    TCCR0B = _BV(CS00) | _BV(CS02); // 1024 prescaler
    TIMSK0 = _BV(TOIE0); // Interrupt on overflow

    TCCR0A |= _BV(COM0A1) | _BV(COM0B1); // Enable PWM output on OC1A/OC1B

    // Timer 1 setup
    TCCR1A = _BV(WGM11); // 8-Bit mode
    TCCR1B = _BV(CS10) | _BV(CS12); // 1024 prescaler
    TIMSK1 = _BV(TOIE1); // Interrupt on overflow

    TCCR1A |= _BV(COM1A1) | _BV(COM1B1); // Enable PWM output on OC1A/OC1B
}

void pwm_set_duty(uint8_t channel, uint8_t duty)
{
    switch (channel)
    {
        case 0:
            OCR0A = duty;
            break;
        case 1:
            OCR0B = duty;
            break;
        case 2:
            OCR1A = duty;
            break;
        case 3:
            OCR1B = duty;
            break;
    }
}