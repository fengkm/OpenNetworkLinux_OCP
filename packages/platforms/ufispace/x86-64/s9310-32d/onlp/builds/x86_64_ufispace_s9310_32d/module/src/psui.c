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
 *
 *
 ***********************************************************/
#include <onlp/platformi/psui.h>
#include <onlplib/file.h>

#include "platform_lib.h"

static onlp_psu_info_t pinfo[] =
{
    { }, /* Not used */
    {
        {
            PSU_OID_PSU0,
            "PSU-1",
            0,
            {
                FAN_OID_PSU0_FAN1,
                FAN_OID_PSU0_FAN2,
                THERMAL_OID_PSU0,
            },
        }
    },
    {
        {
            PSU_OID_PSU1,
            "PSU-2",
            0,
            {
                FAN_OID_PSU1_FAN1,            
                FAN_OID_PSU1_FAN2,
                THERMAL_OID_PSU1,
            },
        }
    }
};

int
onlp_psui_init(void)
{   
    lock_init();
    return ONLP_STATUS_OK;
}

#if 0
int 
psu_status_info_get(int id, onlp_psu_info_t *info)
{   
    int rc, pw_present, pw_good;

    static int cache_rc[PSU_ID_MAX];
    static int cache_cycle[PSU_ID_MAX];
    static int cache_miin[PSU_ID_MAX];
    static int cache_miout[PSU_ID_MAX];
    static int cache_mvin[PSU_ID_MAX];
    static int cache_mvout[PSU_ID_MAX];
    static int cache_stbmvout[PSU_ID_MAX];
    static int cache_stbmiout[PSU_ID_MAX];
    static int cache_caps[PSU_ID_MAX];
    static char cache_model[PSU_ID_MAX][ONLP_CONFIG_INFO_STR_MAX]; 
    static char cache_serial[PSU_ID_MAX][ONLP_CONFIG_INFO_STR_MAX]; 
  
    ++cache_cycle[id];
    cache_cycle[id] %= BMC_CACHE_CYCLE;
    int stbmvout, stbmiout;
    
     /* Get power present status */
    if ((rc = psu_present_get(&pw_present, id))
            != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }

    if (pw_present != PSU_STATUS_PRESENT) {
        info->status &= ~ONLP_PSU_STATUS_PRESENT;
        info->status |=  ONLP_PSU_STATUS_UNPLUGGED;
        return ONLP_STATUS_OK;
    }

    info->status |= ONLP_PSU_STATUS_PRESENT;

    /* Get power good status */
    if ((rc = psu_pwgood_get(&pw_good, id))
            != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }

    if (pw_good != PSU_STATUS_POWER_GOOD) {
        info->status |= ONLP_PSU_STATUS_FAILED;
    } else {
        info->status &= ~ONLP_PSU_STATUS_FAILED;
    }

    if (cache_cycle[id] == 1) { 
        
        /* Get power iin status */
        if ((rc = psu_iin_get(info, id)) != ONLP_STATUS_OK) {            
            cache_rc[id] = ONLP_STATUS_E_INTERNAL;
            return ONLP_STATUS_E_INTERNAL;
        } else {
            //write cache
            cache_rc[id] = rc;
            cache_miin[id] = info->miin;
            cache_caps[id] = info->caps;
        }
        
        /* Get power iout status */
        if ((rc = psu_iout_get(info, id)) != ONLP_STATUS_OK) {
            cache_rc[id] = ONLP_STATUS_E_INTERNAL;
            return ONLP_STATUS_E_INTERNAL;
        } else {
            //write cache
            cache_rc[id] = rc;
            cache_miout[id] = info->miout;
            cache_caps[id] = info->caps;
        }       
        
        /* Get power vin status */
        if ((rc = psu_vin_get(info, id)) != ONLP_STATUS_OK) {
            cache_rc[id] = ONLP_STATUS_E_INTERNAL;
            return ONLP_STATUS_E_INTERNAL;
        } else {
            //write cache
            cache_rc[id] = rc;
            cache_mvin[id] = info->mvin;
            cache_caps[id] = info->caps;
        }

        /* Get power vout status */
        if ((rc = psu_vout_get(info, id)) != ONLP_STATUS_OK) {
            cache_rc[id] = ONLP_STATUS_E_INTERNAL;
            return ONLP_STATUS_E_INTERNAL;
        } else {
            //write cache
            cache_mvout[id] = info->mvout;
            cache_caps[id] = info->caps;
        }

        /* Get standby power vout */
        if ((rc = psu_stbvout_get(&stbmvout, id)) != ONLP_STATUS_OK) {
            cache_rc[id] = ONLP_STATUS_E_INTERNAL;
            return ONLP_STATUS_E_INTERNAL;
        } else {
            //write cache
            cache_stbmvout[id] = stbmvout;
        }

        /* Get standby power iout */
        if ((rc = psu_stbiout_get(&stbmiout, id)) != ONLP_STATUS_OK) {
            cache_rc[id] = ONLP_STATUS_E_INTERNAL;
            return ONLP_STATUS_E_INTERNAL;
        } else {
            //write cache
            cache_stbmiout[id] = stbmiout;
        }

        /* Get power in and out */
        info->mpin = info->miin * info->mvin / 1000;
        info->mpout = (info->miout * info->mvout + stbmiout * stbmvout) / 1000;        
        info->caps |= ONLP_PSU_CAPS_PIN | ONLP_PSU_CAPS_POUT;
        cache_caps[id] = info->caps;
        
        /* Get FRU (model/serial) */
        if ((rc = psu_fru_get(info, id)) != ONLP_STATUS_OK) {
            cache_rc[id] = ONLP_STATUS_E_INTERNAL;
            return ONLP_STATUS_E_INTERNAL;
        } else {
            //write cache
            //cache_rc[id] = rc;

            memset(cache_model[id], 0, sizeof(cache_model[id]));
            memset(cache_serial[id], 0, sizeof(cache_serial[id]));

            snprintf(cache_model[id], sizeof(cache_model[id]), "%s", info->model);
            snprintf(cache_serial[id], sizeof(cache_serial[id]), "%s", info->serial);
        }
        
        //write cache
        cache_rc[id] = ONLP_STATUS_OK; 
        return ONLP_STATUS_OK;
    } else { 
    
        //read cache
        rc = cache_rc[id];
        info->caps = cache_caps[id];
        info->miin = cache_miin[id];
        info->miout = cache_miout[id];
        info->mvin = cache_mvin[id];
        info->mvout = cache_mvout[id];
        info->mpin = cache_miin[id] * cache_mvin[id] / 1000;
        info->mpout = (cache_miout[id] * cache_mvout[id] + cache_stbmiout[id] * cache_stbmvout[id])/ 1000;
        memset(info->model, 0, sizeof(info->model));
        memset(info->serial, 0, sizeof(info->serial));
        snprintf(info->model, sizeof(info->model), "%s", cache_model[id]);
        snprintf(info->serial, sizeof(info->serial), "%s", cache_serial[id]);
        return rc;
    }
}
#endif

int 
psu_status_info_get(int id, onlp_psu_info_t *info)
{   
    int rc, pw_present, pw_good;    
    int stbmvout, stbmiout;
    float data;
    int index_offset = 0;

    if (id == PSU_ID_PSU1) {
        index_offset = 5;
    }
    
     /* Get power present status */
    if ((rc = psu_present_get(&pw_present, id))
            != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }

    if (pw_present != PSU_STATUS_PRESENT) {
        info->status &= ~ONLP_PSU_STATUS_PRESENT;
        info->status |=  ONLP_PSU_STATUS_UNPLUGGED;
        return ONLP_STATUS_OK;
    }

    info->status |= ONLP_PSU_STATUS_PRESENT;

    /* Get power good status */
    if ((rc = psu_pwgood_get(&pw_good, id))
            != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    }

    if (pw_good != PSU_STATUS_POWER_GOOD) {
        info->status |= ONLP_PSU_STATUS_FAILED;
    } else {
        info->status &= ~ONLP_PSU_STATUS_FAILED;
    }

    /* Get power vin status */
    if ((rc = bmc_sensor_read(id + 19 + index_offset, PSU_SENSOR, &data)) != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    } else {        
        info->mvin = (int) (data*1000);
        info->caps |= ONLP_PSU_CAPS_VIN;
    }
    
    /* Get power vout status */
    if ((rc = bmc_sensor_read(id + 20 + index_offset, PSU_SENSOR, &data)) != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    } else {        
        info->mvout = (int) (data*1000);
        info->caps |= ONLP_PSU_CAPS_VOUT;
    }
            
    /* Get power iin status */
    if ((rc = bmc_sensor_read(id + 21 + index_offset, PSU_SENSOR, &data)) != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    } else {        
        info->miin = (int) (data*1000);
        info->caps |= ONLP_PSU_CAPS_IIN;
    }
    
    /* Get power iout status */
    if ((rc = bmc_sensor_read(id + 22 + index_offset, PSU_SENSOR, &data)) != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    } else {        
        info->miout = (int) (data*1000);        
        info->caps |= ONLP_PSU_CAPS_IOUT;
    }    

    /* Get standby power vout */    
    if ((rc = bmc_sensor_read(id + 23 + index_offset, PSU_SENSOR, &data)) != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    } else {
        stbmvout = (int) (data*1000);        
    }
    
    /* Get standby power iout */
    if ((rc = bmc_sensor_read(id + 24 + index_offset, PSU_SENSOR, &data)) != ONLP_STATUS_OK) {
        return ONLP_STATUS_E_INTERNAL;
    } else {
        stbmiout = (int) (data*1000);        
    }
    
    /* Get power in and out */
    info->mpin = info->miin * info->mvin / 1000;
    info->mpout = (info->miout * info->mvout + stbmiout * stbmvout) / 1000;        
    info->caps |= ONLP_PSU_CAPS_PIN | ONLP_PSU_CAPS_POUT;
    
    /* Get FRU (model/serial) */
    if ((rc = psu_fru_get(info, id)) != ONLP_STATUS_OK) {        
        return ONLP_STATUS_E_INTERNAL;
    }
    
    return ONLP_STATUS_OK;
}

int
onlp_psui_info_get(onlp_oid_t id, onlp_psu_info_t* info)
{        
    int pid;
    
    pid = ONLP_OID_ID_GET(id);
    memset(info, 0, sizeof(onlp_psu_info_t));

    /* Set the onlp_oid_hdr_t */
    *info = pinfo[pid];

    switch (pid) {
        case PSU_ID_PSU0:
        case PSU_ID_PSU1:
            return psu_status_info_get(pid, info);
            break;
        default:
            return ONLP_STATUS_E_UNSUPPORTED;
            break;
    }

    return ONLP_STATUS_OK;
}
