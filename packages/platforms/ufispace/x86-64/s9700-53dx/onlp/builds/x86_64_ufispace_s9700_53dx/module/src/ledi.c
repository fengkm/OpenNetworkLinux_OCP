/************************************************************
 * <bsn.cl fy=2014 v=onl>
 *
 *           Copyright 2014 Big Switch Networks, Inc.
 *           Copyright 2014 Accton Technology Corporation.
 *
 * Licensed under the Eclipse Public License, Version 1.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *        http://www.eclipse.org/legal/epl-v10.html
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the
 * License.
 *
 * </bsn.cl>
 ************************************************************
 *
 *
 *
 ***********************************************************/
#include <onlp/platformi/ledi.h>
#include <onlplib/file.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "platform_lib.h"

/*
 * Get the information for the given LED OID.
 */
static onlp_led_info_t __onlp_led_info[ONLP_LED_COUNT] =
{
    { }, /* Not used */
    {
        .hdr = {
            .id = ONLP_LED_ID_CREATE(ONLP_LED_SYSTEM),
            .description = "Chassis LED 1 (SYS LED)",
            .poid = ONLP_OID_CHASSIS,
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },
        .caps = ONLP_LED_CAPS_OFF | ONLP_LED_CAPS_YELLOW | ONLP_LED_CAPS_YELLOW_BLINKING |
                ONLP_LED_CAPS_GREEN | ONLP_LED_CAPS_GREEN_BLINKING,
    },
    {
        .hdr = {
            .id = ONLP_LED_ID_CREATE(ONLP_LED_PSU0),
            .description = "Chassis LED 2 (PSU1 LED)",
            .poid = ONLP_OID_CHASSIS,
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },
        .caps = ONLP_LED_CAPS_OFF | ONLP_LED_CAPS_YELLOW | ONLP_LED_CAPS_YELLOW_BLINKING |
                ONLP_LED_CAPS_GREEN | ONLP_LED_CAPS_GREEN_BLINKING,
    },
    {
        .hdr = {
            .id = ONLP_LED_ID_CREATE(ONLP_LED_PSU1),
            .description = "Chassis LED 3 (PSU2 LED)",
            .poid = ONLP_OID_CHASSIS,
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },
        .caps = ONLP_LED_CAPS_OFF | ONLP_LED_CAPS_YELLOW | ONLP_LED_CAPS_YELLOW_BLINKING |
                ONLP_LED_CAPS_GREEN | ONLP_LED_CAPS_GREEN_BLINKING,
    },
    {
        .hdr = {
            .id = ONLP_LED_ID_CREATE(ONLP_LED_FAN),
            .description = "Chassis LED 4 (FAN LED)",
            .poid = ONLP_OID_CHASSIS,
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },
        .caps = ONLP_LED_CAPS_OFF | ONLP_LED_CAPS_YELLOW | ONLP_LED_CAPS_YELLOW_BLINKING |
                ONLP_LED_CAPS_GREEN | ONLP_LED_CAPS_GREEN_BLINKING,
    },
};

static int ufi_ledi_info_get(int local_id, onlp_led_info_t* info)
{
    int ret = ONLP_STATUS_OK;
    int value;
    int sysfs_index;
    int shift, led_val,led_val_color, led_val_blink, led_val_onoff;

    if (local_id == ONLP_LED_SYSTEM) {
        sysfs_index = 0;
        shift = 4;
    } else if (local_id == ONLP_LED_PSU0) {
        sysfs_index = 1;
        shift = 0;
    } else if (local_id == ONLP_LED_PSU1) {
        sysfs_index = 1;
        shift = 4;
    } else if (local_id == ONLP_LED_FAN) {
        sysfs_index = 0;
        shift = 0;
    } else {
        return ONLP_STATUS_E_INTERNAL;
    }
    
    ret = file_read_hex(&value, "/sys/bus/i2c/devices/1-0030/cpld_system_led_%d", sysfs_index);
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }

    led_val = (value >> shift);
    led_val_color = (led_val >> 0) & 1;
    led_val_blink = (led_val >> 2) & 1;
    led_val_onoff = (led_val >> 3) & 1;

    //onoff
    if (led_val_onoff == 0) { 
        info->mode = ONLP_LED_MODE_OFF;
    } else {
        //color
        if (led_val_color == 0) {
            if (led_val_blink == 1) {
                info->mode = ONLP_LED_MODE_YELLOW_BLINKING;
            } else {
                info->mode = ONLP_LED_MODE_YELLOW;
            }
        } else {
            if (led_val_blink == 1) {
                info->mode = ONLP_LED_MODE_GREEN_BLINKING;
            } else {
                info->mode = ONLP_LED_MODE_GREEN;
            }
        }
    }    
    
    return ONLP_STATUS_OK;
}

/**
 * @brief Software initialization of the LED module.
 */
int onlp_ledi_sw_init(void)
{
    lock_init();
    return ONLP_STATUS_OK;
}

/**
 * @brief Hardware initialization of the LED module.
 * @param flags The hardware initialization flags.
 */
int onlp_ledi_hw_init(uint32_t flags)
{
    return ONLP_STATUS_OK;
}

/**
 * @brief Deinitialize the led software module.
 * @note The primary purpose of this API is to properly
 * deallocate any resources used by the module in order
 * faciliate detection of real resouce leaks.
 */
int onlp_ledi_sw_denit(void)
{
    return ONLP_STATUS_OK;
}

/**
 * @brief Validate an LED id.
 * @param id The id.
 */
int onlp_ledi_id_validate(onlp_oid_id_t id)
{
    return ONLP_OID_ID_VALIDATE_RANGE(id, 1, ONLP_LED_MAX-1);
}

/**
 * @brief Get the LED header.
 * @param id The LED OID
 * @param[out] rv  Receives the header.
 */
int onlp_ledi_hdr_get(onlp_oid_t id, onlp_oid_hdr_t* hdr)
{
    int ret = ONLP_STATUS_OK;
    int local_id = ONLP_OID_ID_GET(id);

    /* Set the onlp_led_info_t */
    *hdr = __onlp_led_info[local_id].hdr;

    return ret;
}

/**
 * @brief Get the information for the given LED
 * @param id The LED OID
 * @param[out] rv  Receives the LED information.
 */
int onlp_ledi_info_get(onlp_oid_id_t id, onlp_led_info_t* info)
{
    int ret = ONLP_STATUS_OK;
    int local_id = ONLP_OID_ID_GET(id);

    /* Set the onlp_led_info_t */
    memset(info, 0, sizeof(onlp_led_info_t));
    *info = __onlp_led_info[local_id];
    ONLP_TRY(onlp_ledi_hdr_get(id, &info->hdr));

    switch (local_id) {
        case ONLP_LED_SYSTEM:
        case ONLP_LED_PSU0:
        case ONLP_LED_PSU1:
        case ONLP_LED_FAN:
            ret = ufi_ledi_info_get(local_id, info);
            break;
        default:
            return ONLP_STATUS_E_INTERNAL;
            break;
    }

    return ret;
}

/**
 * @brief Get the caps for the given LED
 * @param id The LED ID
 * @param[out] rv Receives the caps.
 */
int onlp_ledi_caps_get(onlp_oid_id_t id, uint32_t* rv)
{
    int local_id = ONLP_OID_ID_GET(id);

    *rv = __onlp_led_info[local_id].caps;

    return ONLP_STATUS_OK;
}

/**
 * @brief Set the LED mode.
 * @param id The LED OID
 * @param mode The new mode.
 * @note Only called if the mode is advertised in the LED capabilities.
 */
int onlp_ledi_mode_set(onlp_oid_id_t id, onlp_led_mode_t mode)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

/**
 * @brief Set the LED character.
 * @param id The LED OID
 * @param c The character..
 * @note Only called if the char capability is set.
 */
int onlp_ledi_char_set(onlp_oid_id_t id, char c)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

