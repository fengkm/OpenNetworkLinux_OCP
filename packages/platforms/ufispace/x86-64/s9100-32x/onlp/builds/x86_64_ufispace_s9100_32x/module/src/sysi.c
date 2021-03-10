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
 * ONLP System Platform Interface.
 *
 ***********************************************************/
#include <onlp/platformi/sysi.h>
#include <onlp/platformi/ledi.h>
#include <onlp/platformi/psui.h>
#include <onlp/platformi/thermali.h>
#include <onlp/platformi/fani.h>
#include <onlplib/file.h>
#include <onlplib/crc32.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "platform_lib.h"

#define CMD_BIOS_VER  "dmidecode -s bios-version | tail -1 | tr -d '\r\n'"


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

/* update sysi platform info */
static int update_sysi_platform_info(onlp_platform_info_t* info)
{
    int ret = ONLP_STATUS_OK;
    int cpld_version = 0; 
    char bios_out[32] = "";
    int cpld_board_type = 0; 
    int board_build_rev = 0; 
    int board_hw_rev = 0; 
    int board_id = 0; 

    //Get CPLD Version
    ret = file_read_hex(&cpld_version, "/sys/bus/i2c/devices/0-0033/cpld_version");
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }    

    if (cpld_version < 0) { 
        AIM_LOG_ERROR("unable to read CPLD version\n");
        return ONLP_STATUS_E_INTERNAL;
    }    

    info->cpld_versions = aim_fstrdup("0x%02x\n", cpld_version);

    //Get BIOS version 
    if (exec_cmd(CMD_BIOS_VER, bios_out, sizeof(bios_out)) < 0) { 
        AIM_LOG_ERROR("unable to read BIOS version\n");
        return ONLP_STATUS_E_INTERNAL;
    }    

    //Get Board Type (Board Buld Rev, Board HW Rev, Boadr ID)
    ret = file_read_hex(&cpld_board_type, "/sys/bus/i2c/devices/0-0033/cpld_board_type");
    if (ret != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }    

    if (cpld_board_type < 0) { 
        AIM_LOG_ERROR("unable to read CPLD board type\n");
        return ONLP_STATUS_E_INTERNAL;
    }    

    board_build_rev = ((cpld_board_type) & 0x03);
    board_hw_rev = ((cpld_board_type) >> 2 & 0x03);
    board_id = ((cpld_board_type) >> 4);


    info->other_versions = aim_fstrdup(
            "\n" 
            "    [Board Type] 0x%x\n"
            "    [Build Rev ] %d\n"
            "    [HW Rev    ] %d\n"
            "    [Board ID  ] %d\n"
            "    [BIOS      ] %s\n",
            cpld_board_type,
            board_build_rev,
            board_hw_rev,
            board_id,
            bios_out);

    return ONLP_STATUS_OK;
}

/**
 * @brief Return the name of the the platform implementation.
 * @notes This will be called PRIOR to any other calls into the
 * platform driver, including the sysi_init() function below.
 *
 * The platform implementation name should match the current
 * ONLP platform name.
 *
 * IF the platform implementation name equals the current platform name,
 * initialization will continue.
 *
 * If the platform implementation name does not match, the following will be
 * attempted:
 *
 *    onlp_sysi_platform_set(current_platform_name);
 * If this call is successful, initialization will continue.
 * If this call fails, platform initialization will abort().
 *
 * The onlp_sysi_platform_set() function is optional.
 * The onlp_sysi_platform_get() is not optional.
 */
const char* onlp_sysi_platform_get(void)
{
    return "x86-64-ufispace-s9100-32x-r0";
}

/**
 * @brief Attempt to set the platform personality
 * in the event that the current platform does not match the
 * reported platform.
 * @note Optional
 */
int onlp_sysi_platform_set(const char* platform)
{
    return ONLP_STATUS_OK;
}


/**
 * @brief Initialize the system platform subsystem.
 */
int onlp_sysi_init(void)
{
    return ONLP_STATUS_OK;
}


/**
 * @brief Provide the physical base address for the ONIE eeprom.
 * @param param [out] physaddr Receives the physical address.
 * @notes If your platform provides a memory-mappable base
 * address for the ONIE eeprom data you can return it here.
 * The ONLP common code will then use this address and decode
 * the ONIE TLV specification data. If you cannot return a mappable
 * address due to the platform organization see onlp_sysi_onie_data_get()
 * instead.
 */
int onlp_sysi_onie_data_phys_addr_get(void** physaddr) 
{
     return ONLP_STATUS_E_UNSUPPORTED;
}


/**
 * @brief Return the raw contents of the ONIE system eeprom.
 * @param data [out] Receives the data pointer to the ONIE data.
 * @param size [out] Receives the size of the data (if available).
 * @notes This function is only necessary if you cannot provide
 * the physical base address as per onlp_sysi_onie_data_phys_addr_get().
 */
int onlp_sysi_onie_data_get(uint8_t** data, int* size)
{
    uint8_t* rdata = aim_zmalloc(512);

    if(onlp_file_read(rdata, 512, size, "/sys/bus/i2c/devices/9-0054/eeprom") == ONLP_STATUS_OK) {
        if(*size == 512) {
            *data = rdata;
            return ONLP_STATUS_OK;
        }
    }

    AIM_LOG_INFO("Unable to data get eeprom \n");    
    aim_free(rdata);
    *size = 0;
    return ONLP_STATUS_E_INTERNAL;
}


/**
 * @brief Free the data returned by onlp_sys_onie_data_get()
 * @param data The data pointer.
 * @notes If onlp_sysi_onie_data_get() is called to retreive the
 * contents of the ONIE system eeprom then this function
 * will be called to perform any cleanup that may be necessary
 * after the data has been used.
 */
void onlp_sysi_onie_data_free(uint8_t* data)
{
    /* If onlp_sysi_onie_data_get() allocated, it has be freed here. */
    if (data) {
        aim_free(data);
    } 
}


/**
 * @brief Return the ONIE system information for this platform.
 * @param onie The onie information structure.
 * @notes If all previous attempts to get the eeprom data fail
 * then this routine will be called. Used as a translation option
 * for platforms without access to an ONIE-formatted eeprom.
 */
int onlp_sysi_onie_info_get(onlp_onie_info_t* onie)
{
     return ONLP_STATUS_E_UNSUPPORTED;
}


/**
 * @brief This function returns the root oid list for the platform.
 * @param table [out] Receives the table.
 * @param max The maximum number of entries you can fill.
 */
int onlp_sysi_oids_get(onlp_oid_t* table, int max)
{

    memcpy(table, __onlp_oid_info, sizeof(__onlp_oid_info));

#if 0
    /** Add 32 QSFP OIDs after the static table */
    onlp_oid_t* e = table + AIM_ARRAYSIZE(__onlp_oid_info);

    /* 32 QSFP */
    for(i = 1; i <= 32; i++) {
        *e++ = ONLP_SFP_ID_CREATE(i);
    }
#endif

    return ONLP_STATUS_OK;
}


/**
 * @brief This function provides a generic ioctl interface.
 * @param code context dependent.
 * @param vargs The variable argument list for the ioctl call.
 * @notes This is provided as a generic expansion and
 * and custom programming mechanism for future and non-standard
 * functionality.
 * @notes Optional
 */
int onlp_sysi_ioctl(int code, va_list vargs)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}


/**
 * @brief Platform management initialization.
 */
int onlp_sysi_platform_manage_init(void)
{
    return ONLP_STATUS_OK;
}


/**
 * @brief Perform necessary platform fan management.
 * @note This function should automatically adjust the FAN speeds
 * according to the platform conditions.
 */
int onlp_sysi_platform_manage_fans(void)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}


/**
 * @brief Perform necessary platform LED management.
 * @note This function should automatically adjust the LED indicators
 * according to the platform conditions.
 */
int onlp_sysi_platform_manage_leds(void)
{
    onlp_led_info_t info;

    ONLP_TRY(onlp_ledi_info_get(ONLP_LED_ID_CREATE(ONLP_LED_SYSTEM), &info));
    ONLP_TRY(onlp_ledi_info_get(ONLP_LED_ID_CREATE(ONLP_LED_PSU1), &info));
    ONLP_TRY(onlp_ledi_info_get(ONLP_LED_ID_CREATE(ONLP_LED_PSU2), &info));
    ONLP_TRY(onlp_ledi_info_get(ONLP_LED_ID_CREATE(ONLP_LED_FAN), &info));
    ONLP_TRY(onlp_ledi_info_get(ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY1), &info));
    ONLP_TRY(onlp_ledi_info_get(ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY2), &info));
    ONLP_TRY(onlp_ledi_info_get(ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY3), &info));
    ONLP_TRY(onlp_ledi_info_get(ONLP_LED_ID_CREATE(ONLP_LED_FAN_TRAY4), &info));

    return ONLP_STATUS_OK;
}


/**
 * @brief Return custom platform information.
 */
int onlp_sysi_platform_info_get(onlp_platform_info_t* info)
{

    ONLP_TRY(update_sysi_platform_info(info));

    return ONLP_STATUS_OK;
}

/**
 * @brief Friee a custom platform information structure.
 */
void onlp_sysi_platform_info_free(onlp_platform_info_t* info)
{
    if (info->cpld_versions) {
        aim_free(info->cpld_versions);
    }   

    if (info->other_versions) {
        aim_free(info->other_versions);
    } 
}

/**
 * @brief Builtin platform debug tool.
 */
int onlp_sysi_debug(aim_pvs_t* pvs, int argc, char** argv)
{
     return ONLP_STATUS_E_UNSUPPORTED;
}

