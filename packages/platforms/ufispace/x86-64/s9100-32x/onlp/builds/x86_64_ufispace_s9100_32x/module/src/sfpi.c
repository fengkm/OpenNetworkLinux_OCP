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
 * SFP Implementation
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

#define QSFP_PORT_NUM  32
#define ALL_PORT_NUM   32

static int qsfp_port_status_gpio_index_array[QSFP_PORT_NUM] = { 497, 496, 499, 498, 501, \
                                                                500, 503, 502, 505, 504, \
                                                                507, 506, 509, 508, 511, \
                                                                510, 481, 480, 483, 482, \
                                                                485, 484, 487, 486, 489, \
                                                                488, 491, 490, 493, 492, \
                                                                495, 494 };

static int qsfp_port_lpmode_gpio_index_array[QSFP_PORT_NUM] = { 433, 432, 435, 434, 437, \
                                                                436, 439, 438, 441, 440, \
                                                                443, 442, 445, 444, 447, \
                                                                446, 417, 416, 419, 418, \
                                                                421, 420, 423, 422, 425, \
                                                                424, 427, 426, 429, 428, \
                                                                431, 430 };

static int qsfp_port_reset_gpio_index_array[QSFP_PORT_NUM] = { 401, 400, 403, 402, 405, \
                                                               404, 407, 406, 409, 408, \
                                                               411, 410, 413, 412, 415, \
                                                               414, 385, 384, 387, 386, \
                                                               389, 388, 391, 390, 393, \
                                                               392, 395, 394, 397, 396, \
                                                               399, 398 };

static int qsfp_port_eeprom_bus_id_array[QSFP_PORT_NUM] = { 11, 10, 13, 12, 15, \
                                                            14, 17, 16, 19, 18, \
                                                            21, 20, 23, 22, 25, \
                                                            24, 27, 26, 29, 28, \
                                                            31, 30, 33, 32, 35, \
                                                            34, 37, 36, 39, 38, \
                                                            41, 40 };

/**
 * @brief get qsfp port presnet status
 * @param local_id: qsfp port id
 * @return:
 *    0: absence
 *    1: presence
 *   <0: error code
 */
static int get_qsfp_port_present_status(int local_id)
{
    int port_presence = 0;
    int port_id;

    if (local_id < QSFP_PORT_NUM) {
        /* QSFP Port */
        port_id = local_id;
        ONLP_TRY(onlp_file_read_int(&port_presence, "/sys/class/gpio/gpio%d/value", 
                    qsfp_port_status_gpio_index_array[port_id]));
    } else { 
        /* unkonwn ports */
        AIM_LOG_ERROR("unknown ports, local_id=%d, func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_UNSUPPORTED;
    }

    return port_presence;
}

/**
 * @brief get qsfp port low power mode status
 * @param local_id: qsfp port id
 * @return:
 *    0: normal power mode
 *    1: low power mode
 *   <0: error code
 */
static int get_qsfp_port_lpmode_status(int local_id)
{
    int port_lpmode = 0;
    int port_id;

    if (local_id < QSFP_PORT_NUM) {
        /* QSFP Port */
        port_id = local_id;
        ONLP_TRY(onlp_file_read_int(&port_lpmode, "/sys/class/gpio/gpio%d/value", 
                    qsfp_port_lpmode_gpio_index_array[port_id]));
    } else { 
        /* unkonwn ports */
        AIM_LOG_ERROR("unknown ports, local_id=%d, func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_UNSUPPORTED;
    }

    return port_lpmode;
}


/**
 * @brief set qsfp port low power mode status
 * @param local_id: qsfp port id
 * @param value: low power mode value (0 or 1)
 *    0: normal power mode
 *    1: low power mode
 */
static int set_qsfp_port_lpmode_status(int local_id, int value)
{
    int port_id;

    if (value != 0 && value != 1) {
        AIM_LOG_ERROR("unsupported value (%d) for QSFP port low power mode\n", value);
        return ONLP_STATUS_E_PARAM;
    }

    if (local_id < QSFP_PORT_NUM) {
        /* QSFP Port */
        port_id = local_id;
        ONLP_TRY(onlp_file_write_int(value, "/sys/class/gpio/gpio%d/value", 
                    qsfp_port_lpmode_gpio_index_array[port_id]));
    } else { 
        /* unkonwn ports */
        AIM_LOG_ERROR("unknown ports, local_id=%d, func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_UNSUPPORTED;
    }

    return ONLP_STATUS_OK;
}



/**
 * @brief get qsfp port reset status
 * @param local_id: qsfp port id
 * @return:
 *    0: the module is in reset
 *    1: the module is NOT in reset
 *   <0: error code
 */
static int get_qsfp_port_reset_status(int local_id)
{
    int port_reset = 0;
    int port_id;

    if (local_id < QSFP_PORT_NUM) {
        /* QSFP Port */
        port_id = local_id;
        ONLP_TRY(onlp_file_read_int(&port_reset, "/sys/class/gpio/gpio%d/value", 
                    qsfp_port_reset_gpio_index_array[port_id]));
        port_reset = ~port_reset;
    } else { 
        /* unkonwn ports */
        AIM_LOG_ERROR("unknown ports, local_id=%d, func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_UNSUPPORTED;
    }

    return port_reset;
}

/**
 * @brief set qsfp port reset status
 * @param local_id: qsfp port id
 * @param value: reset value (0 or 1)
 *    0: the module is in reset
 *    1: the module is NOT in reset
 */
static int set_qsfp_port_reset_status(int local_id, int value)
{
    int port_id;

    if (value != 0 && value != 1) {
        AIM_LOG_ERROR("unsupported value (%d) for QSFP port reset\n", value);
        return ONLP_STATUS_E_PARAM;
    }

    if (local_id < QSFP_PORT_NUM) {
        /* QSFP Port */
        port_id = local_id;
        ONLP_TRY(onlp_file_write_int(value, "/sys/class/gpio/gpio%d/value", 
                    qsfp_port_reset_gpio_index_array[port_id]));
    } else { 
        /* unkonwn ports */
        AIM_LOG_ERROR("unknown ports, local_id=%d, func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_UNSUPPORTED;
    }

    return ONLP_STATUS_OK;
}


/**
 * @brief Software initialization of the SFP module.
 */
int onlp_sfpi_sw_init(void)
{
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

    if (local_id < QSFP_PORT_NUM) {
        *rtype = ONLP_SFP_TYPE_QSFP28;
    } else {
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

    if (local_id < QSFP_PORT_NUM) {
        status = get_qsfp_port_present_status(local_id);
    } else {
        AIM_LOG_ERROR("unknown ports, local_id=%d, func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_UNSUPPORTED;
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
    return ONLP_STATUS_E_UNSUPPORTED;
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

    } else {
        AIM_LOG_ERROR("unknown ports, local_id=%d, func=%s\n", local_id, __FUNCTION__);
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
    if (local_id < QSFP_PORT_NUM) { //QSFP
        port_id = local_id;
        bus_id = qsfp_port_eeprom_bus_id_array[port_id];
        ret = onlp_i2c_readb(bus_id, devaddr, addr, ONLP_I2C_F_FORCE);
    } else {
        AIM_LOG_ERROR("unknown ports, local_id=%d, func=%s\n", local_id, __FUNCTION__);
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

                    /* set value into qsfp port config register */
                    ret = set_qsfp_port_reset_status(port_id, value);
                    if (ret != ONLP_STATUS_OK) {
                        AIM_LOG_ERROR("set_qsfp_port_reset_status() failed, port=%d, value=%d\n", port_id, value);
                        return ret;
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

        case ONLP_SFP_CONTROL_LP_MODE:
            if (port_type == ONLP_SFP_TYPE_QSFP28) {
                /**
                 * QSFP28/QSFPDD Low Power Mode Value
                 * return 0: normal power mode
                 * return 1: low power mode
                 */
                if (local_id < QSFP_PORT_NUM) { //QSFP
                    port_id = local_id;

                    /* set value into qsfp port config register */
                    ret = set_qsfp_port_lpmode_status(port_id, value);
                    if (ret != ONLP_STATUS_OK) {
                        AIM_LOG_ERROR("set_qsfp_port_lpmode_status() failed, port=%d, value=%d\n", port_id, value);
                        return ret;
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
    int port_reset = 0;
    int port_lpmode = 0;
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
                if (local_id < QSFP_PORT_NUM) {
                    /* QSFP */
                    port_id = local_id;
                    port_reset = get_qsfp_port_reset_status(port_id);
                    if (port_reset < 0) {
                        AIM_LOG_ERROR("get_qsfp_port_reset_status() failed, port=%d\n", port_id);
                        return port_reset;
                    } else {
                        *value = port_reset;
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

        case ONLP_SFP_CONTROL_LP_MODE:
            if (port_type == ONLP_SFP_TYPE_QSFP28) {
                /**
                 * QSFP28/QSFPDD Low Power Mode Value
                 * return 0: normal power mode
                 * return 1: low power mode
                 */
                if (local_id < QSFP_PORT_NUM) { //QSFP
                    port_id = local_id;
                    port_lpmode = get_qsfp_port_lpmode_status(port_id);
                    if (port_lpmode < 0) {
                        AIM_LOG_ERROR("get_qsfp_port_lpmode_status() failed, port=%d\n", port_id);
                        return port_lpmode;
                    } else {
                        *value = port_lpmode;
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

