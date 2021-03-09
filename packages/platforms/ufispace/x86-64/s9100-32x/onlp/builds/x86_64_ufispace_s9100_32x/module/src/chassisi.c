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
 * [01] CHASSIS----[01] ONLP_THERMAL_FRONT_MAC
 *            |----[02] ONLP_THERMAL_REAR_MAC
 *            |----[03] ONLP_THERMAL_CPU1
 *            |----[04] ONLP_THERMAL_CPU2
 *            |----[05] ONLP_THERMAL_CPU3
 *            |----[06] ONLP_THERMAL_CPU4
 *            |
 *            |----[01] ONLP_FAN_1----[05] ONLP_LED_FAN_TRAY1
 *            |----[02] ONLP_FAN_2----[05] ONLP_LED_FAN_TRAY1
 *            |----[03] ONLP_FAN_3----[06] ONLP_LED_FAN_TRAY2
 *            |----[04] ONLP_FAN_4----[06] ONLP_LED_FAN_TRAY2
 *            |----[05] ONLP_FAN_5----[07] ONLP_LED_FAN_TRAY3
 *            |----[06] ONLP_FAN_6----[07] ONLP_LED_FAN_TRAY3
 *            |----[07] ONLP_FAN_7----[08] ONLP_LED_FAN_TRAY4
 *            |----[08] ONLP_FAN_8----[08] ONLP_LED_FAN_TRAY4
 *            |
 *            |----[01] ONLP_PSU_1----[07] ONLP_THERMAL_PSU1_1
 *            |                  |----[08] ONLP_THERMAL_PSU1_2
 *            |                  |----[09] ONLP_PSU1_FAN_1
 *            |
 *            |----[02] ONLP_PSU_2----[09] ONLP_THERMAL_PSU2_1
 *            |                  |----[10] ONLP_THERMAL_PSU2_2
 *            |                  |----[10] ONLP_PSU2_FAN_1
 *            |
 *            |----[01] ONLP_LED_SYSTEM
 *            |----[02] ONLP_LED_PSU1
 *            |----[03] ONLP_LED_PSU2
 *            |----[04] ONLP_LED_FAN
 */
static onlp_oid_t __onlp_oid_info[] = { 
    ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_FRONT_MAC),
    ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_REAR_MAC),
    ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_CPU1),
    ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_CPU2),
    ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_CPU3),
    ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_CPU4),
    //ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_PSU1_1),
    //ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_PSU1_2),
    //ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_PSU2_1),
    //ONLP_THERMAL_ID_CREATE(ONLP_THERMAL_PSU2_2),
    ONLP_FAN_ID_CREATE(ONLP_FAN_1),
    ONLP_FAN_ID_CREATE(ONLP_FAN_2),
    ONLP_FAN_ID_CREATE(ONLP_FAN_3),
    ONLP_FAN_ID_CREATE(ONLP_FAN_4),
    ONLP_FAN_ID_CREATE(ONLP_FAN_5),
    ONLP_FAN_ID_CREATE(ONLP_FAN_6),
    ONLP_FAN_ID_CREATE(ONLP_FAN_7),
    ONLP_FAN_ID_CREATE(ONLP_FAN_8),
    //ONLP_FAN_ID_CREATE(ONLP_PSU1_FAN_1),
    //ONLP_FAN_ID_CREATE(ONLP_PSU2_FAN_1),
    ONLP_PSU_ID_CREATE(ONLP_PSU_1),
    ONLP_PSU_ID_CREATE(ONLP_PSU_2),
    ONLP_LED_ID_CREATE(ONLP_LED_SYSTEM),
    ONLP_LED_ID_CREATE(ONLP_LED_PSU1),
    ONLP_LED_ID_CREATE(ONLP_LED_PSU2),
    ONLP_LED_ID_CREATE(ONLP_LED_FAN),
    //ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY1), //connect behind ONLP_FAN_1,2
    //ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY2), //connect behind ONLP_FAN_3,4
    //ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY3), //connect behind ONLP_FAN_5,6
    //ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY4), //connect behind ONLP_FAN_7,8
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

    memcpy(hdr->coids, __onlp_oid_info, sizeof(__onlp_oid_info));

    /* Add 32 QSFP OIDs after the static table */
    onlp_oid_t* e = hdr->coids + AIM_ARRAYSIZE(__onlp_oid_info);

    /* 32 QSFP */
    for(i = 1; i <= 32; i++) {
        *e++ = ONLP_SFP_ID_CREATE(i);
    }

    return ONLP_STATUS_OK;
}
