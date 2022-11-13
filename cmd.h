/*
 *   File:   cmd.h
 *   Author: Matt
 *
 *   Created on 30 July 2018, 07:00
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

#include <stdint.h>

#ifndef __CMD_H__
#define __CMD_H__

#define CONSOLE_1       0x00

void cmd_process(sys_config_t *config);
void cmd_init(void);

#endif /* __CMD_H__ */