#include <onlp/platformi/chassisi.h>
#include "platform_lib.h"

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
