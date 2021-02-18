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
 * LED Implementation
 *
 ***********************************************************/
#include <stdio.h>
#include <string.h>
#include <onlp/platformi/ledi.h>
#include <onlplib/file.h>
#include <onlplib/i2c.h>
//#include <sys/mman.h>
//#include <fcntl.h>

#include "platform_lib.h"

/**
 * Get the information for the given LED OID.
 * 
 * [01] CHASSIS----[01] ONLP_LED_SYSTEM
 *            |----[02] ONLP_LED_PSU1
 *            |----[03] ONLP_LED_PSU2
 *            |----[04] ONLP_LED_FAN
 *            |
 *            |----[01] ONLP_FAN_1----[05] ONLP_LED_FAN_TRAY1
 *            |----[02] ONLP_FAN_2----[05] ONLP_LED_FAN_TRAY1
 *            |----[03] ONLP_FAN_3----[06] ONLP_LED_FAN_TRAY2
 *            |----[04] ONLP_FAN_4----[06] ONLP_LED_FAN_TRAY2
 *            |----[05] ONLP_FAN_5----[07] ONLP_LED_FAN_TRAY3
 *            |----[06] ONLP_FAN_6----[07] ONLP_LED_FAN_TRAY3
 *            |----[07] ONLP_FAN_7----[08] ONLP_LED_FAN_TRAY4
 *            |----[08] ONLP_FAN_8----[08] ONLP_LED_FAN_TRAY4
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
        .caps = ONLP_LED_CAPS_OFF | ONLP_LED_CAPS_ORANGE | ONLP_LED_CAPS_GREEN,
        .mode = ONLP_LED_MODE_GREEN,
    },  
    {   
        .hdr = { 
            .id = ONLP_LED_ID_CREATE(ONLP_LED_PSU1),
            .description = "Chassis LED 2 (PSU1 LED)",
            .poid = ONLP_OID_CHASSIS,
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },  
        .caps = ONLP_LED_CAPS_OFF | ONLP_LED_CAPS_ORANGE | ONLP_LED_CAPS_GREEN,
        .mode = ONLP_LED_MODE_GREEN,
    },  
    {   
        .hdr = { 
            .id = ONLP_LED_ID_CREATE(ONLP_LED_PSU2),
            .description = "Chassis LED 3 (PSU2 LED)",
            .poid = ONLP_OID_CHASSIS,
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },  
        .caps = ONLP_LED_CAPS_OFF | ONLP_LED_CAPS_ORANGE | ONLP_LED_CAPS_GREEN,
        .mode = ONLP_LED_MODE_GREEN,
    },  
    {   
        .hdr = { 
            .id = ONLP_LED_ID_CREATE(ONLP_LED_FAN),
            .description = "Chassis LED 4 (FAN LED)",
            .poid = ONLP_OID_CHASSIS,
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },  
        .caps = ONLP_LED_CAPS_OFF | ONLP_LED_CAPS_ORANGE | ONLP_LED_CAPS_GREEN,
        .mode = ONLP_LED_MODE_GREEN,
    },  
    {   
        .hdr = { 
            .id = ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY1),
            .description = "Rear LED 1 (FAN TRAY1 LED)",
            .poid = ONLP_FAN_ID_CREATE(ONLP_FAN_1),
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },  
        .caps = ONLP_LED_CAPS_OFF | ONLP_LED_CAPS_ORANGE | ONLP_LED_CAPS_GREEN,
        .mode = ONLP_LED_MODE_GREEN,
    },  
    {   
        .hdr = { 
            .id = ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY2),
            .description = "Rear LED 2 (FAN TRAY2 LED)",
            .poid = ONLP_FAN_ID_CREATE(ONLP_FAN_3),
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },  
        .caps = ONLP_LED_CAPS_OFF | ONLP_LED_CAPS_ORANGE | ONLP_LED_CAPS_GREEN,
        .mode = ONLP_LED_MODE_GREEN,
    },  
    {   
        .hdr = { 
            .id = ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY3),
            .description = "Rear LED 3 (FAN TRAY3 LED)",
            .poid = ONLP_FAN_ID_CREATE(ONLP_FAN_5),
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },  
        .caps = ONLP_LED_CAPS_OFF | ONLP_LED_CAPS_ORANGE | ONLP_LED_CAPS_GREEN,
        .mode = ONLP_LED_MODE_GREEN,
    },  
    {   
        .hdr = { 
            .id = ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY4),
            .description = "Rear LED 4 (FAN TRAY4 LED)",
            .poid = ONLP_FAN_ID_CREATE(ONLP_FAN_7),
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },  
        .caps = ONLP_LED_CAPS_OFF | ONLP_LED_CAPS_ORANGE | ONLP_LED_CAPS_GREEN,
        .mode = ONLP_LED_MODE_GREEN,
    },  
};


/**
 * @brief Update the status of LED's oid header.
 * @param id The LED ID.
 * @param[out] hdr Receives the header.
 */
static int update_ledi_status(int local_id, onlp_oid_hdr_t* hdr)
{
    int fan_presence = 0;

    /* clear ledi status */
    hdr->status = 0;

    /* get led present status */
    if (local_id >= ONLP_LED_SYSTEM && local_id <= ONLP_LED_FAN) {
        ONLP_OID_STATUS_FLAG_SET(hdr, PRESENT);
    } else if (local_id >= ONLP_LED_FAN_TRAY1 && local_id <= ONLP_LED_FAN_TRAY4) {
        if (local_id == ONLP_LED_FAN_TRAY1) {
            fan_presence = get_fan_present_status(ONLP_FAN_1);
        } else if (local_id == ONLP_LED_FAN_TRAY2) {
            fan_presence = get_fan_present_status(ONLP_FAN_3);
        } else if (local_id == ONLP_LED_FAN_TRAY3) {
            fan_presence = get_fan_present_status(ONLP_FAN_5);
        } else if (local_id == ONLP_LED_FAN_TRAY4) {
            fan_presence = get_fan_present_status(ONLP_FAN_7);
        }
        if (fan_presence == 0) {
            ONLP_OID_STATUS_FLAG_SET(hdr, UNPLUGGED);
        } else if (fan_presence == 1) {
            ONLP_OID_STATUS_FLAG_SET(hdr, PRESENT);
        } else {
            return ONLP_STATUS_E_INTERNAL;
        }
    } else {
        AIM_LOG_ERROR("unknown LED id (%d), func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_PARAM;
    }

    return ONLP_STATUS_OK;
}


/**
 * @brief Update the information structure for the given System LED
 * @param id The LED Local ID
 * @param[out] info Receives the FAN information.
 */
static int update_ledi_system_info(int local_id, onlp_led_info_t* info)
{
    int ret = ONLP_STATUS_OK;

    if (local_id != ONLP_LED_SYSTEM) {
        AIM_LOG_ERROR("incorrect parameter, id=%d, func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_PARAM;
    }

    //set system led to green
    ret = onlp_i2c_modifyb(9, 0x22, 0x02, 0x3F, 0x40, ONLP_I2C_F_FORCE);
    if (ret == ONLP_STATUS_OK) {
        info->mode = ONLP_LED_MODE_GREEN;
    } else {
        return ONLP_STATUS_E_INTERNAL;
    }

    return ONLP_STATUS_OK;
}


/**
 * @brief Update the information structure for the given PSU LED
 * @param id The LED Local ID
 * @param[out] info Receives the FAN information.
 */
static int update_ledi_psu_info(int local_id, onlp_led_info_t* info)
{
    int ret = ONLP_STATUS_OK;
    int psu_id = 0;
    int psu_presence = 0;
    int psu_pwgood = 0;

    if (local_id == ONLP_LED_PSU1) {
        psu_id = ONLP_PSU_1; 
    } else if (local_id == ONLP_LED_PSU2) {
        psu_id = ONLP_PSU_2; 
    } else {
        AIM_LOG_ERROR("incorrect parameter, id=%d, func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_PARAM;
    }

    psu_presence = get_psu_present_status(psu_id);
    psu_pwgood = get_psu_pwgood_status(psu_id);

    if (psu_presence < 0 || psu_pwgood < 0) {
        return ONLP_STATUS_E_INTERNAL;
    }

    if (local_id == ONLP_LED_PSU1) {
        if (psu_presence == 1 && psu_pwgood == 1) {
            /* LED GREEN */
            ret = onlp_i2c_modifyb(9, 0x22, 0x02, 0b11111100, 0b00000001, ONLP_I2C_F_FORCE);
            if (ret == ONLP_STATUS_OK) {
                info->mode = ONLP_LED_MODE_GREEN;
            } else {
                return ONLP_STATUS_E_INTERNAL;
            }
        } else if (psu_presence == 1 && psu_pwgood == 0) {
            /* LED ORANGE */
            ret = onlp_i2c_modifyb(9, 0x22, 0x02, 0b11111100, 0b00000010, ONLP_I2C_F_FORCE);
            if (ret == ONLP_STATUS_OK) {
                info->mode = ONLP_LED_MODE_ORANGE;
            } else {
                return ONLP_STATUS_E_INTERNAL;
            }
        } else {
            /* LED OFF */
            ret = onlp_i2c_modifyb(9, 0x22, 0x02, 0b11111100, 0b00000011, ONLP_I2C_F_FORCE);
            if (ret == ONLP_STATUS_OK) {
                info->mode = ONLP_LED_MODE_OFF;
            } else {
                return ONLP_STATUS_E_INTERNAL;
            }
        }

    } else if (local_id == ONLP_LED_PSU2) {
        if (psu_presence == 1 && psu_pwgood == 1) {
            /* LED GREEN */
            ret = onlp_i2c_modifyb(9, 0x22, 0x02, 0b11001111, 0b00010000, ONLP_I2C_F_FORCE);
            if (ret == ONLP_STATUS_OK) {
                info->mode = ONLP_LED_MODE_GREEN;
            } else {
                return ONLP_STATUS_E_INTERNAL;
            }
        } else if (psu_presence == 1 && psu_pwgood == 0) {
            /* LED ORANGE */
            ret = onlp_i2c_modifyb(9, 0x22, 0x02, 0b11001111, 0b00100000, ONLP_I2C_F_FORCE);
            if (ret == ONLP_STATUS_OK) {
                info->mode = ONLP_LED_MODE_ORANGE;
            } else {
                return ONLP_STATUS_E_INTERNAL;
            }
        } else {
            /* LED OFF */
            ret = onlp_i2c_modifyb(9, 0x22, 0x02, 0b11001111, 0b00110000, ONLP_I2C_F_FORCE);
            if (ret == ONLP_STATUS_OK) {
                info->mode = ONLP_LED_MODE_OFF;
            } else {
                return ONLP_STATUS_E_INTERNAL;
            }
        }
    }

    return ONLP_STATUS_OK;
}


/**
 * @brief Update the information structure for the given FAN LED
 * @param id The LED Local ID
 * @param[out] info Receives the FAN information.
 */
static int update_ledi_fan_info(int local_id, onlp_led_info_t* info)
{
    int ret = ONLP_STATUS_OK;
    int fan_presence = 0;
    int fan_operational = 0;
    int fan_absent_flag = 0; //1: all fans are absent.
    int fan_failed_flag = 0; //1: at least one fan is failed
    int i = 0;

    if (local_id != ONLP_LED_FAN) {
        AIM_LOG_ERROR("incorrect parameter, id=%d, func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_PARAM;
    }

    /* If all fans are absent, turn off the FAN LED. */
    for(i=ONLP_FAN_1;i<=ONLP_FAN_8;i++) {
        fan_presence = get_fan_present_status(i);
        if (fan_presence == 1) {
            fan_absent_flag = 0;
            break;
        } else {
            fan_absent_flag = 1;
        }
    }

    /* If one of the Fans is alarm, set FAN LED to Orange. */
    for(i=ONLP_FAN_1;i<=ONLP_FAN_8;i++) {
        fan_operational = get_fan_operational_status(i);
        if (fan_operational == 0) {
            fan_failed_flag = 1;
            break;
        } else {
            fan_failed_flag = 0;
        }
    }

    if (fan_absent_flag == 1) {
        /* LED OFF */
        ret = onlp_i2c_modifyb(9, 0x22, 0x02, 0b11110011, 0b00001100, ONLP_I2C_F_FORCE);
        if (ret == ONLP_STATUS_OK) {
            info->mode = ONLP_LED_MODE_OFF;
        } else {
            return ONLP_STATUS_E_INTERNAL;
        }
    } else if (fan_failed_flag == 0) {
        /* LED GREEN */
        ret = onlp_i2c_modifyb(9, 0x22, 0x02, 0b11110011, 0b00000100, ONLP_I2C_F_FORCE);
        if (ret == ONLP_STATUS_OK) {
            info->mode = ONLP_LED_MODE_GREEN;
        } else {
            return ONLP_STATUS_E_INTERNAL;
        }
    } else if (fan_failed_flag == 1) {
        /* LED ORANGE */
        ret = onlp_i2c_modifyb(9, 0x22, 0x02, 0b11110011, 0b00001000, ONLP_I2C_F_FORCE);
        if (ret == ONLP_STATUS_OK) {
            info->mode = ONLP_LED_MODE_ORANGE;
        } else {
            return ONLP_STATUS_E_INTERNAL;
        }
    } else {
        AIM_LOG_ERROR("unknown error, fan_absent_flag=%d, fan_failed_flag=%d, func=%s\n", fan_absent_flag, fan_failed_flag, __FUNCTION__);
        return ONLP_STATUS_E_INTERNAL;
    }


    return ONLP_STATUS_OK;
}


/**
 * @brief Update the information structure for the given FANTRAY LED
 * @param id The LED Local ID
 * @param[out] info Receives the FAN information.
 */
static int update_ledi_fantray_info(int local_id, onlp_led_info_t* info)
{
    int ret = ONLP_STATUS_OK;
    int fan_presence = 0;
    int fan_operational = 0;

    if (local_id < ONLP_LED_FAN_TRAY1 || local_id > ONLP_LED_FAN_TRAY4) {
        AIM_LOG_ERROR("incorrect parameter, id=%d, func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_PARAM;
    }

    if (local_id == ONLP_LED_FAN_TRAY1) {
        fan_presence = get_fan_present_status(ONLP_FAN_1);
        fan_operational = get_fan_operational_status(ONLP_FAN_1);
        
        if (fan_presence < 0 || fan_operational < 0) {
            return ONLP_STATUS_E_INTERNAL;
        }

        if (fan_presence == 0) {
            /* LED OFF */
            info->mode = ONLP_LED_MODE_OFF;
        } else if (fan_operational == 1) {
            /* LED GREEN */
            ret = onlp_i2c_modifyb(9, 0x20, 0x02, 0b11111100, 0b00000001, ONLP_I2C_F_FORCE);
            if (ret == ONLP_STATUS_OK) {
                info->mode = ONLP_LED_MODE_GREEN;
            } else {
                return ONLP_STATUS_E_INTERNAL;
            }
        } else if (fan_operational == 0) {
            /* LED ORANGE */
            ret = onlp_i2c_modifyb(9, 0x20, 0x02, 0b11111100, 0b00000010, ONLP_I2C_F_FORCE);
            if (ret == ONLP_STATUS_OK) {
                info->mode = ONLP_LED_MODE_ORANGE;
            } else {
                return ONLP_STATUS_E_INTERNAL;
            }
        } else {
            AIM_LOG_ERROR("unknown error, fan_presence=%d, fan_operational=%d, func=%s\n", fan_presence, fan_operational, __FUNCTION__);
            return ONLP_STATUS_E_INTERNAL;
        }
    } else if (local_id == ONLP_LED_FAN_TRAY2) {
        fan_presence = get_fan_present_status(ONLP_FAN_3);
        fan_operational = get_fan_operational_status(ONLP_FAN_3);
        
        if (fan_presence < 0 || fan_operational < 0) {
            return ONLP_STATUS_E_INTERNAL;
        }

        if (fan_presence == 0) {
            /* LED OFF */
            info->mode = ONLP_LED_MODE_OFF;
        } else if (fan_operational == 1) {
            /* LED GREEN */
            ret = onlp_i2c_modifyb(9, 0x20, 0x02, 0b11001111, 0b00010000, ONLP_I2C_F_FORCE);
            if (ret == ONLP_STATUS_OK) {
                info->mode = ONLP_LED_MODE_GREEN;
            } else {
                return ONLP_STATUS_E_INTERNAL;
            }
        } else if (fan_operational == 0) {
            /* LED ORANGE */
            ret = onlp_i2c_modifyb(9, 0x20, 0x02, 0b11001111, 0b00100000, ONLP_I2C_F_FORCE);
            if (ret == ONLP_STATUS_OK) {
                info->mode = ONLP_LED_MODE_ORANGE;
            } else {
                return ONLP_STATUS_E_INTERNAL;
            }
        } else {
            AIM_LOG_ERROR("unknown error, fan_presence=%d, fan_operational=%d, func=%s\n", fan_presence, fan_operational, __FUNCTION__);
            return ONLP_STATUS_E_INTERNAL;
        }
    } else if (local_id == ONLP_LED_FAN_TRAY3) {
        fan_presence = get_fan_present_status(ONLP_FAN_5);
        fan_operational = get_fan_operational_status(ONLP_FAN_5);
        
        if (fan_presence < 0 || fan_operational < 0) {
            return ONLP_STATUS_E_INTERNAL;
        }

        if (fan_presence == 0) {
            /* LED OFF */
            info->mode = ONLP_LED_MODE_OFF;
        } else if (fan_operational == 1) {
            /* LED GREEN */
            ret = onlp_i2c_modifyb(9, 0x20, 0x03, 0b11111100, 0b00000010, ONLP_I2C_F_FORCE);
            if (ret == ONLP_STATUS_OK) {
                info->mode = ONLP_LED_MODE_GREEN;
            } else {
                return ONLP_STATUS_E_INTERNAL;
            }
        } else if (fan_operational == 0) {
            /* LED ORANGE */
            ret = onlp_i2c_modifyb(9, 0x20, 0x03, 0b11111100, 0b00000001, ONLP_I2C_F_FORCE);
            if (ret == ONLP_STATUS_OK) {
                info->mode = ONLP_LED_MODE_ORANGE;
            } else {
                return ONLP_STATUS_E_INTERNAL;
            }
        } else {
            AIM_LOG_ERROR("unknown error, fan_presence=%d, fan_operational=%d, func=%s\n", fan_presence, fan_operational, __FUNCTION__);
            return ONLP_STATUS_E_INTERNAL;
        }
    } else if (local_id == ONLP_LED_FAN_TRAY4) {
        fan_presence = get_fan_present_status(ONLP_FAN_7);
        fan_operational = get_fan_operational_status(ONLP_FAN_7);
        
        if (fan_presence < 0 || fan_operational < 0) {
            return ONLP_STATUS_E_INTERNAL;
        }

        if (fan_presence == 0) {
            /* LED OFF */
            info->mode = ONLP_LED_MODE_OFF;
        } else if (fan_operational == 1) {
            /* LED GREEN */
            ret = onlp_i2c_modifyb(9, 0x20, 0x03, 0b11001111, 0b00010000, ONLP_I2C_F_FORCE);
            if (ret == ONLP_STATUS_OK) {
                info->mode = ONLP_LED_MODE_GREEN;
            } else {
                return ONLP_STATUS_E_INTERNAL;
            }
        } else if (fan_operational == 0) {
            /* LED ORANGE */
            ret = onlp_i2c_modifyb(9, 0x20, 0x03, 0b11001111, 0b00100000, ONLP_I2C_F_FORCE);
            if (ret == ONLP_STATUS_OK) {
                info->mode = ONLP_LED_MODE_ORANGE;
            } else {
                return ONLP_STATUS_E_INTERNAL;
            }
        } else {
            AIM_LOG_ERROR("unknown error, fan_presence=%d, fan_operational=%d, func=%s\n", fan_presence, fan_operational, __FUNCTION__);
            return ONLP_STATUS_E_INTERNAL;
        }
    }

    return ONLP_STATUS_OK;
}


/**
 * @brief Update the information structure for the given LED
 * @param id The LED Local ID
 * @param[out] info Receives the FAN information.
 */
static int update_ledi_info(int local_id, onlp_led_info_t* info)
{
    int ret = ONLP_STATUS_OK;

    if (local_id == ONLP_LED_SYSTEM) {
        /* update system ledi info */
        ret = update_ledi_system_info(local_id, info);
        if (ret != ONLP_STATUS_OK) {
            return ret;
        }
    } else if (local_id == ONLP_LED_PSU1 || local_id == ONLP_LED_PSU2) {
        /* update psu ledi info */
        ret = update_ledi_psu_info(local_id, info);
        if (ret != ONLP_STATUS_OK) {
            return ret;
        }
    } else if (local_id == ONLP_LED_FAN) {
        /* update fan ledi info */
        ret = update_ledi_fan_info(local_id, info);
        if (ret != ONLP_STATUS_OK) {
            return ret;
        }
    } else if (local_id >= ONLP_LED_FAN_TRAY1 && local_id <= ONLP_LED_FAN_TRAY4) {
        /* update fantray ledi info */
        ret = update_ledi_fantray_info(local_id, info);
        if (ret != ONLP_STATUS_OK) {
            return ret;
        }
    } else {
        AIM_LOG_ERROR("unknown LED id (%d), func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_PARAM;
    }

    return ONLP_STATUS_OK;
}


/**
 * @brief Software initialization of the LED module.
 */
int onlp_ledi_sw_init(void)
{
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

    /* Update onlp_oid_hdr_t */
    ret = update_ledi_status(local_id, hdr);

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

    //get and update ledi info
    if (local_id >= ONLP_LED_SYSTEM && local_id <= ONLP_LED_FAN_TRAY4) {
        ret = update_ledi_info(local_id, info);
    } else {
        AIM_LOG_ERROR("unknown LED id (%d), func=%s\n", local_id, __FUNCTION__);
        ret = ONLP_STATUS_E_PARAM;
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

