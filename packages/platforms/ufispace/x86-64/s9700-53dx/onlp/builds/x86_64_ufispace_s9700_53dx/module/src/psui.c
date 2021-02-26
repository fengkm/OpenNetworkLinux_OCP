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
 * PSU
 *
 ***********************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <onlp/platformi/psui.h>
#include <onlplib/file.h>
#include <AIM/aim.h>

#include "platform_lib.h"

#define CMD_FRU_INFO_GET        "ipmitool fru print %d | grep '%s' | cut -d':' -f2 | awk '{$1=$1};1' | tr -d '\n'"


/**
 * Get all information about the given PSU oid.
 *
 *            |----[01] ONLP_PSU_0----[07] ONLP_THERMAL_PSU0
 *            |                  |----[05] ONLP_PSU0_FAN_1
 *            |                  |----[06] ONLP_PSU0_FAN_2
 *            |
 *            |----[02] ONLP_PSU_1----[08] ONLP_THERMAL_PSU1
 *            |                  |----[07] ONLP_PSU1_FAN_1
 *            |                  |----[08] ONLP_PSU1_FAN_2
 */
static onlp_psu_info_t __onlp_psu_info[ONLP_PSU_COUNT] = {
    { }, /* Not used */
    {
        .hdr = {
            .id = ONLP_PSU_ID_CREATE(ONLP_PSU_0),
            .description = "PSU-0",
            .poid = ONLP_OID_CHASSIS,
            .coids = {
                ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_PSU0),
                ONLP_FAN_ID_CREATE(ONLP_PSU0_FAN_1),
                ONLP_FAN_ID_CREATE(ONLP_PSU0_FAN_2),
            },
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },
        .model = "",
        .serial = "",
        .caps = ONLP_PSU_CAPS_GET_VIN|ONLP_PSU_CAPS_GET_VOUT|ONLP_PSU_CAPS_GET_IIN|ONLP_PSU_CAPS_GET_IOUT|ONLP_PSU_CAPS_GET_PIN|ONLP_PSU_CAPS_GET_POUT,
        .type = ONLP_PSU_TYPE_AC,
    },
    {
        .hdr = {
            .id = ONLP_PSU_ID_CREATE(ONLP_PSU_1),
            .description = "PSU-1",
            .poid = ONLP_OID_CHASSIS,
            .coids = {
                ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_PSU1),
                ONLP_FAN_ID_CREATE(ONLP_PSU1_FAN_1),
                ONLP_FAN_ID_CREATE(ONLP_PSU1_FAN_2),
            },
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },
        .model = "",
        .serial = "",
        .caps = ONLP_PSU_CAPS_GET_VIN|ONLP_PSU_CAPS_GET_VOUT|ONLP_PSU_CAPS_GET_IIN|ONLP_PSU_CAPS_GET_IOUT|ONLP_PSU_CAPS_GET_PIN|ONLP_PSU_CAPS_GET_POUT,
        .type = ONLP_PSU_TYPE_AC,
    }
};

/**
 * @brief get psu pwgood status
 * @param local_id: psu id
 * @return:
 *    0: absence
 *    1: presence
 *   <0: error code
 */
static int get_psu_pwgood_status(int local_id)
{
    int psu_reg_value = 0;
    int psu_pwgood = 0;

    if (local_id == ONLP_PSU_0) {
        ONLP_TRY(file_read_hex(&psu_reg_value, "/sys/bus/i2c/devices/1-0030/cpld_psu_status_0"));
        psu_pwgood = (psu_reg_value & 0b00010000) ? 1 : 0;
    } else if (local_id == ONLP_PSU_1) {
        ONLP_TRY(file_read_hex(&psu_reg_value, "/sys/bus/i2c/devices/1-0030/cpld_psu_status_1"));
        psu_pwgood = (psu_reg_value & 0b00100000) ? 1 : 0;
    } else {
        AIM_LOG_ERROR("unknown psu id (%d), func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_PARAM;
    }   

    return psu_pwgood;
}

/**
 * @brief Update the status of PSU's oid header.
 * @param id The PSU ID.
 * @param[out] hdr Receives the header.
 */
static int update_psui_status(int local_id, onlp_oid_hdr_t* hdr) {
    int psu_presence = 0, psu_pwgood = 0;

    /* clear psui status */
    hdr->status = 0;

    /* get psu present status */
    psu_presence = get_psu_present_status(local_id);
    if (psu_presence == 0) {
        ONLP_OID_STATUS_FLAG_SET(hdr, UNPLUGGED);
    } else if (psu_presence == 1) {
        ONLP_OID_STATUS_FLAG_SET(hdr, PRESENT);
    } else {
        return ONLP_STATUS_E_INTERNAL;
    }   

    /* get psu power good status */
    psu_pwgood = get_psu_pwgood_status(local_id);
    if (psu_pwgood == 0) {
        ONLP_OID_STATUS_FLAG_SET(hdr, FAILED);
    } else if (psu_pwgood == 1) {
        ONLP_OID_STATUS_FLAG_SET(hdr, OPERATIONAL);
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
    char cmd[100];
    char cmd_out[150];
    char fru_model[] = "Product Name";  //only Product Name can identify AC/DC
    char fru_serial[] = "Product Serial";

    //FRU (model)
    
    memset(cmd, 0, sizeof(cmd));
    memset(cmd_out, 0, sizeof(cmd_out));
    memset(info->model, 0, sizeof(info->model));
    
    snprintf(cmd, sizeof(cmd), CMD_FRU_INFO_GET, local_id, fru_model);
    
    //Get psu fru info (model) from BMC 
    if (exec_cmd(cmd, cmd_out, sizeof(cmd_out)) < 0) { 
        AIM_LOG_ERROR("unable to read fru info from BMC, fru id=%d, cmd=%s, out=%s\n", local_id, cmd, cmd_out);
        return ONLP_STATUS_E_INTERNAL; 
    }    

    //Check output is correct    
    if (strnlen(cmd_out, sizeof(cmd_out)) == 0){
        AIM_LOG_ERROR("unable to read fru info from BMC, fru id=%d, cmd=%s, out=%s\n", local_id, cmd, cmd_out);
        return ONLP_STATUS_E_INTERNAL; 
    }    
    
    snprintf(info->model, sizeof(info->model), "%s", cmd_out);

    //FRU (serial) 
    memset(cmd, 0, sizeof(cmd));
    memset(cmd_out, 0, sizeof(cmd_out));
    memset(info->serial, 0, sizeof(info->serial));
    
    snprintf(cmd, sizeof(cmd), CMD_FRU_INFO_GET, local_id, fru_serial);
    
    //Get psu fru info (model) from BMC
    if (exec_cmd(cmd, cmd_out, sizeof(cmd_out)) < 0) {
        AIM_LOG_ERROR("unable to read fru info from BMC, fru id=%d, cmd=%s, out=%s\n", local_id, cmd, cmd_out);
        return ONLP_STATUS_E_INTERNAL;
    }

    //Check output is correct
    if (strnlen(cmd_out, sizeof(cmd_out)) == 0) {
        AIM_LOG_ERROR("unable to read fru info from BMC, fru id=%d, cmd=%s, out=%s\n", local_id, cmd, cmd_out);
        return ONLP_STATUS_E_INTERNAL;
    }
    
    snprintf(info->serial, sizeof(info->model), "%s", cmd_out);
    
    return ONLP_STATUS_OK;
}

/**
 * @brief Update the information of PSU Type from PSU Model (AC/DC)
 * @param id The PSU Local ID
 * @param[out] info Receives the PSU information (psu type).
 */
static int update_psui_psu_type(int local_id, onlp_psu_info_t* info)
{
    int ret = ONLP_STATUS_OK;

    if (strstr(info->model, "AM") > 0) {
        info->type = ONLP_PSU_TYPE_AC;
    } else if (strstr(info->model, "EM") > 0) {
        info->type = ONLP_PSU_TYPE_DC48;
    } else {
        AIM_LOG_ERROR("unknown PSU type, model=%s, func=%s\n", info->model, __FUNCTION__);
    }

    return ret;
}

/**
 * @brief Update the information structure for the given PSU
 * @param id The PSU Local ID
 * @param[out] info Receives the PSU information.
 */
static int update_psui_info(int local_id, onlp_psu_info_t* info)
{
    int ret = ONLP_STATUS_OK;
    int stbmvout, stbmiout;
    float data;
    int index_offset = 0;
    //int pw_present;

    if ((info->hdr.status & ONLP_OID_STATUS_FLAG_PRESENT) == 0) {
        //not present, do nothing
        return ONLP_STATUS_OK;
    }

    if (local_id == ONLP_PSU_0) {
        index_offset = 0;
    } else if (local_id == ONLP_PSU_1) {
        index_offset = 5;
    } else {
        AIM_LOG_ERROR("unknown PSU_ID (%d), func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_INTERNAL;
    }

    /* Get power vin status */
    ret = bmc_sensor_read(local_id + 19 + index_offset, PSU_SENSOR, &data);
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    } else {
        info->mvin = (int) (data*1000);
        //info->caps |= ONLP_PSU_CAPS_GET_VIN;
    }

    /* Get power vout status */
    ret = bmc_sensor_read(local_id + 20 + index_offset, PSU_SENSOR, &data);
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    } else {
        info->mvout = (int) (data*1000);
        //info->caps |= ONLP_PSU_CAPS_GET_VOUT;
    }

    /* Get power iin status */
    ret = bmc_sensor_read(local_id + 21 + index_offset, PSU_SENSOR, &data);
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    } else {
        info->miin = (int) (data*1000);
        //info->caps |= ONLP_PSU_CAPS_GET_IIN;
    }

    /* Get power iout status */
    ret = bmc_sensor_read(local_id + 22 + index_offset, PSU_SENSOR, &data);
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    } else {
        info->miout = (int) (data*1000);
        //info->caps |= ONLP_PSU_CAPS_GET_IOUT;
    }

    /* Get standby power vout */
    ret = bmc_sensor_read(local_id + 23 + index_offset, PSU_SENSOR, &data);
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    } else {
        stbmvout = (int) (data*1000);
    }

    /* Get standby power iout */
    ret = bmc_sensor_read(local_id + 24 + index_offset, PSU_SENSOR, &data);
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    } else {
        stbmiout = (int) (data*1000);
    }

    /* Get power in and out */
    info->mpin = info->miin * info->mvin / 1000;
    info->mpout = (info->miout * info->mvout + stbmiout * stbmvout) / 1000;
    //info->caps |= ONLP_PSU_CAPS_GET_PIN | ONLP_PSU_CAPS_GET_POUT;

    /* Update FRU (model/serial) */
    ret = update_psui_fru_info(local_id, info);
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }

    /* Update PSU Type AC/DC */
    ret = update_psui_psu_type(local_id, info);
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }

    return ONLP_STATUS_OK;
}

/**
 * @brief Software initialization of the PSU module.
 */
int onlp_psui_sw_init(void)
{
    return ONLP_STATUS_OK;
}

/**
 * @brief Hardware initialization of the PSU module.
 * @param flags The hardware initialization flags.
 */
int onlp_psui_hw_init(uint32_t flags)
{
    return ONLP_STATUS_OK;
}

/**
 * @brief Deinitialize the psu software module.
 * @note The primary purpose of this API is to properly
 * deallocate any resources used by the module in order
 * faciliate detection of real resouce leaks.
 */
int onlp_psui_sw_denit(void)
{
    return ONLP_STATUS_OK;
}

/**
 * @brief Validate a PSU OID.
 * @param id The id.
 */
int onlp_psui_id_validate(onlp_oid_id_t id)
{
    return ONLP_OID_ID_VALIDATE_RANGE(id, 1, ONLP_PSU_MAX-1);
}

/**
 * @brief Get the PSU's oid header.
 * @param id The PSU OID.
 * @param[out] rv Receives the header.
 */
int onlp_psui_hdr_get(onlp_oid_t id, onlp_oid_hdr_t* hdr)
{
    int ret = ONLP_STATUS_OK;
    int local_id = ONLP_OID_ID_GET(id);

    /* Set the onlp_psu_info_t */
    *hdr = __onlp_psu_info[local_id].hdr;

    /* Update onlp_oid_hdr_t */
    ret = update_psui_status(local_id, hdr);

    return ret;
}

/**
 * @brief Get the information structure for the given PSU
 * @param id The PSU OID
 * @param[out] info Receives the PSU information.
 */
int onlp_psui_info_get(onlp_oid_id_t id, onlp_psu_info_t* info)
{
    int ret = ONLP_STATUS_OK;
    int local_id = ONLP_OID_ID_GET(id);

    /* Set the onlp_psu_info_t */
    memset(info, 0, sizeof(onlp_psu_info_t));
    *info = __onlp_psu_info[local_id];
    ONLP_TRY(onlp_psui_hdr_get(id, &info->hdr));

    if (local_id == ONLP_PSU_0 || ONLP_PSU_1) {
        ret = update_psui_info(local_id, info);
    } else {
        return ONLP_STATUS_E_PARAM;
    }   

    return ret;
}
