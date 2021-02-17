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
 * Platform Lib
 *
 ***********************************************************/
#ifndef __PLATFORM_LIB_H__
#define __PLATFORM_LIB_H__

#include "x86_64_ufispace_s9100_32x_log.h"

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


/* Thermal definitions*/
enum onlp_thermal_id {
    ONLP_THERMAL_RESERVED  = 0,
    ONLP_THERMAL_FRONT_MAC = 1,
    ONLP_THERMAL_REAR_MAC  = 2,
    ONLP_THERMAL_CPU1      = 3,
    ONLP_THERMAL_CPU2      = 4,
    ONLP_THERMAL_CPU3      = 5,
    ONLP_THERMAL_CPU4      = 6,
    ONLP_THERMAL_PSU1_1    = 7,
    ONLP_THERMAL_PSU1_2    = 8,
    ONLP_THERMAL_PSU2_1    = 9,
    ONLP_THERMAL_PSU2_2    = 10,
    ONLP_THERMAL_MAX       = ONLP_THERMAL_PSU2_2+1,
};

#define ONLP_THERMAL_COUNT ONLP_THERMAL_MAX /*include "reserved"*/


/* Fan definitions*/
enum onlp_fan_id {
    ONLP_FAN_RESERVED = 0,
    ONLP_FAN_1        = 1,
    ONLP_FAN_2        = 2,
    ONLP_FAN_3        = 3,
    ONLP_FAN_4        = 4,
    ONLP_FAN_5        = 5,
    ONLP_FAN_6        = 6,
    ONLP_FAN_7        = 7,
    ONLP_FAN_8        = 8,
    ONLP_PSU1_FAN_1   = 9,
    ONLP_PSU2_FAN_1   = 10,
    ONLP_FAN_MAX      = ONLP_PSU2_FAN_1+1,
};

#define ONLP_FAN_COUNT ONLP_FAN_MAX /*include "reserved"*/


/* PSU definitions*/
enum onlp_psu_id {
    ONLP_PSU_RESERVED  = 0,
    ONLP_PSU_1         = 1,
    ONLP_PSU_2         = 2,
    ONLP_PSU_1_VIN     = 3,
    ONLP_PSU_1_VOUT    = 4,
    ONLP_PSU_1_IIN     = 5,
    ONLP_PSU_1_IOUT    = 6,
    ONLP_PSU_1_STBVOUT = 7,
    ONLP_PSU_1_STBIOUT = 8,
    ONLP_PSU_2_VIN     = 9,
    ONLP_PSU_2_VOUT    = 10,
    ONLP_PSU_2_IIN     = 11,
    ONLP_PSU_2_IOUT    = 12,
    ONLP_PSU_2_STBVOUT = 13,
    ONLP_PSU_2_STBIOUT = 14,
    ONLP_PSU_MAX       = ONLP_PSU_2+1,
};

#define ONLP_PSU_COUNT ONLP_PSU_MAX /*include "reserved"*/


/* LED definitions*/
enum onlp_led_id {
    ONLP_LED_RESERVED  = 0,
    ONLP_LED_SYSTEM    = 1,
    ONLP_LED_PSU1      = 2,
    ONLP_LED_PSU2      = 3,
    ONLP_LED_FAN       = 4,
    ONLP_LED_FAN_TRAY1 = 5,
    ONLP_LED_FAN_TRAY2 = 6,
    ONLP_LED_FAN_TRAY3 = 7,
    ONLP_LED_FAN_TRAY4 = 8,
    ONLP_LED_MAX       = ONLP_LED_FAN_TRAY4+1,
};

#define ONLP_LED_COUNT ONLP_LED_MAX /*include "reserved"*/

int file_read_hex(int* value, const char* fmt, ...);
int file_vread_hex(int* value, const char* fmt, va_list vargs);
int exec_cmd(char *cmd, char* out, int size);
int get_psu_present_status(int local_id);
int get_psu_pwgood_status(int local_id);
int get_fan_present_status(int local_id);
int get_fan_operational_status(int local_id);


#endif  /* __PLATFORM_LIB_H__ */

