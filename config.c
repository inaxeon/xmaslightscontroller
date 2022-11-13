/* 
 *   File:   config.c
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

#include "project.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <avr/pgmspace.h>

#include "config.h"
#include "util.h"

void load_configuration(sys_config_t *config)
{
    uint16_t config_size = sizeof(sys_config_t);
    if (config_size > 0x100)
    {
        printf("\r\nConfiguration size is too large. Currently %u bytes.", config_size);
        reset();
    }
    
    eeprom_read_data(0, (uint8_t *)config, sizeof(sys_config_t));

    if (config->magic != CONFIG_MAGIC)
    {
        printf("\r\nNo configuration found. Setting defaults\r\n");
        default_configuration(config);
        save_configuration(config);
    }
}

void default_configuration(sys_config_t *config)
{
    config->magic = CONFIG_MAGIC;
    config->start_seq = 0;
}

void save_configuration(sys_config_t *config)
{
    eeprom_write_data(0, (uint8_t *)config, sizeof(sys_config_t));
}
