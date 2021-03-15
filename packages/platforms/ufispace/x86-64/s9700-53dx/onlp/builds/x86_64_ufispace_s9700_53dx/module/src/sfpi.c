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
 * sfpi
 *
 ***********************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <onlp/platformi/sfpi.h>
#include <onlplib/i2c.h>
#include <onlplib/file.h>
//#include <fcntl.h> /* For O_RDWR && open */
//#include <sys/ioctl.h>
//#include <dirent.h>

#include "platform_lib.h"


#define QSFP_PORT_NUM    40
#define QSFPDD_PORT_NUM  13
#define SFP_PORT_NUM     2
#define ALL_PORT_NUM     55

#define SFP0_INTERFACE_NAME "enp2s0f0"
#define SFP1_INTERFACE_NAME "enp2s0f1"


static int qsfp_port_eeprom_bus_id_array[QSFP_PORT_NUM] = { 25, 26, 27, 28, 29, \
                                                            30, 31, 32, 33, 34, \
                                                            35, 36, 37, 38, 39, \
                                                            40, 41, 42, 43, 44, \
                                                            46, 45, 48, 47, 50, \
                                                            49, 52, 51, 54, 53, \
                                                            56, 55, 58, 57, 60, \
                                                            59, 62, 61, 64, 63 };

static char qsfp_port_status_cpld_addr_array[QSFP_PORT_NUM][5] = {"0030", "0030", "0030", "0030", "0030", \
                                                                  "0030", "0030", "0030", "0030", "0030", \
                                                                  "0030", "0030", "0039", "0039", "0039", \
                                                                  "0039", "0039", "0039", "0039", "0039", \
                                                                  "0039", "0039", "0039", "0039", "003a", \
                                                                  "0039", "003a", "003a", "003a", "003a", \
                                                                  "003a", "003a", "003a", "003a", "003a", \
                                                                  "003a", "003a", "003a", "003b", "003b" };

static char qsfp_port_status_cpld_index_array[QSFP_PORT_NUM][3] = {"0" , "1" , "2" , "3" , "4" , \
                                                                   "5" , "6" , "7" , "8" , "9" , \
                                                                   "10", "11", "0" , "1" , "2" , \
                                                                   "3" , "4" , "5" , "6" , "7" , \
                                                                   "9" , "8" , "11", "10", "0" , \
                                                                   "12", "2" , "1" , "4" , "3" , \
                                                                   "6" , "5" , "8" , "7" , "10", \
                                                                   "9" , "12", "11", "1" , "0" };

static int qsfpdd_port_eeprom_bus_id_array[QSFPDD_PORT_NUM] = { 65, 66, 67, 68, 69, \
                                                                70, 71, 72, 73, 74, \
                                                                75, 76, 77};

static char qsfpdd_port_status_cpld_addr_array[QSFPDD_PORT_NUM][5] = {"003b", "003b", "003b", "003c", "003c", \
                                                                      "003c", "003c", "003c", "003c", "003c", \
                                                                      "003c", "003c", "003c"};

static char qsfpdd_port_status_cpld_index_array[QSFP_PORT_NUM][3] = {"0", "1", "2", "0", "1", \
                                                                     "2", "3", "4", "5", "6", \
                                                                     "7", "8", "9"};

/* to get cpld qsfp port status register */
static int ufi_get_qsfp_port_status_reg(int port_id, int *reg)
{
    int ret = ONLP_STATUS_OK;
    int cpld_qsfp_port_status_reg = 0;
    char qsfp_port_status_sysfs[255] = ""; 
    char buf[ONLP_CONFIG_INFO_STR_MAX] = ""; 
    int bus_id = 1;
    int len = 0;

    snprintf(qsfp_port_status_sysfs, sizeof(qsfp_port_status_sysfs), 
            "/sys/bus/i2c/devices/%d-%s/cpld_qsfp_port_status_%s", 
            bus_id, 
            qsfp_port_status_cpld_addr_array[port_id], 
            qsfp_port_status_cpld_index_array[port_id]);

    ONLP_TRY(onlp_file_read((uint8_t*)&buf, ONLP_CONFIG_INFO_STR_MAX, &len, qsfp_port_status_sysfs));

    /* qsfp_port_status_sysfs output is hex value (0xab), hex to int */
    cpld_qsfp_port_status_reg = (int) strtol((char *)buf, NULL, 0); 

    *reg = cpld_qsfp_port_status_reg;

    return ret;
}

/* to get cpld qsfp port config register */
static int ufi_get_qsfp_port_config_reg(int port_id, int *reg)
{
    int ret = ONLP_STATUS_OK;
    int cpld_qsfp_port_config_reg = 0;
    char qsfp_port_config_sysfs[255] = ""; 
    char buf[ONLP_CONFIG_INFO_STR_MAX] = ""; 
    int bus_id = 1;
    int len = 0;

    snprintf(qsfp_port_config_sysfs, sizeof(qsfp_port_config_sysfs), 
            "/sys/bus/i2c/devices/%d-%s/cpld_qsfp_port_config_%s",
            bus_id, 
            qsfp_port_status_cpld_addr_array[port_id], 
            qsfp_port_status_cpld_index_array[port_id]);

    ONLP_TRY(onlp_file_read((uint8_t*)&buf, ONLP_CONFIG_INFO_STR_MAX, &len, qsfp_port_config_sysfs));

    /* qsfp_port_config_sysfs output is hex value (0xab), hex to int */
    cpld_qsfp_port_config_reg = (int) strtol((char *)buf, NULL, 0); 

    *reg = cpld_qsfp_port_config_reg;

    return ret;
}


/* to set cpld qsfp port config register */
static int ufi_set_qsfp_port_config_reg(int port_id, int value)
{
    int ret = ONLP_STATUS_OK;
    char qsfp_port_config_sysfs[255] = ""; 
    int bus_id = 1;

    snprintf(qsfp_port_config_sysfs, sizeof(qsfp_port_config_sysfs), 
            "/sys/bus/i2c/devices/%d-%s/cpld_qsfp_port_config_%s",
            bus_id, 
            qsfp_port_status_cpld_addr_array[port_id], 
            qsfp_port_status_cpld_index_array[port_id]);

    ONLP_TRY(onlp_file_write_int(value, qsfp_port_config_sysfs));

    return ret;
}

/* to get cpld qsfpdd port status register */
static int ufi_get_qsfpdd_port_status_reg(int port_id, int *reg)
{
    int ret = ONLP_STATUS_OK;
    int cpld_qsfpdd_port_status_reg = 0;
    char qsfpdd_port_status_sysfs[255] = ""; 
    char buf[ONLP_CONFIG_INFO_STR_MAX] = ""; 
    int bus_id = 1;
    int len = 0;

    snprintf(qsfpdd_port_status_sysfs, sizeof(qsfpdd_port_status_sysfs), 
            "/sys/bus/i2c/devices/%d-%s/cpld_qsfpdd_port_status_%s",
            bus_id, 
            qsfpdd_port_status_cpld_addr_array[port_id], 
            qsfpdd_port_status_cpld_index_array[port_id]);

    ONLP_TRY(onlp_file_read((uint8_t*)&buf, ONLP_CONFIG_INFO_STR_MAX, &len, qsfpdd_port_status_sysfs));

    /* qsfpdd_port_status_sysfs output is hex value (0xab), hex to int */
    cpld_qsfpdd_port_status_reg = (int) strtol((char *)buf, NULL, 0); 

    *reg = cpld_qsfpdd_port_status_reg;

    return ret;
}

/* to get cpld qsfpdd port config register */
static int ufi_get_qsfpdd_port_config_reg(int port_id, int *reg)
{
    int ret = ONLP_STATUS_OK;
    int cpld_qsfpdd_port_config_reg = 0;
    char qsfpdd_port_config_sysfs[255] = ""; 
    char buf[ONLP_CONFIG_INFO_STR_MAX] = ""; 
    int bus_id = 1;
    int len = 0;

    snprintf(qsfpdd_port_config_sysfs, sizeof(qsfpdd_port_config_sysfs), 
            "/sys/bus/i2c/devices/%d-%s/cpld_qsfpdd_port_config_%s",
            bus_id, 
            qsfpdd_port_status_cpld_addr_array[port_id], 
            qsfpdd_port_status_cpld_index_array[port_id]);

    ONLP_TRY(onlp_file_read((uint8_t*)&buf, ONLP_CONFIG_INFO_STR_MAX, &len, qsfpdd_port_config_sysfs));

    /* qsfpdd_port_config_sysfs output is hex value (0xab), hex to int */
    cpld_qsfpdd_port_config_reg = (int) strtol((char *)buf, NULL, 0); 

    *reg = cpld_qsfpdd_port_config_reg;

    return ret;
}

/* to set cpld qsfpdd port config register */
static int ufi_set_qsfpdd_port_config_reg(int port_id, int value)
{
    int ret = ONLP_STATUS_OK;
    char qsfpdd_port_config_sysfs[255] = ""; 
    int bus_id = 1;

    snprintf(qsfpdd_port_config_sysfs, sizeof(qsfpdd_port_config_sysfs), 
            "/sys/bus/i2c/devices/%d-%s/cpld_qsfpdd_port_config_%s",
            bus_id, 
            qsfpdd_port_status_cpld_addr_array[port_id], 
            qsfpdd_port_status_cpld_index_array[port_id]);

    ONLP_TRY(onlp_file_write_int(value, qsfpdd_port_config_sysfs));

    return ret;
}

/* to get cpld sfp port status register */
static int ufi_get_sfp_port_status_reg(int port_id, int *reg)
{
    int ret = ONLP_STATUS_OK;
    int cpld_sfp_port_status_reg = 0;
    char sfp_port_status_sysfs[255] = ""; 
    char buf[ONLP_CONFIG_INFO_STR_MAX] = ""; 
    int bus_id = 1;
    int len = 0;

    snprintf(sfp_port_status_sysfs, sizeof(sfp_port_status_sysfs), 
            "/sys/bus/i2c/devices/%d-0030/cpld_sfp_port_status", bus_id);

    ONLP_TRY(onlp_file_read((uint8_t*)&buf, ONLP_CONFIG_INFO_STR_MAX, &len, sfp_port_status_sysfs));

    /* sfp_port_status_sysfs output is hex value (0xab), hex to int */
    cpld_sfp_port_status_reg = (int) strtol((char *)buf, NULL, 0); 

    *reg = cpld_sfp_port_status_reg;

    return ret;
}

/* to get cpld sfp port config register */
static int ufi_get_sfp_port_config_reg(int port_id, int *reg)
{
    int ret = ONLP_STATUS_OK;
    int cpld_sfp_port_config_reg = 0;
    char sfp_port_config_sysfs[255] = ""; 
    char buf[ONLP_CONFIG_INFO_STR_MAX] = ""; 
    int bus_id = 1;
    int len = 0;

    snprintf(sfp_port_config_sysfs, sizeof(sfp_port_config_sysfs), 
            "/sys/bus/i2c/devices/%d-0030/cpld_sfp_port_config", bus_id);

    ONLP_TRY(onlp_file_read((uint8_t*)&buf, ONLP_CONFIG_INFO_STR_MAX, &len, sfp_port_config_sysfs));

    /* qsfpdd_port_config_sysfs output is hex value (0xab), hex to int */
    cpld_sfp_port_config_reg = (int) strtol((char *)buf, NULL, 0); 

    *reg = cpld_sfp_port_config_reg;

    return ret;
}

/* to set cpld sfp port config register */
static int ufi_set_sfp_port_config_reg(int port_id, int value)
{
    int ret = ONLP_STATUS_OK;
    char sfp_port_config_sysfs[255] = ""; 
    int bus_id = 1;

    snprintf(sfp_port_config_sysfs, sizeof(sfp_port_config_sysfs), 
            "/sys/bus/i2c/devices/%d-0030/cpld_sfp_port_config", bus_id);

    ONLP_TRY(onlp_file_write_int(value, sfp_port_config_sysfs));

    return ret;
}

/* to get port present status */
static int ufi_get_port_present_status(int local_id, int *status)
{
    int ret = ONLP_STATUS_OK;
    int port_status_reg = 0;
    int port_id;
    char command[256] = "";

    //QSFP, QSFPDD, SFP Ports
    if (local_id < QSFP_PORT_NUM) { //QSFP
        port_id = local_id;
        ret = ufi_get_qsfp_port_status_reg(port_id, &port_status_reg);
        if (ret != ONLP_STATUS_OK) {
            AIM_LOG_ERROR("ufi_get_qsfp_port_status_reg() failed, port=%d\n", port_id);
            return ONLP_STATUS_E_INTERNAL;
        } else {
            *status = !((port_status_reg & 0b00000010) >> 1);
        }
    } else if (local_id >= QSFP_PORT_NUM && local_id < (QSFP_PORT_NUM + QSFPDD_PORT_NUM)) { //QSFPDD
        port_id = local_id - QSFP_PORT_NUM;
        ret = ufi_get_qsfpdd_port_status_reg(port_id, &port_status_reg);
        if (ret != ONLP_STATUS_OK) {
            AIM_LOG_ERROR("ufi_get_qsfpdd_port_status_reg() failed, port=%d\n", port_id);
            return ONLP_STATUS_E_INTERNAL;
        } else {
            *status = !((port_status_reg & 0b00000010) >> 1);
        }
    } else if (local_id >= (QSFP_PORT_NUM + QSFPDD_PORT_NUM) && local_id < ALL_PORT_NUM) { //SFP
        port_id = local_id - (QSFP_PORT_NUM + QSFPDD_PORT_NUM);
        if (port_id == 0) { //SFP port0
            snprintf(command, sizeof(command), "ethtool -m %s raw on length 1 > /dev/null 2>&1", SFP0_INTERFACE_NAME);
            ret = system(command);
            *status = (ret==0) ? 1 : 0;
        } else if (port_id == 1) { //SFP Port1
            snprintf(command, sizeof(command), "ethtool -m %s raw on length 1 > /dev/null 2>&1", SFP1_INTERFACE_NAME);
            ret = system(command);
            *status = (ret==0) ? 1 : 0;
        } else {
            AIM_LOG_ERROR("unknown SFP ports, port=%d\n", port_id);
            return ONLP_STATUS_E_UNSUPPORTED;
        }
    } else { //unkonwn ports
        AIM_LOG_ERROR("unknown ports, local_id=%d, func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_UNSUPPORTED;
    }

    return ONLP_STATUS_OK;
}

static int ufi_sfpi_sfp_dev_read(int port_id, uint8_t* dst, int len) {
    int ret = ONLP_STATUS_OK;
    char command[256] = "";
    char eeprom_path[256] = "";
    int size = 0;

    if (port_id == 0) {
        snprintf(command, sizeof(command), "ethtool -m %s raw on length %d > /tmp/.sfp.%s.eeprom", SFP0_INTERFACE_NAME, len, SFP0_INTERFACE_NAME);
        snprintf(eeprom_path, sizeof(eeprom_path), "/tmp/.sfp.%s.eeprom", SFP0_INTERFACE_NAME);
    } else if (port_id == 1) {
        snprintf(command, sizeof(command), "ethtool -m %s raw on length %d > /tmp/.sfp.%s.eeprom", SFP1_INTERFACE_NAME, len, SFP1_INTERFACE_NAME);
        snprintf(eeprom_path, sizeof(eeprom_path), "/tmp/.sfp.%s.eeprom", SFP1_INTERFACE_NAME);
    } else {
        AIM_LOG_ERROR("unknown SFP ports, port=%d\n", port_id);
        return ONLP_STATUS_E_PARAM;
    }
    
    ret = system(command);
    if (ret != 0) {
        AIM_LOG_ERROR("Unable to read sfp eeprom (port_id=%d), func=%s\n", port_id, __FUNCTION__);
        return ONLP_STATUS_E_INTERNAL;
    }
    
    ONLP_TRY(onlp_file_read(dst, len, &size, eeprom_path));

    return ONLP_STATUS_OK;
}

/**
 * @brief Software initialization of the SFP module.
 */
int onlp_sfpi_sw_init(void)
{
    lock_init();
    return ONLP_STATUS_OK;
}

/**
 * @brief Hardware initialization of the SFP module.
 * @param flags The hardware initialization flags.
 */
int onlp_sfpi_hw_init(uint32_t flags)
{
    return ONLP_STATUS_OK;
}

/**
 * @brief Deinitialize the chassis software module.
 * @note The primary purpose of this API is to properly
 * deallocate any resources used by the module in order
 * faciliate detection of real resouce leaks.
 */
int onlp_sfpi_sw_denit(void)
{
    return ONLP_STATUS_OK;
}

/**
 * @brief Get the bitmap of SFP-capable port numbers.
 * @param[out] bmap Receives the bitmap.
 */
int onlp_sfpi_bitmap_get(onlp_sfp_bitmap_t* bmap)
{
    int p;

    AIM_BITMAP_CLR_ALL(bmap);
    for(p = 0; p < ALL_PORT_NUM; p++) {
        AIM_BITMAP_SET(bmap, p);
    }
    
    return ONLP_STATUS_OK;
}

/**
 * @brief Determine the SFP connector type.
 * @param id The SFP Port ID.
 * @param[out] rtype Receives the connector type.
 */
int onlp_sfpi_type_get(onlp_oid_id_t id, onlp_sfp_type_t* rtype)
{
    int local_id = ONLP_OID_ID_GET(id);

    //QSFP, QSFPDD, SFP Ports
    if (local_id < QSFP_PORT_NUM) { //QSFP
        *rtype = ONLP_SFP_TYPE_QSFP28;
    } else if (local_id >= QSFP_PORT_NUM && local_id < (QSFP_PORT_NUM + QSFPDD_PORT_NUM)) { //QSFPDD
        *rtype = ONLP_SFP_TYPE_QSFP28;
    } else if (local_id >= (QSFP_PORT_NUM + QSFPDD_PORT_NUM) && local_id < ALL_PORT_NUM) { //SFP
        *rtype = ONLP_SFP_TYPE_SFP;
    } else { //unkonwn ports
        AIM_LOG_ERROR("unknown ports, port=%d, func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_PARAM;
    }

    return ONLP_STATUS_OK;
}

/**
 * @brief Determine if an SFP is present.
 * @param id The SFP Port ID.
 * @returns 1 if present
 * @returns 0 if absent
 * @returns An error condition.
 */
int onlp_sfpi_is_present(onlp_oid_id_t id)
{
    int local_id = ONLP_OID_ID_GET(id);
    int status = 0;
    int ret = ONLP_STATUS_OK;

    if (local_id > ALL_PORT_NUM) {
        AIM_LOG_ERROR("unknown ports, local_id=%d, func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_UNSUPPORTED;
    } else {
        ret = ufi_get_port_present_status(local_id, &status);
        if (ret != ONLP_STATUS_OK) {
            AIM_LOG_ERROR("ufi_get_port_present_status() failed, local_id=%d\n", local_id);
            return ONLP_STATUS_E_INTERNAL;
        }
    }

    return status;
}

/**
 * @brief Return the presence bitmap for all SFP ports.
 * @param[out] dst Receives the presence bitmap.
 */
int onlp_sfpi_presence_bitmap_get(onlp_sfp_bitmap_t* dst)
{
    int p = 0;
    int ret = 0;

    AIM_BITMAP_CLR_ALL(dst);
    for (p = 0; p < ALL_PORT_NUM; p++) {
        ret = onlp_sfpi_is_present(p);
        AIM_BITMAP_MOD(dst, p, ret);
    }

    return ONLP_STATUS_OK;
}

/**
 * @brief Return the RX_LOS bitmap for all SFP ports.
 * @param[out] dst Receives the RX_LOS bitmap.
 */
int onlp_sfpi_rx_los_bitmap_get(onlp_sfp_bitmap_t* dst)
{
    int ret = ONLP_STATUS_OK;
    int i = 0, value = 0;

    AIM_BITMAP_CLR_ALL(dst);
    for (i = 0; i < ALL_PORT_NUM; i++) {
        ret = onlp_sfpi_control_get(i, ONLP_SFP_CONTROL_RX_LOS, &value);
        if (ret == ONLP_STATUS_OK) {
            AIM_BITMAP_MOD(dst, i, value);
        } else {
            AIM_LOG_ERROR("get RX_LOS bitmap failed, port=%d, func=%s\n", i, __FUNCTION__);
            return ret;
        }
    }

    return ONLP_STATUS_OK;
}

/**
 * @brief Read bytes from the target device on the given SFP port.
 * @param id The SFP Port ID.
 * @param devaddr The device address.
 * @param addr Read offset.
 * @param[out] dst Receives the data.
 * @param len Read length.
 * @returns The number of bytes read or ONLP_STATUS_E_* no error.
 */
int onlp_sfpi_dev_read(onlp_oid_id_t id, int devaddr, int addr,
                       uint8_t* dst, int len)
{
    int local_id = ONLP_OID_ID_GET(id);
    int bus_id = -1;
    int ret = ONLP_STATUS_OK;
    int port_id;

    if (onlp_sfpi_is_present(id) != 1) {
        AIM_LOG_INFO("sfp module (port=%d) is absent.\n", local_id);
        return ONLP_STATUS_OK;
    }

    devaddr = 0x50;
    if (local_id < QSFP_PORT_NUM) {
        /* QSFP */
        port_id = local_id;
        bus_id = qsfp_port_eeprom_bus_id_array[port_id];
        ONLP_TRY(onlp_i2c_block_read(bus_id, devaddr, addr, len, dst, ONLP_I2C_F_FORCE));

    } else if (local_id >= QSFP_PORT_NUM && local_id < (QSFP_PORT_NUM + QSFPDD_PORT_NUM)) {
        /* QSFPDD */
        port_id = local_id - QSFP_PORT_NUM;
        bus_id = qsfpdd_port_eeprom_bus_id_array[port_id];
        ONLP_TRY(onlp_i2c_block_read(bus_id, devaddr, addr, len, dst, ONLP_I2C_F_FORCE));

    } else if (local_id >= (QSFP_PORT_NUM + QSFPDD_PORT_NUM) && local_id < ALL_PORT_NUM) {
        /* SFP */
        port_id = local_id - (QSFP_PORT_NUM + QSFPDD_PORT_NUM);
        ret = ufi_sfpi_sfp_dev_read(port_id, dst, len);
        if (ret != ONLP_STATUS_OK) {
            AIM_LOG_ERROR("ufi_sfpi_sfp_dev_read(port_id=%d, len=%d) read failed!!\n", port_id, len);
            return ret;
        }
    } else {
        return ONLP_STATUS_E_PARAM;
    }

    return ONLP_STATUS_OK;
}


/**
 * @brief Write bytes to the target device on the given SFP port.
 * @param id The SFP Port ID.
 * @param devaddr The device address.
 * @param addr Write offset.
 * @param src The bytes to write.
 * @param len Write length.
 */
int onlp_sfpi_dev_write(onlp_oid_id_t id, int devaddr, int addr,
                        uint8_t* src, int len)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

/**
 * @brief Read a byte from the target device on the given SFP port.
 * @param id The SFP Port ID.
 * @param devaddr The device address.
 * @param addr The read address.
 * @returns The byte on success or ONLP_STATUS_E* on error.
 */
int onlp_sfpi_dev_readb(onlp_oid_id_t id, int devaddr, int addr)
{
    int local_id = ONLP_OID_ID_GET(id);
    int bus_id = -1;
    int ret = ONLP_STATUS_OK;
    int port_id;

    if (onlp_sfpi_is_present(id) != 1) {
        AIM_LOG_INFO("sfp module (port=%d) is absent.\n", local_id);
        return ONLP_STATUS_OK;
    }

    devaddr = 0x50;
    if (local_id < QSFP_PORT_NUM) {
        /* QSFP */
        port_id = local_id;
        bus_id = qsfp_port_eeprom_bus_id_array[port_id];
        ret = onlp_i2c_readb(bus_id, devaddr, addr, ONLP_I2C_F_FORCE);

    } else if (local_id >= QSFP_PORT_NUM && local_id < (QSFP_PORT_NUM + QSFPDD_PORT_NUM)) {
        /* QSFPDD */
        port_id = local_id - QSFP_PORT_NUM;
        bus_id = qsfpdd_port_eeprom_bus_id_array[port_id];
        ret = onlp_i2c_readb(bus_id, devaddr, addr, ONLP_I2C_F_FORCE);

    } else if (local_id >= (QSFP_PORT_NUM + QSFPDD_PORT_NUM) && local_id < ALL_PORT_NUM) {
        /* SFP */
        return ONLP_STATUS_E_UNSUPPORTED;

    } else {
        return ONLP_STATUS_E_PARAM;
    }

    return ret;
}

/**
 * @brief Write a byte to the target device on the given SFP port.
 * @param id The SFP Port ID.
 * @param devaddr The device address.
 * @param addr The write address.
 * @param value The write value.
 */
int onlp_sfpi_dev_writeb(onlp_oid_id_t id, int devaddr, int addr,
                         uint8_t value)
{
    int local_id = ONLP_OID_ID_GET(id);
    int bus_id = -1;
    int ret = ONLP_STATUS_OK;
    int port_id;

    if (onlp_sfpi_is_present(id) != 1) {
        AIM_LOG_INFO("sfp module (port=%d) is absent.\n", local_id);
        return ONLP_STATUS_OK;
    }

    devaddr = 0x50;
    if (local_id < QSFP_PORT_NUM) {
        /* QSFP */
        port_id = local_id;
        bus_id = qsfp_port_eeprom_bus_id_array[port_id];
        ret = onlp_i2c_writeb(bus_id, devaddr, addr, value, ONLP_I2C_F_FORCE);

    } else if (local_id >= QSFP_PORT_NUM && local_id < (QSFP_PORT_NUM + QSFPDD_PORT_NUM)) {
        /* QSFPDD */
        port_id = local_id - QSFP_PORT_NUM;
        bus_id = qsfpdd_port_eeprom_bus_id_array[port_id];
        ret = onlp_i2c_writeb(bus_id, devaddr, addr, value, ONLP_I2C_F_FORCE);

    } else if (local_id >= (QSFP_PORT_NUM + QSFPDD_PORT_NUM) && local_id < ALL_PORT_NUM) {
        /* SFP */
        return ONLP_STATUS_E_UNSUPPORTED;

    } else {
        return ONLP_STATUS_E_PARAM;
    }

    return ret;
}

/**
 * @brief Read a word from the target device on the given SFP port.
 * @param id The SFP Port ID.
 * @param devaddr The device address.
 * @param addr The read address.
 * @returns The word if successful, ONLP_STATUS_E* on error.
 */
int onlp_sfpi_dev_readw(onlp_oid_id_t id, int devaddr, int addr)
{
    int local_id = ONLP_OID_ID_GET(id);
    int bus_id = -1;
    int ret = ONLP_STATUS_OK;
    int port_id;

    if (onlp_sfpi_is_present(id) != 1) {
        AIM_LOG_INFO("sfp module (port=%d) is absent.\n", local_id);
        return ONLP_STATUS_OK;
    }

    devaddr = 0x50;
    if (local_id < QSFP_PORT_NUM) {
        /* QSFP */
        port_id = local_id;
        bus_id = qsfp_port_eeprom_bus_id_array[port_id];
        ret = onlp_i2c_readw(bus_id, devaddr, addr, ONLP_I2C_F_FORCE);

    } else if (local_id >= QSFP_PORT_NUM && local_id < (QSFP_PORT_NUM + QSFPDD_PORT_NUM)) {
        /* QSFPDD */
        port_id = local_id - QSFP_PORT_NUM;
        bus_id = qsfpdd_port_eeprom_bus_id_array[port_id];
        ret = onlp_i2c_readw(bus_id, devaddr, addr, ONLP_I2C_F_FORCE);

    } else if (local_id >= (QSFP_PORT_NUM + QSFPDD_PORT_NUM) && local_id < ALL_PORT_NUM) {
        /* SFP */
        return ONLP_STATUS_E_UNSUPPORTED;

    } else {
        return ONLP_STATUS_E_PARAM;
    }

    return ret;
}

/**
 * @brief Write a word to the target device on the given SFP port.
 * @param id The SFP Port ID.
 * @param devaddr The device address.
 * @param addr The write address.
 * @param value The write value.
 */
int onlp_sfpi_dev_writew(onlp_oid_id_t id, int devaddr, int addr,
                         uint16_t value)
{
    int local_id = ONLP_OID_ID_GET(id);
    int bus_id = -1;
    int ret = ONLP_STATUS_OK;
    int port_id;

    if (onlp_sfpi_is_present(id) != 1) {
        AIM_LOG_INFO("sfp module (port=%d) is absent.\n", local_id);
        return ONLP_STATUS_OK;
    }

    devaddr = 0x50;
    if (local_id < QSFP_PORT_NUM) {
        /* QSFP */
        port_id = local_id;
        bus_id = qsfp_port_eeprom_bus_id_array[port_id];
        ret = onlp_i2c_writew(bus_id, devaddr, addr, value, ONLP_I2C_F_FORCE);

    } else if (local_id >= QSFP_PORT_NUM && local_id < (QSFP_PORT_NUM + QSFPDD_PORT_NUM)) {
        /* QSFPDD */
        port_id = local_id - QSFP_PORT_NUM;
        bus_id = qsfpdd_port_eeprom_bus_id_array[port_id];
        ONLP_TRY(onlp_i2c_writew(bus_id, devaddr, addr, value, ONLP_I2C_F_FORCE));

    } else if (local_id >= (QSFP_PORT_NUM + QSFPDD_PORT_NUM) && local_id < ALL_PORT_NUM) {
        /* SFP */
        return ONLP_STATUS_E_UNSUPPORTED;

    } else {
        return ONLP_STATUS_E_PARAM;
    }

    return ret;
}

/**
 * @brief Perform any actions required after an SFP is inserted.
 * @param id The SFP Port ID.
 * @param info The SFF Module information structure.
 * @note This function is optional. If your platform must
 * adjust equalizer or preemphasis settings internally then
 * this function should be implemented as the trigger.
 */
int onlp_sfpi_post_insert(onlp_oid_id_t id, sff_info_t* info)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

/**
 * @brief Returns whether or not the given control is supported on the given port.
 * @param id The SFP Port ID.
 * @param control The control.
 * @param[out] rv Receives 1 if supported, 0 if not supported.
 * @note This provided for convenience and is optional.
 * If you implement this function your control_set and control_get APIs
 * will not be called on unsupported ports.
 */
int onlp_sfpi_control_supported(onlp_oid_id_t id,
                                onlp_sfp_control_t control, int* rv)
{
    int local_id = ONLP_OID_ID_GET(id);
    int ret = ONLP_STATUS_OK;
    int port_type = 0;

    ret = onlp_sfpi_type_get(local_id, &port_type);    
    if (ret != ONLP_STATUS_OK) {
        return ret;
    }

    *rv = 0;
    switch (control) {
        case ONLP_SFP_CONTROL_RESET:
            if (port_type == ONLP_SFP_TYPE_QSFP28) {
                *rv = 1;
            }
            break;
        case ONLP_SFP_CONTROL_RESET_STATE:
            if (port_type == ONLP_SFP_TYPE_QSFP28) {
                *rv = 1;
            }
            break;
        case ONLP_SFP_CONTROL_RX_LOS:
            if (port_type == ONLP_SFP_TYPE_SFP) {
                *rv = 1;
            }
            break;
        case ONLP_SFP_CONTROL_TX_FAULT:
            if (port_type == ONLP_SFP_TYPE_SFP) {
                *rv = 1;
            }
            break;
        case ONLP_SFP_CONTROL_TX_DISABLE:
            if (port_type == ONLP_SFP_TYPE_SFP) {
                *rv = 1;
            }
            break;
        case ONLP_SFP_CONTROL_LP_MODE:
            if (port_type == ONLP_SFP_TYPE_QSFP28) {
                *rv = 1;
            }
            break;
        default:
            return ONLP_STATUS_E_UNSUPPORTED;
            break;

    }

    return ONLP_STATUS_OK;
}

/**
 * @brief Set an SFP control.
 * @param id The SFP Port ID.
 * @param control The control.
 * @param value The value.
 */
int onlp_sfpi_control_set(onlp_oid_id_t id, onlp_sfp_control_t control,
                          int value)
{
    int ret = ONLP_STATUS_OK;
    int local_id = ONLP_OID_ID_GET(id);
    int port_type = 0;
    int port_config_reg = 0;
    int port_id;

    ret = onlp_sfpi_type_get(id, &port_type);    
    if (ret != ONLP_STATUS_OK) {
        return ret;
    }

    switch (control) {
        case ONLP_SFP_CONTROL_RESET:
            if (port_type == ONLP_SFP_TYPE_QSFP28) {
                /**
                 * QSFP28/QSFPDD Reset Value
                 * return 0: the module is in reset
                 * return 1: the module is NOT in reset
                 */
                if (local_id < QSFP_PORT_NUM) { //QSFP
                    port_id = local_id;
                    ret = ufi_get_qsfp_port_config_reg(port_id, &port_config_reg);
                    if (ret != ONLP_STATUS_OK) {
                        AIM_LOG_ERROR("ufi_get_qsfp_port_config_reg() failed, port=%d\n", port_id);
                        return ret;
                    } else {
                        if (value == 0) {
                            port_config_reg = port_config_reg & 0b11111110;
                        } else if (value == 1) {
                            port_config_reg = port_config_reg | 0b00000001;
                        } else {
                            AIM_LOG_ERROR("unknown supported value, port=%d, value=%d, func=%s\n", port_id, value, __FUNCTION__);
                            return ONLP_STATUS_E_PARAM;
                        }

                        /* set value into qsfp port config register */
                        ret = ufi_set_qsfp_port_config_reg(port_id, port_config_reg);
                        if (ret != ONLP_STATUS_OK) {
                            AIM_LOG_ERROR("ufi_set_qsfp_port_config_reg() failed, port=%d, value=%d\n", port_id, port_config_reg);
                            return ret;
                        }
                    }
                } else if (local_id >= QSFP_PORT_NUM && local_id < (QSFP_PORT_NUM + QSFPDD_PORT_NUM)) { //QSFPDD
                    port_id = local_id - QSFP_PORT_NUM;
                    ret = ufi_get_qsfpdd_port_config_reg(port_id, &port_config_reg);
                    if (ret != ONLP_STATUS_OK) {
                        AIM_LOG_ERROR("ufi_get_qsfpdd_port_config_reg() failed, port=%d\n", port_id);
                        return ret;
                    } else {
                        if (value == 0) {
                            port_config_reg = port_config_reg & 0b11111110;
                        } else if (value == 1) {
                            port_config_reg = port_config_reg | 0b00000001;
                        } else {
                            AIM_LOG_ERROR("unknown supported value, port=%d, value=%d, func=%s\n", port_id, value, __FUNCTION__);
                            return ONLP_STATUS_E_PARAM;
                        }

                        /* set value into qsfp port config register */
                        ret = ufi_set_qsfpdd_port_config_reg(port_id, port_config_reg);
                        if (ret != ONLP_STATUS_OK) {
                            AIM_LOG_ERROR("ufi_set_qsfpdd_port_config_reg() failed, port=%d, value=%d\n", port_id, port_config_reg);
                            return ret;
                        }
                    }
                } else {
                    AIM_LOG_ERROR("unknown ports, local_id=%d, func=%s\n", local_id, __FUNCTION__);
                    return ONLP_STATUS_E_PARAM;
                }
            } else {
                //do nothing
                return ONLP_STATUS_OK;
            }
            break;

        case ONLP_SFP_CONTROL_TX_DISABLE:
            if (port_type == ONLP_SFP_TYPE_SFP) {
                /**
                 * SFP TX_Disable Value
                 * return 0: turn on
                 * return 1: turn off
                 */
                port_id = local_id - (QSFP_PORT_NUM + QSFPDD_PORT_NUM);
                ret = ufi_get_sfp_port_config_reg(port_id, &port_config_reg);
                if (ret != ONLP_STATUS_OK) {
                    AIM_LOG_ERROR("ufi_get_sfp_port_config_reg() failed, port=%d\n", port_id);
                    return ret;
                } else {
                    if (value == 0) {
                        if (port_id == (QSFP_PORT_NUM + QSFPDD_PORT_NUM)) { //SFP Port0
                            port_config_reg = port_config_reg & 0b11111110;
                        } else if (port_id == (QSFP_PORT_NUM + QSFPDD_PORT_NUM + 1)) { //SFP Port1
                            port_config_reg = port_config_reg & 0b11101111;
                        } else {
                            AIM_LOG_ERROR("unknown ports, port=%d, value=%d, func=%s\n", port_id, value, __FUNCTION__);
                            return ONLP_STATUS_E_PARAM;
                        }
                    } else if (value == 1) {
                        if (port_id == (QSFP_PORT_NUM + QSFPDD_PORT_NUM)) { //SFP Port0
                            port_config_reg = port_config_reg | 0b00000001;
                        } else if (port_id == (QSFP_PORT_NUM + QSFPDD_PORT_NUM + 1)) { //SFP Port1
                            port_config_reg = port_config_reg | 0b00010000;
                        } else {
                            AIM_LOG_ERROR("unknown ports, port=%d, value=%d, func=%s\n", port_id, value, __FUNCTION__);
                            return ONLP_STATUS_E_PARAM;
                        }
                    } else {
                        AIM_LOG_ERROR("unknown supported value, port=%d, value=%d, func=%s\n", port_id, value, __FUNCTION__);
                        return ONLP_STATUS_E_PARAM;
                    }

                    /* set value into sfp port config register */ 
                    ret = ufi_set_sfp_port_config_reg(port_id, port_config_reg);
                    if (ret != ONLP_STATUS_OK) {
                        AIM_LOG_ERROR("ufi_set_sfp_port_config_reg() failed, port=%d, value=%d\n", port_id, port_config_reg);
                        return ret;
                    }
                }
            } else {
                //do nothing
                return ONLP_STATUS_OK;
            }
            break;

        case ONLP_SFP_CONTROL_LP_MODE:
            if (port_type == ONLP_SFP_TYPE_QSFP28) {
                /**
                 * QSFP28/QSFPDD Low Power Mode Value
                 * return 0: normal power mode
                 * return 1: low power mode
                 */
                if (local_id < QSFP_PORT_NUM) { //QSFP
                    port_id = local_id;
                    ret = ufi_get_qsfp_port_config_reg(port_id, &port_config_reg);
                    if (ret != ONLP_STATUS_OK) {
                        AIM_LOG_ERROR("ufi_get_qsfp_port_config_reg() failed, port=%d\n", port_id);
                        return ret;
                    } else {
                        if (value == 0) {
                            port_config_reg = port_config_reg & 0b11111011;
                        } else if (value == 1) {
                            port_config_reg = port_config_reg | 0b00000100;
                        } else {
                            AIM_LOG_ERROR("unknown supported value, port=%d, value=%d, func=%s\n", port_id, value, __FUNCTION__);
                            return ONLP_STATUS_E_PARAM;
                        }

                        /* set value into qsfp port config register */
                        ret = ufi_set_qsfp_port_config_reg(port_id, port_config_reg);
                        if (ret != ONLP_STATUS_OK) {
                            AIM_LOG_ERROR("ufi_set_qsfp_port_config_reg() failed, port=%d, value=%d\n", port_id, port_config_reg);
                            return ret;
                        }
                    }
                } else if (local_id >= QSFP_PORT_NUM && local_id < (QSFP_PORT_NUM + QSFPDD_PORT_NUM)) { //QSFPDD
                    port_id = local_id - QSFP_PORT_NUM;
                    ret = ufi_get_qsfpdd_port_config_reg(port_id, &port_config_reg);
                    if (ret != ONLP_STATUS_OK) {
                        AIM_LOG_ERROR("ufi_get_qsfpdd_port_config_reg() failed, port=%d\n", port_id);
                        return ret;
                    } else {
                        if (value == 0) {
                            port_config_reg = port_config_reg & 0b11111011;
                        } else if (value == 1) {
                            port_config_reg = port_config_reg | 0b00000100;
                        } else {
                            AIM_LOG_ERROR("unknown supported value, port=%d, value=%d, func=%s\n", port_id, value, __FUNCTION__);
                            return ONLP_STATUS_E_PARAM;
                        }

                        /* set value into qsfp port config register */
                        ret = ufi_set_qsfpdd_port_config_reg(port_id, port_config_reg);
                        if (ret != ONLP_STATUS_OK) {
                            AIM_LOG_ERROR("ufi_set_qsfpdd_port_config_reg() failed, port=%d, value=%d\n", port_id, port_config_reg);
                            return ret;
                        }
                    }
                } else {
                    AIM_LOG_ERROR("unknown ports, local_id=%d, func=%s\n", local_id, __FUNCTION__);
                    return ONLP_STATUS_E_PARAM;
                }
            } else {
                //do nothing
                return ONLP_STATUS_OK;
            }
            break;

        default:
            //do nothing
            return ONLP_STATUS_OK;
            break;
    }

    return ONLP_STATUS_OK;
}

/**
 * @brief Get an SFP control.
 * @param id The SFP Port ID.
 * @param control The control
 * @param[out] value Receives the current value.
 */
int onlp_sfpi_control_get(onlp_oid_id_t id, onlp_sfp_control_t control,
                          int* value)
{
    int ret = ONLP_STATUS_OK;
    int local_id = ONLP_OID_ID_GET(id);
    int port_type = 0;
    int port_status_reg = 0;
    int port_config_reg = 0;
    int port_id;

    *value = 0;
    ret = onlp_sfpi_type_get(id, &port_type);    
    if (ret != ONLP_STATUS_OK) {
        return ret;
    }

    switch (control) {
        case ONLP_SFP_CONTROL_RESET_STATE:
            if (port_type == ONLP_SFP_TYPE_QSFP28) {
                /**
                 * QSFP28/QSFPDD Reset Value
                 * return 0: the module is in reset
                 * return 1: the module is NOT in reset
                 */
                if (local_id < QSFP_PORT_NUM) { //QSFP
                    port_id = local_id;
                    ret = ufi_get_qsfp_port_config_reg(port_id, &port_config_reg);
                    if (ret != ONLP_STATUS_OK) {
                        AIM_LOG_ERROR("ufi_get_qsfp_port_config_reg() failed, port=%d\n", port_id);
                        return ret;
                    } else {
                        *value = (port_config_reg & 0b00000001) >> 0;
                    }
                } else if (local_id >= QSFP_PORT_NUM && local_id < (QSFP_PORT_NUM + QSFPDD_PORT_NUM)) { //QSFPDD
                    port_id = local_id - QSFP_PORT_NUM;
                    ret = ufi_get_qsfpdd_port_config_reg(port_id, &port_config_reg);
                    if (ret != ONLP_STATUS_OK) {
                        AIM_LOG_ERROR("ufi_get_qsfpdd_port_config_reg() failed, port=%d\n", port_id);
                        return ret;
                    } else {
                        *value = (port_config_reg & 0b00000001) >> 0;
                    }
                } else {
                    AIM_LOG_ERROR("unknown ports, local=%d, func=%s\n", local_id, __FUNCTION__);
                    return ONLP_STATUS_E_PARAM;
                }
            } else {
                //do nothing
                return ONLP_STATUS_OK;
            }
            break;
        case ONLP_SFP_CONTROL_RX_LOS:
            if (port_type == ONLP_SFP_TYPE_SFP) {
                /**
                 * SFP RX_LOS Value
                 * return 0: link up
                 * return 1: link down (rx los)
                 */
                port_id = local_id - (QSFP_PORT_NUM + QSFPDD_PORT_NUM);
                ret = ufi_get_sfp_port_status_reg(port_id, &port_status_reg);
                if (ret != ONLP_STATUS_OK) {
                    AIM_LOG_ERROR("ufi_get_sfp_port_status_reg() failed, port=%d\n", port_id);
                    return ret;
                } else {
                    if (port_id == 0) { //SFP Port0
                        *value = (port_status_reg & 0b00000100) >> 2;
                    } else if (port_id == 1) { //SFP Port1
                        *value = (port_status_reg & 0b01000000) >> 6;
                    } else {
                        AIM_LOG_ERROR("unknown ports, local_id=%d, func=%s\n", local_id, __FUNCTION__);
                        return ONLP_STATUS_E_PARAM;
                    }
                }
            } else {
                //do nothing
                return ONLP_STATUS_OK;
            }
            break;
        case ONLP_SFP_CONTROL_TX_FAULT:
            if (port_type == ONLP_SFP_TYPE_SFP) {
                /**
                 * SFP TX_FAULT Value
                 * return 0: normal
                 * return 1: tx fault
                 */
                port_id = local_id - (QSFP_PORT_NUM + QSFPDD_PORT_NUM);
                ret = ufi_get_sfp_port_status_reg(port_id, &port_status_reg);
                if (ret != ONLP_STATUS_OK) {
                    AIM_LOG_ERROR("ufi_get_sfp_port_status_reg() failed, port=%d\n", port_id);
                    return ret;
                } else {
                    if (port_id == 0) { //SFP Port0
                        *value = (port_status_reg & 0b00000010) >> 1;
                    } else if (port_id == 1) { //SFP Port1
                        *value = (port_status_reg & 0b00100000) >> 5;
                    } else {
                        AIM_LOG_ERROR("unknown ports, local_id=%d, func=%s\n", local_id, __FUNCTION__);
                        return ONLP_STATUS_E_PARAM;
                    }
                }
            } else {
                //do nothing
                return ONLP_STATUS_OK;
            }
            break;
        case ONLP_SFP_CONTROL_TX_DISABLE:
            if (port_type == ONLP_SFP_TYPE_SFP) {
                /**
                 * SFP TX_Disable Value
                 * return 0: turn on
                 * return 1: turn off
                 */
                port_id = local_id - (QSFP_PORT_NUM + QSFPDD_PORT_NUM);
                ret = ufi_get_sfp_port_config_reg(port_id, &port_config_reg);
                if (ret != ONLP_STATUS_OK) {
                    AIM_LOG_ERROR("ufi_get_sfp_port_config_reg() failed, port=%d\n", port_id);
                    return ret;
                } else {
                    if (port_id == 0) { //SFP Port0
                        *value = (port_config_reg & 0b00000001) >> 0;
                    } else if (port_id == 1) { //SFP Port1
                        *value = (port_config_reg & 0b00010000) >> 4;
                    } else {
                        AIM_LOG_ERROR("unknown ports, local_id=%d, func=%s\n", local_id, __FUNCTION__);
                        return ONLP_STATUS_E_PARAM;
                    }
                }
            } else {
                //do nothing
                return ONLP_STATUS_OK;
            }
            break;
        case ONLP_SFP_CONTROL_LP_MODE:
            if (port_type == ONLP_SFP_TYPE_QSFP28) {
                /**
                 * QSFP28/QSFPDD Low Power Mode Value
                 * return 0: normal power mode
                 * return 1: low power mode
                 */
                if (local_id < QSFP_PORT_NUM) { //QSFP
                    port_id = local_id;
                    ret = ufi_get_qsfp_port_config_reg(port_id, &port_config_reg);
                    if (ret != ONLP_STATUS_OK) {
                        AIM_LOG_ERROR("ufi_get_qsfp_port_config_reg() failed, port=%d\n", port_id);
                        return ret;
                    } else {
                        *value = (port_config_reg & 0b00000100) >> 2;
                    }
                } else if (local_id >= QSFP_PORT_NUM && local_id < (QSFP_PORT_NUM + QSFPDD_PORT_NUM)) { //QSFPDD
                    port_id = local_id - QSFP_PORT_NUM;
                    ret = ufi_get_qsfpdd_port_config_reg(port_id, &port_config_reg);
                    if (ret != ONLP_STATUS_OK) {
                        AIM_LOG_ERROR("ufi_get_qsfpdd_port_config_reg() failed, port=%d\n", port_id);
                        return ret;
                    } else {
                        *value = (port_config_reg & 0b00000100) >> 2;
                    }
                } else {
                    AIM_LOG_ERROR("unknown ports, local_id=%d, func=%s\n", local_id, __FUNCTION__);
                    return ONLP_STATUS_E_PARAM;
                }
            } else {
                //do nothing
                return ONLP_STATUS_OK;
            }
            break;
        default:
            //do nothing
            return ONLP_STATUS_OK;
            break;
    }

    return ONLP_STATUS_OK;
}

/**
 * @brief Remap SFP user SFP port numbers before calling the SFPI interface.
 * @param id The SFP Port ID.
 * @param[out] rport Receives the new port.
 * @note This function will be called to remap the user SFP port number
 * to the number returned in rport before the SFPI functions are called.
 * This is an optional convenience for platforms with dynamic or
 * variant physical SFP numbering.
 */
int onlp_sfpi_port_map(onlp_oid_id_t id, int* rport)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

/**
 * @brief Get the SFP's OID header.
 * @param id The SFP oid.
 * @param [out] hdr Receives the header.
 */
int onlp_sfpi_hdr_get(onlp_oid_id_t id, onlp_oid_hdr_t* hdr)
{
    int local_id = ONLP_OID_ID_GET(id);
    int is_present = onlp_sfpi_is_present(id);

    hdr->id = ONLP_SFP_ID_CREATE(local_id);
    hdr->poid = ONLP_OID_CHASSIS;
    hdr->status = is_present ? ONLP_OID_STATUS_FLAG_PRESENT : ONLP_OID_STATUS_FLAG_UNPLUGGED;

    return ONLP_STATUS_OK;
}

