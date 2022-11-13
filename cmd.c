/*
 *   File:   cmd.c
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/pgmspace.h>

#include "project.h"
#include "config.h"
#include "cmd.h"
#include "pwm.h"
#include "usart.h"
#include "util.h"

#define CMD_NONE              0x00
#define CMD_READLINE          0x01
#define CMD_COMPLETE          0x02
#define CMD_ESCAPE            0x03
#define CMD_AWAIT_NAV         0x04
#define CMD_PREVCOMMAND       0x05
#define CMD_NEXTCOMMAND       0x06
#define CMD_DEL               0x07
#define CMD_DROP_NAV          0x08
#define CMD_CANCEL            0x10

#define CTL_CANCEL            0x03
#define CTL_XOFF              0x13
#define CTL_U                 0x15

#define SEQ_ESCAPE_CHAR       0x1B
#define SEQ_CTRL_CHAR1        0x5B
#define SEQ_ARROW_UP          0x41
#define SEQ_ARROW_DOWN        0x42
#define SEQ_HOME              0x31
#define SEQ_INS               0x32
#define SEQ_DEL               0x33
#define SEQ_END               0x34
#define SEQ_PGUP              0x35
#define SEQ_PGDN              0x36
#define SEQ_NAV_END           0x7E

#define PARAM_U8              0
#define PARAM_U16             1
#define PARAM_U16_F10         2

#define CMD_MAX_CONSOLE       1
#define CMD_MAX_LINE          64
#define CMD_MAX_HISTORY       4

typedef struct
{
    char cmd_buf[CMD_MAX_LINE];
    char cmd_history[CMD_MAX_HISTORY][CMD_MAX_LINE];
    uint8_t next_history;
    uint8_t show_history;
    uint8_t max_history;
    int8_t count;
    uint8_t state;
    uint8_t ignore_lf;
    
} cmd_state_t;

static void cmd_prompt(cmd_state_t *ccmd);
static void cmd_erase_line(cmd_state_t *ccmd);
static bool parse_param(void *param, uint8_t type, char *arg);

uint8_t _g_current_console;
cmd_state_t _g_cmd[CMD_MAX_CONSOLE];

static void do_help(void)
{
    printf(
        "\r\nCommands:\r\n\r\n"
        "\tshow\r\n"
        "\t\tShow current configuration\r\n\r\n"
        "\tdefault\r\n"
        "\t\tLoad the default configuration\r\n\r\n"
        "\tsave\r\n"
        "\t\tSave current configuration\r\n\r\n"
        "\tstartseq\r\n"
        "\t\tStartup seqience\r\n\r\n"
    );
}

static void do_show(sys_config_t *config)
{
    printf(
            "\r\nCurrent configuration:\r\n\r\n"
            "\tstartseq ...........: %u\r\n"
            "\r\n",
            config->start_seq
    );
}

static bool command_prompt_handler(char *text, sys_config_t *config)
{
    char *command;
    char *arg;

    command = strtok(text, " ");
    arg = strtok(NULL, "");

    if (!stricmp(command, "startseq"))
    {
        uint8_t ret = parse_param(&config->start_seq, PARAM_U8, arg);
        return ret;
    }
    else if (!stricmp(command, "show"))
    {
        do_show(config);
        return true;
    }
    else if (!stricmp(command, "save"))
    {
        save_configuration(config);
        printf("\r\nConfiguration saved.\r\n\r\n");
        return true;
    }
    else if (!stricmp(command, "default"))
    {
        default_configuration(config);
        printf("\r\nDefault configuration loaded.\r\n\r\n");
        return true;
    }
    else if (!stricmp(command, "reset"))
    {
        printf("\r\n");
        while (console_busy());
        reset();
        return true;
    }
    else if ((!stricmp(command, "help") || !stricmp(command, "?"))) {
        do_help();
        return true;
    }

    printf("Error: No such command (%s)\r\n", command);
    return false;
}


static bool parse_param(void *param, uint8_t type, char *arg)
{
    uint8_t u8param;
    uint16_t u16param;
 
    if (!arg || !*arg)
    {
        printf("Error: Missing parameter\r\n");
        return false;
    }

    switch (type)
    {
    case PARAM_U8:
        if (*arg == '-')
            return false;
        u8param = (uint8_t)atoi(arg);
        *(uint8_t *)param = u8param;
        break;
    case PARAM_U16:
        if (*arg == '-')
            return false;
        u16param = (uint16_t)atoi(arg);
        *(uint16_t *)param = u16param;
        break;
    case PARAM_U16_F10:
    if (*arg == '-')
            return false;
        u16param = (uint16_t)(atof(arg) * 10);
        *(uint16_t *)param = u16param;
        break;
    }
    
    return true;
}

void putch(char byte)
{
    if (_g_current_console == CONSOLE_1)
        console_put(byte);
}

void cmd_init(void)
{
    uint8_t i;

    for (i = 0; i < CMD_MAX_CONSOLE; i++)
    {
        _g_cmd[i].count = 0;
        _g_cmd[i].state = CMD_NONE;
        _g_cmd[i].next_history = 0;
        _g_cmd[i].show_history = 0;
        _g_cmd[i].max_history = 0;
        _g_cmd[i].ignore_lf = 0;
        memset(_g_cmd[i].cmd_history, 0, CMD_MAX_HISTORY * CMD_MAX_LINE);
        memset(_g_cmd[i].cmd_buf, 0, CMD_MAX_LINE);
    }
    
    _g_current_console = CONSOLE_1;
}

void cmd_process_state(sys_config_t *config)
{
    uint8_t idx, i;

    for (idx = 0; idx < CMD_MAX_CONSOLE; idx++)
    {
        cmd_state_t *ccmd = &_g_cmd[idx];
        _g_current_console = idx;
        
        if (ccmd->state == CMD_NONE)
        {
            printf("\r\n");
            cmd_prompt(ccmd);
        }
        else if (ccmd->state == CMD_PREVCOMMAND)
        {
            uint8_t previdx;

            if (!ccmd->max_history)
            {
                ccmd->state = CMD_READLINE;
                return;
            }

            if (ccmd->count)
                cmd_erase_line(ccmd);

            if (ccmd->show_history == 0)
                ccmd->show_history = CMD_MAX_HISTORY;

            previdx = --ccmd->show_history;

            strcpy(ccmd->cmd_buf, ccmd->cmd_history[previdx]);
            ccmd->count = strlen(ccmd->cmd_buf);
            printf("%s", ccmd->cmd_buf);

            ccmd->state = CMD_READLINE;

        }
        else if (ccmd->state == CMD_NEXTCOMMAND)
        {
            uint8_t previdx;

            if (!ccmd->max_history)
            {
                ccmd->state = CMD_READLINE;
                return;
            }

            if (ccmd->count)
                cmd_erase_line(ccmd);

            previdx = ++ccmd->show_history;

            if (ccmd->show_history == CMD_MAX_HISTORY)
            {
                ccmd->show_history = 0;
                previdx = 0;
            }

            strcpy(ccmd->cmd_buf, ccmd->cmd_history[previdx]);
            ccmd->count = strlen(ccmd->cmd_buf);
            printf("%s", ccmd->cmd_buf);

            ccmd->state = CMD_READLINE;
        }
        else if (ccmd->state == CMD_COMPLETE)
        {
            ccmd->cmd_buf[ccmd->count] = 0;
            printf("\r\n");
            
            if (ccmd->count > 0)
            {
                int8_t tostore = -1;
                bool ret;

                if (ccmd->next_history >= CMD_MAX_HISTORY)
                    ccmd->next_history = 0;
                else
                    ccmd->max_history++;

                for (i = 0; i < CMD_MAX_HISTORY; i++)
                {
                    if (!strcasecmp(ccmd->cmd_history[i], ccmd->cmd_buf))
                    {
                        tostore = i;
                        break;
                    }
                }

                if (tostore < 0)
                {
                    // Don't have this command in history. Store it
                    strcpy(ccmd->cmd_history[ccmd->next_history], ccmd->cmd_buf);
                    ccmd->next_history++;
                    ccmd->show_history = ccmd->next_history;
                }
                else
                {
                    // Already have this command in history, set the 'up' arrow to retrieve it.
                    tostore++;

                    if (tostore == CMD_MAX_HISTORY)
                        tostore = 0;

                    ccmd->show_history = tostore;
                }
                
                ret = command_prompt_handler(ccmd->cmd_buf, config);

                if (!ret)
                    printf("Error: Command failed\r\n");
            }
            
            cmd_prompt(ccmd);
        }
        else if (ccmd->state == CMD_CANCEL)
        {
            ccmd->cmd_buf[ccmd->count] = 0;
            printf("\r\n");
            cmd_prompt(ccmd);
        }
    }
}

static void cmd_erase_line(cmd_state_t *ccmd)
{
    printf("%c[%dD%c[K", SEQ_ESCAPE_CHAR, ccmd->count, SEQ_ESCAPE_CHAR);
}

static void cmd_prompt(cmd_state_t *ccmd)
{
    ccmd->state = CMD_READLINE;
    ccmd->count = 0;
    printf("sc>");
}

void cmd_process_char(uint8_t c, uint8_t idx)
{
    cmd_state_t *ccmd = &_g_cmd[idx];
    _g_current_console = idx;
    
    if (ccmd->state == CMD_ESCAPE) {
        if (c == SEQ_CTRL_CHAR1) {
            ccmd->state = CMD_AWAIT_NAV;
            return;
        }
        else {
            ccmd->state = CMD_READLINE;
            return;
        }
    }
    else if (ccmd->state == CMD_AWAIT_NAV)
    {
        if (c == SEQ_ARROW_UP) {
            ccmd->state = CMD_PREVCOMMAND;
            return;
        }
        else if (c == SEQ_ARROW_DOWN) {
            ccmd->state = CMD_NEXTCOMMAND;
            return;
        }
        else if (c == SEQ_DEL) {
            ccmd->state = CMD_DEL;
            return;
        }
        else if (c == SEQ_HOME || c == SEQ_END || c == SEQ_INS || c == SEQ_PGUP || c == SEQ_PGDN) {
            ccmd->state = CMD_DROP_NAV;
            return;
        }
        else {
            ccmd->state = CMD_READLINE;
            return;
        }
    }
    else if (ccmd->state == CMD_DEL) {
        if (c == SEQ_NAV_END && ccmd->count) {
            putch('\b');
            putch(' ');
            putch('\b');
            ccmd->count--;
        }

        ccmd->state = CMD_READLINE;
        return;
    }
    else if (ccmd->state == CMD_DROP_NAV) {
        ccmd->state = CMD_READLINE;
        return;
    }
    else
    {
        if (ccmd->count >= (int8_t)sizeof(ccmd->cmd_buf)) {
            ccmd->count--;
            ccmd->state = CMD_COMPLETE;
            return;
        }

        if (c == CTL_XOFF) /* Swallow XOFF */
            return;
        
        if (c == CTL_U) {
            if (ccmd->count) {
                cmd_erase_line(ccmd);
                *(ccmd->cmd_buf) = 0;
                ccmd->count = 0;
            }
            return;
        }

        if (c == SEQ_ESCAPE_CHAR) {
            ccmd->state = CMD_ESCAPE;
            return;
        }

        /* Unix telnet sends:    <CR> <NUL>
         * Windows telnet sends: <CR> <LF>
         */
        if (ccmd->ignore_lf && (c == '\n' || c == 0x00)) {
            ccmd->ignore_lf = 0;
            return;
        }

        if (c == CTL_CANCEL) { /* Ctrl+C */
            ccmd->state = CMD_CANCEL;
            return;
        }

        if (c == '\b' || c == 0x7F) {
            if (!ccmd->count)
                return;

            putch('\b');
            putch(' ');
            putch('\b');
            ccmd->count--;
            return;
        }
        if (c != '\n' && c != '\r') {
            putch(c);
        }
        else {
            if (c == '\r') {
                ccmd->ignore_lf = 1;
                ccmd->state = CMD_COMPLETE;
                return;
            }

            if (c == '\n')
            {
                ccmd->state = CMD_COMPLETE;
                return;
            }
        }

        ccmd->cmd_buf[ccmd->count] = c;
        ccmd->count++;
    }
}

void cmd_process(sys_config_t *config)
{
    uint8_t i;
    for (i = 0; i < CMD_MAX_CONSOLE; i++)
    {
        if (console_data_ready())
            cmd_process_char(console_get(), CONSOLE_1);
    }

    cmd_process_state(config);
}