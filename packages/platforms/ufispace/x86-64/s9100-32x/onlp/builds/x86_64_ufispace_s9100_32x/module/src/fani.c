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
 * Fan Platform Implementation Defaults.
 *
 ***********************************************************/
#include <onlp/platformi/fani.h>
#include <onlplib/file.h>
//#include <fcntl.h>
//#include <unistd.h>
#include "platform_lib.h"


/**
 * Get all information about the given FAN oid.
 *
 * [01] CHASSIS----[01] ONLP_FAN_1----[05] ONLP_LED_FAN_TRAY1
 *            |----[02] ONLP_FAN_2----[05] ONLP_LED_FAN_TRAY1
 *            |----[03] ONLP_FAN_3----[06] ONLP_LED_FAN_TRAY2
 *            |----[04] ONLP_FAN_4----[06] ONLP_LED_FAN_TRAY2
 *            |----[05] ONLP_FAN_5----[07] ONLP_LED_FAN_TRAY3
 *            |----[06] ONLP_FAN_6----[07] ONLP_LED_FAN_TRAY3
 *            |----[07] ONLP_FAN_7----[08] ONLP_LED_FAN_TRAY4
 *            |----[08] ONLP_FAN_8----[08] ONLP_LED_FAN_TRAY4
 *            |
 *            |----[01] ONLP_PSU_1----[09] ONLP_PSU1_FAN_1
 *            |----[02] ONLP_PSU_2----[10] ONLP_PSU2_FAN_1
 */
static onlp_fan_info_t __onlp_fan_info[ONLP_FAN_COUNT] = {
    { }, /* Not used */
    {
        .hdr = {
            .id = ONLP_FAN_ID_CREATE(ONLP_FAN_1),
            .description = "Chassis FANTRAY 1-A",
            .poid = ONLP_OID_CHASSIS,
            .coids = { ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY1) },
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },
        .dir = ONLP_FAN_DIR_F2B,
        .caps = (ONLP_FAN_CAPS_GET_DIR | ONLP_FAN_CAPS_GET_PERCENTAGE | ONLP_FAN_CAPS_GET_RPM | ONLP_FAN_CAPS_SET_PERCENTAGE),
        .model = "",
        .serial = "",
    },
    {
        .hdr = {
            .id = ONLP_FAN_ID_CREATE(ONLP_FAN_2),
            .description = "Chassis FANTRAY 1-B",
            .poid = ONLP_OID_CHASSIS,
            .coids = { ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY1) },
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },
        .dir = ONLP_FAN_DIR_F2B,
        .caps = (ONLP_FAN_CAPS_GET_DIR | ONLP_FAN_CAPS_GET_PERCENTAGE | ONLP_FAN_CAPS_GET_RPM | ONLP_FAN_CAPS_SET_PERCENTAGE),
        .model = "",
        .serial = "",
    },
    {
        .hdr = {
            .id = ONLP_FAN_ID_CREATE(ONLP_FAN_3),
            .description = "Chassis FANTRAY 2-A",
            .poid = ONLP_OID_CHASSIS,
            .coids = { ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY2) },
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },
        .dir = ONLP_FAN_DIR_F2B,
        .caps = (ONLP_FAN_CAPS_GET_DIR | ONLP_FAN_CAPS_GET_PERCENTAGE | ONLP_FAN_CAPS_GET_RPM | ONLP_FAN_CAPS_SET_PERCENTAGE),
        .model = "",
        .serial = "",
    },
    {
        .hdr = {
            .id = ONLP_FAN_ID_CREATE(ONLP_FAN_4),
            .description = "Chassis FANTRAY 2-B",
            .poid = ONLP_OID_CHASSIS,
            .coids = { ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY2) },
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },
        .dir = ONLP_FAN_DIR_F2B,
        .caps = (ONLP_FAN_CAPS_GET_DIR | ONLP_FAN_CAPS_GET_PERCENTAGE | ONLP_FAN_CAPS_GET_RPM | ONLP_FAN_CAPS_SET_PERCENTAGE),
        .model = "",
        .serial = "",
    },
    {
        .hdr = {
            .id = ONLP_FAN_ID_CREATE(ONLP_FAN_5),
            .description = "Chassis FANTRAY 3-A",
            .poid = ONLP_OID_CHASSIS,
            .coids = { ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY3) },
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },
        .dir = ONLP_FAN_DIR_F2B,
        .caps = (ONLP_FAN_CAPS_GET_DIR | ONLP_FAN_CAPS_GET_PERCENTAGE | ONLP_FAN_CAPS_GET_RPM | ONLP_FAN_CAPS_SET_PERCENTAGE),
        .model = "",
        .serial = "",
    },
    {
        .hdr = {
            .id = ONLP_FAN_ID_CREATE(ONLP_FAN_6),
            .description = "Chassis FANTRAY 3-B",
            .poid = ONLP_OID_CHASSIS,
            .coids = { ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY3) },
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },
        .dir = ONLP_FAN_DIR_F2B,
        .caps = (ONLP_FAN_CAPS_GET_DIR | ONLP_FAN_CAPS_GET_PERCENTAGE | ONLP_FAN_CAPS_GET_RPM | ONLP_FAN_CAPS_SET_PERCENTAGE),
        .model = "",
        .serial = "",
    },
    {
        .hdr = {
            .id = ONLP_FAN_ID_CREATE(ONLP_FAN_7),
            .description = "Chassis FANTRAY 4-A",
            .poid = ONLP_OID_CHASSIS,
            .coids = { ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY4) },
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },
        .dir = ONLP_FAN_DIR_F2B,
        .caps = (ONLP_FAN_CAPS_GET_DIR | ONLP_FAN_CAPS_GET_PERCENTAGE | ONLP_FAN_CAPS_GET_RPM | ONLP_FAN_CAPS_SET_PERCENTAGE),
        .model = "",
        .serial = "",
    },
    {
        .hdr = {
            .id = ONLP_FAN_ID_CREATE(ONLP_FAN_8),
            .description = "Chassis FANTRAY 4-B",
            .poid = ONLP_OID_CHASSIS,
            .coids = { ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY4) },
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },
        .dir = ONLP_FAN_DIR_F2B,
        .caps = (ONLP_FAN_CAPS_GET_DIR | ONLP_FAN_CAPS_GET_PERCENTAGE | ONLP_FAN_CAPS_GET_RPM | ONLP_FAN_CAPS_SET_PERCENTAGE),
        .model = "",
        .serial = "",
    },
    {
        .hdr = {
            .id = ONLP_FAN_ID_CREATE(ONLP_PSU1_FAN_1),
            .description = "PSU-1 FAN",
            .poid = ONLP_PSU_ID_CREATE(ONLP_PSU_1),
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },
        .dir = ONLP_FAN_DIR_F2B,
        .caps = (ONLP_FAN_CAPS_GET_DIR),
        .model = "",
        .serial = "",
    },
    {
        .hdr = {
            .id = ONLP_FAN_ID_CREATE(ONLP_PSU2_FAN_1),
            .description = "PSU-2 FAN",
            .poid = ONLP_PSU_ID_CREATE(ONLP_PSU_2),
            .status = ONLP_OID_STATUS_FLAG_PRESENT,
        },
        .dir = ONLP_FAN_DIR_F2B,
        .caps = (ONLP_FAN_CAPS_GET_DIR),
        .model = "",
        .serial = "",
    },
};

/**
 * @brief Update the status of FAN's oid header.
 * @param id The FAN ID.
 * @param[out] hdr Receives the header.
 */
static int update_fani_status(int local_id, onlp_oid_hdr_t* hdr)
{
    int ret = ONLP_STATUS_OK;
    int fan_presence = 0;

    /* clear fani status */
    hdr->status = 0;

    /* get fan present status */
    fan_presence = get_fan_present_status(local_id);

    if (fan_presence == 0) {
        ONLP_OID_STATUS_FLAG_SET(hdr, UNPLUGGED);
    } else if (fan_presence == 1) {
        ONLP_OID_STATUS_FLAG_SET(hdr, PRESENT);
    } else {
        return ONLP_STATUS_E_INTERNAL;
    }

    return ret;
}

/**
 * @brief Update the information structure for the given FAN
 * @param id The FAN Local ID
 * @param[out] info Receives the FAN information.
 */
static int update_fani_info(int local_id, onlp_fan_info_t* info)
{
    int fan_alarm = 0;
    int fan_rpm = 0;
    int sys_max_fan_speed = 16000;

    if ((info->hdr.status & ONLP_OID_STATUS_FLAG_PRESENT) == 0) {
        //not present, do nothing
        return ONLP_STATUS_OK;
    }

    //only support for ONLP_FAN_1 to ONLP_FAN_8
    if (local_id == ONLP_PSU1_FAN_1 || local_id == ONLP_PSU2_FAN_1) {
        return ONLP_STATUS_OK;
    }

    //get fan alarm
    ONLP_TRY(onlp_file_read_int(&fan_alarm, "/sys/class/hwmon/hwmon1/device/fan%d_alarm", local_id));

    //get and update fan rpm and percentage
    ONLP_TRY(onlp_file_read_int(&fan_rpm, "/sys/class/hwmon/hwmon1/device/fan%d_input", local_id));
    info->rpm = fan_rpm;
    info->percentage = (info->rpm * 100) / sys_max_fan_speed;

    //update fan_status
    info->hdr.status |= (fan_alarm > 0 || fan_rpm == 0) ? ONLP_OID_STATUS_FLAG_FAILED : ONLP_OID_STATUS_FLAG_OPERATIONAL;

    return ONLP_STATUS_OK;
}


/**
 * @brief Software initialization of the Fan module.
 */
int onlp_fani_sw_init(void)
{
    return ONLP_STATUS_OK;
}

/**
 * @brief Hardware initialization of the Fan module.
 * @param flags The hardware initialization flags.
 */
int onlp_fani_hw_init(uint32_t flags)
{
    return ONLP_STATUS_OK;
}

/**
 * @brief Deinitialize the fan software module.
 * @note The primary purpose of this API is to properly
 * deallocate any resources used by the module in order
 * faciliate detection of real resouce leaks.
 */
int onlp_fani_sw_denit(void)
{
    return ONLP_STATUS_OK;
}

/**
 * @brief Validate a fan id.
 * @param id The fan id.
 */
int onlp_fani_id_validate(onlp_oid_id_t id)
{
    return ONLP_OID_ID_VALIDATE_RANGE(id, 1, ONLP_FAN_MAX-1);
}

/**
 * @brief Retrieve the fan's OID hdr.
 * @param id The fan id.
 * @param[out] hdr Receives the OID header.
 */
int onlp_fani_hdr_get(onlp_oid_t id, onlp_oid_hdr_t* hdr)
{
    int ret = ONLP_STATUS_OK;
    int local_id = ONLP_OID_ID_GET(id);

    /* Set the onlp_fan_info_t */
    *hdr = __onlp_fan_info[local_id].hdr;

    /* Update onlp_oid_hdr_t */
    ret = update_fani_status(local_id, hdr);

    return ret;
}

/**
 * @brief Get the information structure for the given fan OID.
 * @param id The fan id
 * @param[out] info Receives the fan information.
 */
int onlp_fani_info_get(onlp_oid_id_t id, onlp_fan_info_t* info)
{
    int ret = ONLP_STATUS_OK;
    int local_id = ONLP_OID_ID_GET(id);

    /* Set the onlp_fan_info_t */
    memset(info, 0, sizeof(onlp_fan_info_t));
    *info = __onlp_fan_info[local_id];
    ONLP_TRY(onlp_fani_hdr_get(id, &info->hdr));

    if (local_id >= ONLP_FAN_1 && local_id <= ONLP_FAN_8) {
        ret = update_fani_info(local_id, info);
    } else if (local_id == ONLP_PSU1_FAN_1 || ONLP_PSU2_FAN_1) {
        ret = ONLP_STATUS_OK;
    } else {
        AIM_LOG_ERROR("unknown FAN id (%d), func=%s\n", local_id, __FUNCTION__);
        ret = ONLP_STATUS_E_PARAM;
    }

    return ret;
}

/**
 * @brief Get the fan capabilities.
 * @param id The fan id.
 * @param[out] rv The fan capabilities
 */
int onlp_fani_caps_get(onlp_oid_id_t id, uint32_t* rv)
{
    int local_id = ONLP_OID_ID_GET(id);

    *rv = __onlp_fan_info[local_id].caps;

    return ONLP_STATUS_OK;
}

/**
 * @brief Set the fan speed in RPM.
 * @param id The fan OID
 * @param rpm The new RPM
 * @note This is only relevant if the RPM capability is set.
 */
int onlp_fani_rpm_set(onlp_oid_id_t id, int rpm)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

/**
 * @brief Set the fan speed in percentage.
 * @param id The fan OID.
 * @param p The new fan speed percentage.
 * @note This is only relevant if the PERCENTAGE capability is set.
 */
int onlp_fani_percentage_set(onlp_oid_id_t id, int p)
{
    int local_id = ONLP_OID_ID_GET(id);
    int pwm = 0; //range from 0-255

    /**
     * Set fan speed 
     * Driver accept value in range between 128 and 255.
     * Value 128 is 50%.
     * Value 200 is 80%.
     * Value 255 is 100%.
    **/
    pwm = (p > 0) ? (p / 100 * 255) : 200;

    /* set the minmux speed */
    if (pwm < 128) {
        pwm = 128;
    }

    if (local_id >= ONLP_FAN_1 && local_id <= ONLP_FAN_4) {
        ONLP_TRY(onlp_file_write_int(pwm, "/sys/class/hwmon/hwmon1/device/pwm1"));
    } else if (local_id >= ONLP_FAN_5 && local_id <= ONLP_FAN_8) {
        ONLP_TRY(onlp_file_write_int(pwm, "/sys/class/hwmon/hwmon1/device/pwm2"));
    } else if (local_id == ONLP_PSU1_FAN_1 || local_id == ONLP_PSU2_FAN_1) {
        return ONLP_STATUS_E_UNSUPPORTED;
    } else {
        return ONLP_STATUS_E_PARAM;
    }

    return ONLP_STATUS_OK;
}

/**
 * @brief Set the fan direction (if supported).
 * @param id The fan OID
 * @param dir The direction.
 */
int onlp_fani_dir_set(onlp_oid_id_t id, onlp_fan_dir_t dir)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}
