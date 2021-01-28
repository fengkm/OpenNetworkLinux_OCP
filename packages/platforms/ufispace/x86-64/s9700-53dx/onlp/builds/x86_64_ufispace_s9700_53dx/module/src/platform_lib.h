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
#ifndef __PLATFORM_LIB_H__
#define __PLATFORM_LIB_H__

#include "x86_64_ufispace_s9700_53dx_log.h"

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

/* Thermal definitions*/
enum onlp_thermal_id {
    ONLP_THERMAL_RESERVED  = 0,
    ONLP_THERMAL_CPU_PECI  = 1,
    ONLP_THERMAL_OP2_ENV   = 2,
    ONLP_THERMAL_J2_ENV_1  = 3,
    ONLP_THERMAL_J2_DIE_1  = 4,
    ONLP_THERMAL_J2_ENV_2  = 5,
    ONLP_THERMAL_J2_DIE_2  = 6,
    ONLP_THERMAL_PSU0      = 7,
    ONLP_THERMAL_PSU1      = 8,
    ONLP_THERMAL_CPU_PKG   = 9,
    ONLP_THERMAL_CPU1      = 10,
    ONLP_THERMAL_CPU2      = 11,
    ONLP_THERMAL_CPU3      = 12,
    ONLP_THERMAL_CPU4      = 13,
    ONLP_THERMAL_CPU5      = 14,
    ONLP_THERMAL_CPU6      = 15,
    ONLP_THERMAL_CPU7      = 16,
    ONLP_THERMAL_CPU8      = 17,
    ONLP_THERMAL_CPU_BOARD = 18,
    ONLP_THERMAL_BMC_ENV   = 19,
    ONLP_THERMAL_ENV       = 20,
    ONLP_THERMAL_ENV_FRONT = 21,
    ONLP_THERMAL_MAX       = 22,
};

#define ONLP_THERMAL_COUNT ONLP_THERMAL_MAX /*include "reserved"*/


/* Fan definitions*/
enum onlp_fan_id {
    ONLP_FAN_RESERVED = 0,
    ONLP_FAN_1        = 1,
    ONLP_FAN_2        = 2,
    ONLP_FAN_3        = 3,
    ONLP_FAN_4        = 4,
    ONLP_PSU0_FAN_1   = 5,
    ONLP_PSU0_FAN_2   = 6,
    ONLP_PSU1_FAN_1   = 7,
    ONLP_PSU1_FAN_2   = 8,
    ONLP_FAN_MAX      = 9,
};

#define ONLP_FAN_COUNT ONLP_FAN_MAX /*include "reserved"*/


/* PSU definitions*/
enum onlp_psu_id {
    ONLP_PSU_RESERVED  = 0,
    ONLP_PSU_0         = 1,
    ONLP_PSU_1         = 2,
    ONLP_PSU_0_VIN     = 3,
    ONLP_PSU_0_VOUT    = 4,
    ONLP_PSU_0_IIN     = 5,
    ONLP_PSU_0_IOUT    = 6,
    ONLP_PSU_0_STBVOUT = 7,
    ONLP_PSU_0_STBIOUT = 8,
    ONLP_PSU_1_VIN     = 9,
    ONLP_PSU_1_VOUT    = 10,
    ONLP_PSU_1_IIN     = 11,
    ONLP_PSU_1_IOUT    = 12,
    ONLP_PSU_1_STBVOUT = 13,
    ONLP_PSU_1_STBIOUT = 14,
    ONLP_PSU_MAX       = 15
};

#define ONLP_PSU_COUNT ONLP_PSU_MAX /*include "reserved"*/


/* LED definitions*/
enum onlp_led_id {
    ONLP_LED_RESERVED  = 0,
    ONLP_LED_SYSTEM    = 1,
    ONLP_LED_PSU0      = 2,
    ONLP_LED_PSU1      = 3,
    ONLP_LED_FAN       = 4,
    //ONLP_LED_FAN_TRAY1 = 5,
    //ONLP_LED_FAN_TRAY2 = 6,
    //ONLP_LED_FAN_TRAY3 = 7,
    //ONLP_LED_FAN_TRAY4 = 8,
    ONLP_LED_MAX       = 5,
};

#define ONLP_LED_COUNT ONLP_LED_MAX /*include "reserved"*/


#define CPLD_MAX 5  //Number of MB CPLD
extern const int CPLD_BASE_ADDR[CPLD_MAX];

enum sensor
{
    FAN_SENSOR = 0,
    PSU_SENSOR,
    THERMAL_SENSOR,
};

typedef struct bmc_info_s
{
    char name[20];
    float data;
}bmc_info_t;


void lock_init();
int check_file_exist(char *file_path, long *file_time);
int bmc_cache_expired_check(long last_time, long new_time, int cache_time);
int bmc_sensor_read(int bmc_cache_index, int sensor_type, float *data);
int read_ioport(int addr, int *reg_val);
int exec_cmd(char *cmd, char* out, int size);
int get_ipmitool_len(char *ipmitool_out) ;
bool onlp_sysi_bmc_en_get(void);
int parse_bmc_sdr_cmd(char *cmd_out, int cmd_out_size, char *tokens[], int token_size, const char *sensor_id_str, int *idx);
int file_read_hex(int* value, const char* fmt, ...);
int file_vread_hex(int* value, const char* fmt, va_list vargs);
int get_psu_present_status(int local_id);
void check_and_do_i2c_mux_reset(int port);


#endif  /* __PLATFORM_LIB_H__ */

