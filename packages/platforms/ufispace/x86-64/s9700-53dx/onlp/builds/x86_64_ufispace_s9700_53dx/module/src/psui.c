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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <onlp/platformi/psui.h>
#include <onlplib/file.h>
#include <AIM/aim.h>

#include "platform_lib.h"

#define PSU_STATUS_PRESENT      1
#define PSU_STATUS_POWER_GOOD   1

#define CMD_FRU_INFO_GET        "ipmitool fru print %d | grep '%s' | cut -d':' -f2 | awk '{$1=$1};1' | tr -d '\n'"


/*
 * Get all information about the given PSU oid.
 */
static onlp_psu_info_t __onlp_psu_info[ONLP_PSU_COUNT] = {
    { }, /* Not used */
    {
        .hdr = {
            .id = ONLP_PSU_ID_CREATE(ONLP_PSU_0),
            .description = "PSU-1",
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
            .description = "PSU-2",
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

static int ufi_psui_present_get(int local_id, int *pw_present)
{
    int ret = ONLP_STATUS_OK;
    int status;
    int psu_index;
    int mask;

    if (local_id == ONLP_PSU_0) {
        mask = 0b01000000; //0x40
        psu_index = 0;
    } else if (local_id == ONLP_PSU_1) {
        mask = 0b10000000; //0x80
        psu_index = 1;
    } else {
        return ONLP_STATUS_E_INTERNAL;
    }    
    
    ret = file_read_hex(&status, "/sys/bus/i2c/devices/1-0030/cpld_psu_status_%d", psu_index);
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }

    *pw_present = ((status & mask)? 0 : 1);

    return ONLP_STATUS_OK;
}

static int ufi_psui_pwgood_get(int local_id, int *pw_good)
{
    int ret = ONLP_STATUS_OK;
    int status;
    int psu_index;
    int mask;

    if (local_id == ONLP_PSU_0) {
        mask = 0b00010000; //0x10
        psu_index=0;
    } else if (local_id == ONLP_PSU_1) {
        mask = 0b00100000; //0x20
        psu_index=1;
    } else {
        return ONLP_STATUS_E_INTERNAL;
    }

    ret = file_read_hex(&status, "/sys/bus/i2c/devices/1-0030/cpld_psu_status_%d", psu_index);
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }

    *pw_good = ((status & mask)? 1 : 0);

    return ONLP_STATUS_OK;
}

static int ufi_psui_status_get(int local_id, onlp_oid_hdr_t* hdr)
{
    int ret = ONLP_STATUS_OK;
    int pw_present, pw_good;

    /* Get power present status */
    ret = ufi_psui_present_get(local_id, &pw_present);
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    } 

    hdr->status = 0;
    if (pw_present != PSU_STATUS_PRESENT) {
        //hdr->status &= ~ONLP_OID_STATUS_FLAG_PRESENT;
        //hdr->status |=  ONLP_OID_STATUS_FLAG_UNPLUGGED;
        ONLP_OID_STATUS_FLAG_SET(hdr, UNPLUGGED);
        return ONLP_STATUS_OK;
    }

    //hdr->status |= ONLP_OID_STATUS_FLAG_PRESENT;
    ONLP_OID_STATUS_FLAG_SET(hdr, PRESENT);

    /* Get power good status */
    ret = ufi_psui_pwgood_get(local_id, &pw_good);
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }                                 

    if (pw_good != PSU_STATUS_POWER_GOOD) {
        //hdr->status |= ONLP_OID_STATUS_FLAG_FAILED;
        ONLP_OID_STATUS_FLAG_SET(hdr, FAILED);
    } else {
        //hdr->status &= ~ONLP_OID_STATUS_FLAG_FAILED;
        ONLP_OID_STATUS_FLAG_SET(hdr, OPERATIONAL);
    }

    return ONLP_STATUS_OK;
}

static int ufi_psui_fru_get(int local_id, onlp_psu_info_t* info)
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

static int ufi_psui_psu_type_get(int local_id, onlp_psu_info_t* info)
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

static int ufi_psui_info_get(int local_id, onlp_psu_info_t* info)
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

    /* Get FRU (model/serial) */
    ret = ufi_psui_fru_get(local_id, info);
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }

    /* Update PSU Type AC/DC */
    ret = ufi_psui_psu_type_get(local_id, info);
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
    ret = ufi_psui_status_get(local_id, hdr);

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

    switch (local_id) {
        case ONLP_PSU_0:
        case ONLP_PSU_1:
            ret = ufi_psui_info_get(local_id, info);
            break;
        default:
            return ONLP_STATUS_E_INTERNAL;
            break;
    }   

    return ret;
}
