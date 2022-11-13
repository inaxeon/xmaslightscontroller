/*
 *   File:   pwm.h
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

#ifndef __PWM_H__
#define __PWM_H__

void pwm_init(void);
void pwm_set_duty(uint8_t channel, uint8_t duty);

#endif /* __PWM_H__ */
