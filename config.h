/* 
 *   File:   config.h
 *   Author: Matt
 *
 *   Created on 25 November 2014, 15:54
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

#ifndef __CONFIG_H__
#define __CONFIG_H__

typedef struct {
    uint16_t magic;
    uint8_t start_seq;
} sys_config_t;

void load_configuration(sys_config_t *config);
void configuration_bootprompt(sys_config_t *config);

#endif /* __CONFIG_H__ */
