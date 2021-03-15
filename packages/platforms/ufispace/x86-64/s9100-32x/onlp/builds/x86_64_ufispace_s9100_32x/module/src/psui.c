/************************************************************
 * <bsn.cl fy=2014 v=onl>
 *
 *        Copyright 2014, 2015 Big Switch Networks, Inc.
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
 * PSU
 *
 ***********************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <onlp/platformi/psui.h>
#include <onlplib/file.h>
#include <onlplib/i2c.h>
#include <AIM/aim.h>

#include "platform_lib.h"

/**
 * Get all information about the given PSU oid.
 *
 * [01] CHASSIS----[01] ONLP_PSU_1----[07] ONLP_THERMAL_PSU1_1
 *            |                  |----[08] ONLP_THERMAL_PSU1_2
 *            |                  |----[09] ONLP_PSU1_FAN_1
 *            |
 *            |----[02] ONLP_PSU_1----[09] ONLP_THERMAL_PSU2_1
 *            |                  |----[10] ONLP_THERMAL_PSU2_2
 *            |                  |----[10] ONLP_PSU2_FAN_1
 */
static onlp_psu_info_t __onlp_psu_info[ONLP_PSU_COUNT] = { 
    { }, /* Not used */
    {   
        .hdr = { 
            .id = ONLP_PSU_ID_CREATE(ONLP_PSU_1),
            .description = "PSU-1",
            .poid = 0,
            .coids = { 
                ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_PSU1_1),
                ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_PSU1_2),
                ONLP_FAN_ID_CREATE(ONLP_PSU1_FAN_1),
            },  
        },
        .status = ONLP_PSU_STATUS_PRESENT,
        .model = "", 
        .serial = "", 
        .caps = ONLP_PSU_CAPS_VOUT | ONLP_PSU_CAPS_IOUT | ONLP_PSU_CAPS_PIN | ONLP_PSU_CAPS_POUT,
    },  
    {   
        .hdr = { 
            .id = ONLP_PSU_ID_CREATE(ONLP_PSU_2),
            .description = "PSU-2",
            .poid = 0,
            .coids = {
                ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_PSU2_1),
                ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_PSU2_2),
                ONLP_FAN_ID_CREATE(ONLP_PSU2_FAN_1),
            },
        },
        .status = ONLP_PSU_STATUS_PRESENT,
        .model = "",
        .serial = "",
        .caps = ONLP_PSU_CAPS_VOUT | ONLP_PSU_CAPS_IOUT | ONLP_PSU_CAPS_PIN | ONLP_PSU_CAPS_POUT,
    },
};


/**
 * @brief Update the status of PSU's oid header.
 * @param id The PSU ID.
 * @param[out] info Receives the header.
 */
static int update_psui_status(int local_id, onlp_psu_info_t* info)
{
    int psu_presence = 0, psu_pwgood = 0;

    /* clear psui status */
    info->status = 0;

    /* get psu present status */
    psu_presence = get_psu_present_status(local_id);
    if (psu_presence == 0) {
        info->status |= ONLP_PSU_STATUS_UNPLUGGED;
    } else if (psu_presence == 1) {
        info->status |= ONLP_PSU_STATUS_PRESENT;
    } else {
        return ONLP_STATUS_E_INTERNAL;
    }

    /* get psu power good status */
    psu_pwgood = get_psu_pwgood_status(local_id);
    if (psu_pwgood == 0) {
        info->status |= ONLP_PSU_STATUS_FAILED;
    } else if (psu_pwgood == 1) {
        info->status &= ~ONLP_PSU_STATUS_FAILED;
    } else {
        return ONLP_STATUS_E_INTERNAL;
    }

    return ONLP_STATUS_OK;
}


/**
 * @brief Update the information of Model and Serial from PSU EEPROM
 * @param id The PSU Local ID
 * @param[out] info Receives the PSU information (model and serial).
 */
static int update_psui_fru_info(int local_id, onlp_psu_info_t* info)
{
    int ret = ONLP_STATUS_OK;
    uint8_t data[256];
    int data_len, i;

    memset(data, 0, sizeof(data));

    if (local_id == ONLP_PSU_1) {
        ret = onlp_file_read(data, sizeof(data), &data_len, "/sys/bus/i2c/devices/8-0050/eeprom");
    } else if (local_id == ONLP_PSU_2) {
        ret = onlp_file_read(data, sizeof(data), &data_len, "/sys/bus/i2c/devices/9-0050/eeprom");
    } else {
        AIM_LOG_ERROR("unknown PSU id (%d), func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_PARAM;
    }

    if (ret == ONLP_STATUS_OK) {
        i = 11;

        /* Manufacturer Name */
        data_len = (data[i] & 0x0f);
        i++;
        i += data_len;

        /* Product Name */
        data_len = (data[i] & 0x0f);
        i++;
        memcpy(info->model, (char *) &(data[i]), data_len);
        i += data_len;

        /* Product part,model number */
        data_len = (data[i] & 0x0f);
        i++;
        i += data_len;

        /* Product Version */
        data_len = (data[i] & 0x0f);
        i++;
        i += data_len;

        /* Product Serial Number */
        data_len = (data[i] & 0x0f);
        i++;
        memcpy(info->serial, (char *) &(data[i]), data_len);
    } else {
        strcpy(info->model, "Missing");
        strcpy(info->serial, "Missing");
    }

    return ONLP_STATUS_OK;
}


/**
 * @brief Update the information of PSU IOUT
 * @param id The PSU Local ID
 * @param[out] info Receives the PSU information (iout).
 */
static int update_psui_iout_info(int local_id, onlp_psu_info_t* info)
{
    int psu_reg_value = 0;
    unsigned int y_value = 0;
    unsigned char n_value = 0;
    unsigned int temp = 0;
    char result[32];
    double dvalue;

    memset(result, 0, sizeof(result));

    if (local_id == ONLP_PSU_1) {
        psu_reg_value = onlp_i2c_readw(8, 0x58, 0x8C, ONLP_I2C_F_FORCE);
    } else if (local_id == ONLP_PSU_2) {
        psu_reg_value = onlp_i2c_readw(9, 0x58, 0x8C, ONLP_I2C_F_FORCE);
    } else {
        AIM_LOG_ERROR("unknown PSU id (%d), func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_PARAM;
    }

    if (psu_reg_value < 0) {
        AIM_LOG_ERROR("PSU register value is less than 0 (%d)\n", psu_reg_value);
        return ONLP_STATUS_E_INTERNAL;
    }

    y_value = (psu_reg_value & 0x07FF);
    if ((psu_reg_value & 0x8000) && (y_value)) {
        n_value = 0xF0 + (((psu_reg_value) >> 11) & 0x0F);
        n_value = (~n_value) + 1;
        temp = (unsigned int)(1 << n_value);
        if (temp) {
            snprintf(result, sizeof(result), "%d.%04d", y_value/temp, ((y_value%temp)*10000)/temp);
        }
    } else {
        n_value = (((psu_reg_value) >> 11) & 0x0F);
        snprintf(result, sizeof(result), "%d", (y_value * (1 << n_value)));
    }

    dvalue = atof((const char *)result);
    if (dvalue > 0.0) {
        info->miout = (int)(dvalue * 1000);
    }

    return ONLP_STATUS_OK;
}


/**
 * @brief Update the information of PSU IIN
 * @param id The PSU Local ID
 * @param[out] info Receives the PSU information (iin).
 */
static int update_psui_iin_info(int local_id, onlp_psu_info_t* info)
{
    int psu_reg_value = 0;
    unsigned int y_value = 0;
    unsigned char n_value = 0;
    unsigned int temp = 0;
    char result[32];
    double dvalue;

    memset(result, 0, sizeof(result));

    if (local_id == ONLP_PSU_1) {
        psu_reg_value = onlp_i2c_readw(8, 0x58, 0x89, ONLP_I2C_F_FORCE);
    } else if (local_id == ONLP_PSU_2) {
        psu_reg_value = onlp_i2c_readw(9, 0x58, 0x89, ONLP_I2C_F_FORCE);
    } else {
        AIM_LOG_ERROR("unknown PSU id (%d), func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_PARAM;
    }

    if (psu_reg_value < 0) {
        AIM_LOG_ERROR("PSU register value is less than 0 (%d)\n", psu_reg_value);
        return ONLP_STATUS_E_INTERNAL;
    }

    y_value = (psu_reg_value & 0x07FF);
    if ((psu_reg_value & 0x8000) && (y_value)) {
        n_value = 0xF0 + (((psu_reg_value) >> 11) & 0x0F);
        n_value = (~n_value) + 1;
        temp = (unsigned int)(1 << n_value);
        if (temp) {
            snprintf(result, sizeof(result), "%d.%04d", y_value/temp, ((y_value%temp)*10000)/temp);
        }
    } else {
        n_value = (((psu_reg_value) >> 11) & 0x0F);
        snprintf(result, sizeof(result), "%d", (y_value * (1 << n_value)));
    }

    dvalue = atof((const char *)result);
    if (dvalue > 0.0) {
        info->miin = (int)(dvalue * 1000);
    }

    return ONLP_STATUS_OK;
}


/**
 * @brief Update the information of PSU POUT
 * @param id The PSU Local ID
 * @param[out] info Receives the PSU information (pout).
 */
static int update_psui_pout_info(int local_id, onlp_psu_info_t* info)
{
    int psu_reg_value = 0;
    unsigned int y_value = 0;
    unsigned char n_value = 0;
    unsigned int temp = 0;
    char result[32];
    double dvalue;

    memset(result, 0, sizeof(result));

    if (local_id == ONLP_PSU_1) {
        psu_reg_value = onlp_i2c_readw(8, 0x58, 0x96, ONLP_I2C_F_FORCE);
    } else if (local_id == ONLP_PSU_2) {
        psu_reg_value = onlp_i2c_readw(9, 0x58, 0x96, ONLP_I2C_F_FORCE);
    } else {
        AIM_LOG_ERROR("unknown PSU id (%d), func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_PARAM;
    }

    if (psu_reg_value < 0) {
        AIM_LOG_ERROR("PSU register value is less than 0 (%d)\n", psu_reg_value);
        return ONLP_STATUS_E_INTERNAL;
    }

    y_value = (psu_reg_value & 0x07FF);
    if ((psu_reg_value & 0x8000) && (y_value)) {
        n_value = 0xF0 + (((psu_reg_value) >> 11) & 0x0F);
        n_value = (~n_value) + 1;
        temp = (unsigned int)(1 << n_value);
        if (temp) {
            snprintf(result, sizeof(result), "%d.%04d", y_value/temp, ((y_value%temp)*10000)/temp);
        }
    } else {
        n_value = (((psu_reg_value) >> 11) & 0x0F);
        snprintf(result, sizeof(result), "%d", (y_value * (1 << n_value)));
    }

    dvalue = atof((const char *)result);
    if (dvalue > 0.0) {
        info->mpout = (int)(dvalue * 1000);
    }

    return ONLP_STATUS_OK;
}


/**
 * @brief Update the information of PSU PIN
 * @param id The PSU Local ID
 * @param[out] info Receives the PSU information (pin).
 */
static int update_psui_pin_info(int local_id, onlp_psu_info_t* info)
{
    int psu_reg_value = 0;
    unsigned int y_value = 0;
    unsigned char n_value = 0;
    unsigned int temp = 0;
    char result[32];
    double dvalue;

    memset(result, 0, sizeof(result));

    if (local_id == ONLP_PSU_1) {
        psu_reg_value = onlp_i2c_readw(8, 0x58, 0x97, ONLP_I2C_F_FORCE);
    } else if (local_id == ONLP_PSU_2) {
        psu_reg_value = onlp_i2c_readw(9, 0x58, 0x97, ONLP_I2C_F_FORCE);
    } else {
        AIM_LOG_ERROR("unknown PSU id (%d), func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_PARAM;
    }

    if (psu_reg_value < 0) {
        AIM_LOG_ERROR("PSU register value is less than 0 (%d)\n", psu_reg_value);
        return ONLP_STATUS_E_INTERNAL;
    }

    y_value = (psu_reg_value & 0x07FF);
    if ((psu_reg_value & 0x8000) && (y_value)) {
        n_value = 0xF0 + (((psu_reg_value) >> 11) & 0x0F);
        n_value = (~n_value) + 1;
        temp = (unsigned int)(1 << n_value);
        if (temp) {
            snprintf(result, sizeof(result), "%d.%04d", y_value/temp, ((y_value%temp)*10000)/temp);
        }
    } else {
        n_value = (((psu_reg_value) >> 11) & 0x0F);
        snprintf(result, sizeof(result), "%d", (y_value * (1 << n_value)));
    }

    dvalue = atof((const char *)result);
    if (dvalue > 0.0) {
        info->mpin = (int)(dvalue * 1000);
    }

    return ONLP_STATUS_OK;
}



/**
 * @brief Update the information of PSU VOUT
 * @param id The PSU Local ID
 * @param[out] info Receives the PSU information (vout).
 */
static int update_psui_vout_info(int local_id, onlp_psu_info_t* info)
{
    int v_value = 0;
    int n_value = 0;
    unsigned int temp = 0;
    char result[32];
    double dvalue;

    memset(result, 0, sizeof(result));

    if (local_id == ONLP_PSU_1) {
        n_value = onlp_i2c_readb(8, 0x58, 0x20, ONLP_I2C_F_FORCE);
        v_value = onlp_i2c_readw(8, 0x58, 0x8B, ONLP_I2C_F_FORCE);
    } else if (local_id == ONLP_PSU_2) {
        n_value = onlp_i2c_readb(9, 0x58, 0x20, ONLP_I2C_F_FORCE);
        v_value = onlp_i2c_readw(9, 0x58, 0x8B, ONLP_I2C_F_FORCE);
    } else {
        AIM_LOG_ERROR("unknown PSU id (%d), func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_PARAM;
    }

    if (n_value < 0 || v_value < 0) {
        AIM_LOG_ERROR("PSU register value is less than 0 (%d, %d)\n", n_value, v_value);
        return ONLP_STATUS_E_INTERNAL;
    }

    if (n_value & 0x10) {
        n_value = 0xF0 + (n_value & 0x0F);
        n_value = (~n_value) + 1;
        temp = (unsigned int)(1 << n_value);
        if (temp) {
            snprintf(result, sizeof(result), "%d.%04d", v_value/temp, ((v_value%temp)*10000)/temp);
        }
    } else {
        snprintf(result, sizeof(result), "%d", (v_value * (1 << n_value)));
    }

    dvalue = atof((const char *)result);
    if (dvalue > 0.0) {
        info->mvout = (int)(dvalue * 1000);
    }

    return ONLP_STATUS_OK;
}

/**
 * @brief Update the information of PSU VIN
 * @param id The PSU Local ID
 * @param[out] info Receives the PSU information (vin).
 */
static int update_psui_vin_info(int local_id, onlp_psu_info_t* info)
{
    int psu_reg_value = 0;
    unsigned int y_value = 0;
    unsigned char n_value = 0;
    unsigned int temp = 0;
    char result[32];
    double dvalue;

    memset(result, 0, sizeof(result));

    if (local_id == ONLP_PSU_1) {
        psu_reg_value = onlp_i2c_readw(8, 0x58, 0x88, ONLP_I2C_F_FORCE);
    } else if (local_id == ONLP_PSU_2) {
        psu_reg_value = onlp_i2c_readw(9, 0x58, 0x88, ONLP_I2C_F_FORCE);
    } else {
        AIM_LOG_ERROR("unknown PSU id (%d), func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_PARAM;
    }

    if (psu_reg_value < 0) {
        AIM_LOG_ERROR("PSU register value is less than 0 (%d)\n", psu_reg_value);
        return ONLP_STATUS_E_INTERNAL;
    }

    y_value = (psu_reg_value & 0x07FF);
    if ((psu_reg_value & 0x8000) && (y_value)) {
        n_value = 0xF0 + (((psu_reg_value) >> 11) & 0x0F);
        n_value = (~n_value) + 1;
        temp = (unsigned int)(1 << n_value);
        if (temp) {
            snprintf(result, sizeof(result), "%d.%04d", y_value/temp, ((y_value%temp)*10000)/temp);
        }
    } else {
        n_value = (((psu_reg_value) >> 11) & 0x0F);
        snprintf(result, sizeof(result), "%d", (y_value * (1 << n_value)));
    }

    dvalue = atof((const char *)result);
    if (dvalue > 0.0) {
        info->mvin = (int)(dvalue * 1000);
    }

    return ONLP_STATUS_OK;
}


/**
 * @brief Update the information structure for the given PSU
 * @param id The PSU Local ID
 * @param[out] info Receives the PSU information.
 */
static int update_psui_info(int local_id, onlp_psu_info_t* info)
{
    int ret = ONLP_STATUS_OK;

    if ((info->status & ONLP_PSU_STATUS_PRESENT) == 0) {
        //not present, do nothing
        return ONLP_STATUS_OK;
    }

    //update model and serial
    ret = update_psui_fru_info(local_id, info);
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }

    //update iout value (ONLP_PSU_CAPS_GET_IOUT)
    ret = update_psui_iout_info(local_id, info);
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }

    //update iin value (ONLP_PSU_CAPS_GET_IIN)
    ret = update_psui_iin_info(local_id, info);
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }

    //update pout value (ONLP_PSU_CAPS_GET_POUT)
    ret = update_psui_pout_info(local_id, info);
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }

    //update pin value (ONLP_PSU_CAPS_GET_PIN)
    ret = update_psui_pin_info(local_id, info);
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }

    //update vout value (ONLP_PSU_CAPS_GET_VOUT)
    ret = update_psui_vout_info(local_id, info);
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }

    //update vin value (ONLP_PSU_CAPS_GET_VIN)
    ret = update_psui_vin_info(local_id, info);
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }

    return ret;
}


/**
 * @brief Initialize the PSU subsystem.
 */
int onlp_psui_init(void)
{
    return ONLP_STATUS_OK;
}

/**
 * @brief Get the information structure for the given PSU
 * @param id The PSU OID
 * @param info [out] Receives the PSU information.
 */
int onlp_psui_info_get(onlp_oid_t id, onlp_psu_info_t* info)
{
    int ret = ONLP_STATUS_OK;
    int local_id = ONLP_OID_ID_GET(id);

    /* Set the onlp_psu_info_t */
    memset(info, 0, sizeof(onlp_psu_info_t));
    *info = __onlp_psu_info[local_id];
    ONLP_TRY(onlp_psui_hdr_get(id, &info->hdr));

    /* Update onlp_psu_info_t status */
    ONLP_TRY(update_psui_status(local_id, info));

    if (local_id == ONLP_PSU_1 || ONLP_PSU_2) {
        ret = update_psui_info(local_id, info);
    } else {
        return ONLP_STATUS_E_PARAM;
    }

    return ret;
}

/**
 * @brief Get the PSU's operational status.
 * @param id The PSU OID.
 * @param rv [out] Receives the operational status.
 */
int onlp_psui_status_get(onlp_oid_t id, uint32_t* status)
{
    int local_id = ONLP_OID_ID_GET(id);
    int psu_presence = 0, psu_pwgood = 0;

    /* clear psui status */
    *status = 0;

    /* get psu present status */
    psu_presence = get_psu_present_status(local_id);
    if (psu_presence == 0) {
        *status |= ONLP_PSU_STATUS_UNPLUGGED;
    } else if (psu_presence == 1) {
        *status |= ONLP_PSU_STATUS_PRESENT;
    } else {
        return ONLP_STATUS_E_INTERNAL;
    }   

    /* get psu power good status */
    psu_pwgood = get_psu_pwgood_status(local_id);
    if (psu_pwgood == 0) {
        *status |= ONLP_PSU_STATUS_FAILED;
    } else if (psu_pwgood == 1) {
        *status &= ~ONLP_PSU_STATUS_FAILED;
    } else {
        return ONLP_STATUS_E_INTERNAL;
    }   

    return ONLP_STATUS_OK;
}

/**
 * @brief Get the PSU's oid header.
 * @param id The PSU OID.
 * @param hdr [out] Receives the header.
 */
int onlp_psui_hdr_get(onlp_oid_t id, onlp_oid_hdr_t* hdr)
{
    int ret = ONLP_STATUS_OK;
    int local_id = ONLP_OID_ID_GET(id);

    /* Set the onlp_psu_info_t */
    *hdr = __onlp_psu_info[local_id].hdr;

    return ret;
}

/**
 * @brief Generic PSU ioctl
 * @param id The PSU OID
 * @param vargs The variable argument list for the ioctl call.
 */
int onlp_psui_ioctl(onlp_oid_t pid, va_list vargs)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

