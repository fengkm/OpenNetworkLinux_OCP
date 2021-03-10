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

/** onlp_sfp_type */
typedef enum ufi_sfp_type_e {
    ONLP_SFP_TYPE_SFP,
    ONLP_SFP_TYPE_QSFP,
    ONLP_SFP_TYPE_SFP28,
    ONLP_SFP_TYPE_QSFP28,
    ONLP_SFP_TYPE_LAST = ONLP_SFP_TYPE_QSFP28,
    ONLP_SFP_TYPE_COUNT,
    ONLP_SFP_TYPE_INVALID = -1,
} ufi_sfp_type_t;


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
        if (port_reset == 0) {
            port_reset = 1;
        } else if (port_reset == 1) {
            port_reset = 0;
        } else {
            AIM_LOG_ERROR("unexpected value, port_reset=%d, func=%s\n", port_reset, __FUNCTION__);
            return ONLP_STATUS_E_INTERNAL;
        }
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
 * @brief Determine the SFP connector type.
 * @param id The SFP Port ID.
 * @param[out] rtype Receives the connector type.
 */
static int onlp_sfpi_type_get(int local_id, ufi_sfp_type_t* rtype)
{
    if (local_id < QSFP_PORT_NUM) {
        *rtype = ONLP_SFP_TYPE_QSFP28;
    } else {
        AIM_LOG_ERROR("unknown ports, port=%d, func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_PARAM;
    }   

    return ONLP_STATUS_OK;
}

/**
 * @brief Initialize the SFPI subsystem.
 */
int onlp_sfpi_init(void)
{
    return ONLP_STATUS_OK;
}

/**
 * @brief Get the bitmap of SFP-capable port numbers.
 * @param bmap [out] Receives the bitmap.
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
 * @brief Determine if an SFP is present.
 * @param port The port number.
 * @returns 1 if present
 * @returns 0 if absent
 * @returns An error condition.
 */
int onlp_sfpi_is_present(int port)
{
    int local_id = port;
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
 * @param dst Receives the presence bitmap.
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
 * @param dst Receives the RX_LOS bitmap.
 */
int onlp_sfpi_rx_los_bitmap_get(onlp_sfp_bitmap_t* dst)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

/**
 * @brief Read the SFP EEPROM.
 * @param port The port number.
 * @param data Receives the SFP data.
 */
int onlp_sfpi_eeprom_read(int port, uint8_t data[256])
{
    int local_id = port;
    int bus_id = -1; 
    int ret = ONLP_STATUS_OK;
    int port_id;
    char eeprom_path[128];
    int size = 0;

    memset(data, 0, 256);
    memset(eeprom_path, 0, sizeof(eeprom_path));
    if (onlp_sfpi_is_present(port) != 1) {
        AIM_LOG_INFO("sfp module (port=%d) is absent.\n", local_id);
        return ONLP_STATUS_OK;
    }

    if (local_id < QSFP_PORT_NUM) { //QSFP
        port_id = local_id;
        bus_id = qsfp_port_eeprom_bus_id_array[port_id];
        snprintf(eeprom_path, sizeof(eeprom_path), "/sys/bus/i2c/devices/%d-0050/eeprom", bus_id);
        ONLP_TRY(onlp_file_read(data, 256, &size, eeprom_path));
    } else {
        AIM_LOG_ERROR("unknown ports, local_id=%d, func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_PARAM;
    }   

    return ret;
}

/**
 * @brief Read a byte from an address on the given SFP port's bus.
 * @param port The port number.
 * @param devaddr The device address.
 * @param addr The address.
 */
int onlp_sfpi_dev_readb(int port, uint8_t devaddr, uint8_t addr) 
{
    int local_id = port;
    int bus_id = -1; 
    int ret = ONLP_STATUS_OK;
    int port_id;

    if (onlp_sfpi_is_present(port) != 1) {
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
 * @brief Write a byte to an address on the given SFP port's bus.
 */
int onlp_sfpi_dev_writeb(int port, uint8_t devaddr, uint8_t addr, uint8_t value)
{
    int local_id = port;
    int bus_id = -1;
    int ret = ONLP_STATUS_OK;
    int port_id;

    if (onlp_sfpi_is_present(port) != 1) {
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
 * @brief Read a word from an address on the given SFP port's bus.
 * @param port The port number.
 * @param devaddr The device address.
 * @param addr The address.
 * @returns The word if successful, error otherwise.
 */
int onlp_sfpi_dev_readw(int port, uint8_t devaddr, uint8_t addr)
{
    int local_id = port;
    int bus_id = -1;
    int ret = ONLP_STATUS_OK;
    int port_id;

    if (onlp_sfpi_is_present(port) != 1) {
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
 * @brief Write a word to an address on the given SFP port's bus.
 */
int onlp_sfpi_dev_writew(int port, uint8_t devaddr, uint8_t addr, uint16_t value)
{
    int local_id = port;
    int bus_id = -1;
    int ret = ONLP_STATUS_OK;
    int port_id;

    if (onlp_sfpi_is_present(port) != 1) {
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
 * @brief Read from an address on the given SFP port's bus.
 * @param port The port number.
 * @param devaddr The device address.
 * @param addr The address.
 * @returns The data if successful, error otherwise.
 */
int onlp_sfpi_dev_read(int port, uint8_t devaddr, uint8_t addr, uint8_t* rdata, int size)
{
    int local_id = port;
    int bus_id = -1;
    int port_id;

    if (onlp_sfpi_is_present(port) != 1) {
        AIM_LOG_INFO("sfp module (port=%d) is absent.\n", local_id);
        return ONLP_STATUS_OK;
    }

    devaddr = 0x50;
    if (local_id < QSFP_PORT_NUM) {
        /* QSFP */
        port_id = local_id;
        bus_id = qsfp_port_eeprom_bus_id_array[port_id];
        ONLP_TRY(onlp_i2c_block_read(bus_id, devaddr, addr, size, rdata, ONLP_I2C_F_FORCE));

    } else {
        AIM_LOG_ERROR("unknown ports, local_id=%d, func=%s\n", local_id, __FUNCTION__);
        return ONLP_STATUS_E_PARAM;
    }

    return ONLP_STATUS_OK;
}

/**
 * @brief Write to an address on the given SFP port's bus.
 */
int onlp_sfpi_dev_write(int port, uint8_t devaddr, uint8_t addr, uint8_t* data, int size)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

/**
 * @brief Read the SFP DOM EEPROM.
 * @param port The port number.
 * @param data Receives the SFP data.
 */
int onlp_sfpi_dom_read(int port, uint8_t data[256])
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

/**
 * @brief Perform any actions required after an SFP is inserted.
 * @param port The port number.
 * @param info The SFF Module information structure.
 * @notes Optional
 */
int onlp_sfpi_post_insert(int port, sff_info_t* info)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

/**
 * @brief Returns whether or not the given control is suppport on the given port.
 * @param port The port number.
 * @param control The control.
 * @param rv [out] Receives 1 if supported, 0 if not supported.
 * @note This provided for convenience and is optional.
 * If you implement this function your control_set and control_get APIs
 * will not be called on unsupported ports.
 */
int onlp_sfpi_control_supported(int port, onlp_sfp_control_t control, int* rv)
{
    int local_id = port;
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
 * @param port The port.
 * @param control The control.
 * @param value The value.
 */
int onlp_sfpi_control_set(int port, onlp_sfp_control_t control, int value)
{
    int ret = ONLP_STATUS_OK;
    int local_id = port;
    int port_type = 0;
    int port_id;

    ret = onlp_sfpi_type_get(local_id, &port_type);
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
 * @param port The port.
 * @param control The control
 * @param [out] value Receives the current value.
 */
int onlp_sfpi_control_get(int port, onlp_sfp_control_t control, int* value)
{
    int ret = ONLP_STATUS_OK;
    int local_id = port;
    int port_type = 0;
    int port_reset = 0;
    int port_lpmode = 0;
    int port_id;

    *value = 0;
    ret = onlp_sfpi_type_get(local_id, &port_type);
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
 * @param port The user SFP port number.
 * @param [out] rport Receives the new port.
 * @note This function will be called to remap the user SFP port number
 * to the number returned in rport before the SFPI functions are called.
 * This is an optional convenience for platforms with dynamic or
 * variant physical SFP numbering.
 */
int onlp_sfpi_port_map(int port, int* rport)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

/**
 * @brief Deinitialize the SFP driver.
 */
int onlp_sfpi_denit(void)
{
    return ONLP_STATUS_OK;
}

/**
 * @brief Generic debug status information.
 * @param port The port number.
 * @param pvs The output pvs.
 * @notes The purpose of this vector is to allow reporting of internal debug
 * status and information from the platform driver that might be used to debug
 * SFP runtime issues.
 * For example, internal equalizer settings, tuning status information, status
 * of additional signals useful for system debug but not exposed in this interface.
 *
 * @notes This is function is optional.
 */
void onlp_sfpi_debug(int port, aim_pvs_t* pvs)
{
}

/**
 * @brief Generic ioctl
 * @param port The port number
 * @param The variable argument list of parameters.
 *
 * @notes This generic ioctl interface can be used
 * for platform-specific or driver specific features
 * that cannot or have not yet been defined in this
 * interface. It is intended as a future feature expansion
 * support mechanism.
 *
 * @notes Optional
 */
int onlp_sfpi_ioctl(int port, va_list vargs)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

