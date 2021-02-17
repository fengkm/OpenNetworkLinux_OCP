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
 * Chassis Implementation.
 *
 ***********************************************************/
#include <onlp/platformi/chassisi.h>
#include "platform_lib.h"


/* This is definitions for x86-64-ufispace-s9700-53dx*/
/* OID map*/
/*
 * [01] CHASSIS----[01] ONLP_THERMAL_CPU_PECI
 *            |----[02] ONLP_THERMAL_OP2_ENV
 *            |----[03] ONLP_THERMAL_J2_ENV_1
 *            |----[04] ONLP_THERMAL_J2_DIE_1
 *            |----[05] ONLP_THERMAL_J2_ENV_2
 *            |----[06] ONLP_THERMAL_J2_DIE_2
 *            |----[09] ONLP_THERMAL_CPU_PKG
 *            |----[10] ONLP_THERMAL_CPU1
 *            |----[11] ONLP_THERMAL_CPU2
 *            |----[12] ONLP_THERMAL_CPU3
 *            |----[13] ONLP_THERMAL_CPU4
 *            |----[14] ONLP_THERMAL_CPU5
 *            |----[15] ONLP_THERMAL_CPU6
 *            |----[16] ONLP_THERMAL_CPU7
 *            |----[17] ONLP_THERMAL_CPU8
 *            |----[18] ONLP_THERMAL_CPU_BOARD
 *            |----[01] ONLP_FAN_1
 *            |----[02] ONLP_FAN_2
 *            |----[03] ONLP_FAN_3
 *            |----[04] ONLP_FAN_4
 *            |----[01] ONLP_PSU_0----[07] ONLP_THERMAL_PSU0
 *            |                  |----[05] ONLP_PSU0_FAN_1
 *            |                  |----[06] ONLP_PSU0_FAN_2
 *            |
 *            |----[02] ONLP_PSU_1----[08] ONLP_THERMAL_PSU1
 *            |                  |----[07] ONLP_PSU1_FAN_1
 *            |                  |----[08] ONLP_PSU1_FAN_2
 *            |
 *            |----[01] ONLP_LED_SYSTEM
 *            |----[02] ONLP_LED_PSU0
 *            |----[03] ONLP_LED_PSU1
 *            |----[04] ONLP_LED_FAN
 */
static onlp_oid_t __oid_info[] = {
    ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_CPU_PECI),
    ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_OP2_ENV),
    ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_J2_ENV_1),
    ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_J2_DIE_1),
    ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_J2_ENV_2),
    ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_J2_DIE_2),
    //ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_PSU0),
    //ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_PSU1),
    ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_CPU_PKG),
    ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_CPU1),
    ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_CPU2),
    ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_CPU3),
    ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_CPU4),
    ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_CPU5),
    ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_CPU6),
    ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_CPU7),
    ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_CPU8),
    ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_CPU_BOARD),
    //ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_BMC_ENV),
    //ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_ENV),
    //ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_ENV_FRONT),
    ONLP_FAN_ID_CREATE(ONLP_FAN_1),
    ONLP_FAN_ID_CREATE(ONLP_FAN_2),
    ONLP_FAN_ID_CREATE(ONLP_FAN_3),
    ONLP_FAN_ID_CREATE(ONLP_FAN_4),
    //ONLP_FAN_ID_CREATE(ONLP_PSU0_FAN_1),
    //ONLP_FAN_ID_CREATE(ONLP_PSU0_FAN_2),
    //ONLP_FAN_ID_CREATE(ONLP_PSU1_FAN_1),
    //ONLP_FAN_ID_CREATE(ONLP_PSU1_FAN_2),
    ONLP_PSU_ID_CREATE(ONLP_PSU_0),
    ONLP_PSU_ID_CREATE(ONLP_PSU_1),
    ONLP_LED_ID_CREATE(ONLP_LED_SYSTEM),
    ONLP_LED_ID_CREATE(ONLP_LED_PSU0),
    ONLP_LED_ID_CREATE(ONLP_LED_PSU1),
    ONLP_LED_ID_CREATE(ONLP_LED_FAN),
    //ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY1),
    //ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY2),
    //ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY3),
    //ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY4),
};

/**
 * @brief Software initializaiton of the Chassis module.
 */
int onlp_chassisi_sw_init(void)
{
    return ONLP_STATUS_OK;
}

/**
 * @brief Hardware initializaiton of the Chassis module.
 * @param flags The hardware initialization flags.
 */
int onlp_chassisi_hw_init(uint32_t flags)
{
    return ONLP_STATUS_OK;
}

/**
 * @brief Deinitialize the chassis software module.
 * @note The primary purpose of this API is to properly
 * deallocate any resources used by the module in order
 * faciliate detection of real resouce leaks.
 */
int onlp_chassisi_sw_denit(void)
{
    return ONLP_STATUS_OK;
}

/**
 * @brief Get the chassis hdr structure.
 * @param id The Chassis OID.
 * @param[out] hdr Receives the header.
 */
int onlp_chassisi_hdr_get(onlp_oid_id_t id, onlp_oid_hdr_t* hdr)
{
    int i;
    ONLP_OID_STATUS_FLAG_SET(hdr, PRESENT);
    ONLP_OID_STATUS_FLAG_SET(hdr, OPERATIONAL);

    memcpy(hdr->coids, __oid_info, sizeof(__oid_info));

    /** Add 40 QSFP and 13 QSFPDD OIDs after the static table */
    onlp_oid_t* e = hdr->coids + AIM_ARRAYSIZE(__oid_info);

    /* 40 QSFP + 13 QSFPDD + 2 SFP+ */
    for(i = 1; i <= 55; i++) {
        *e++ = ONLP_SFP_ID_CREATE(i);
    }

    return ONLP_STATUS_OK;
}
