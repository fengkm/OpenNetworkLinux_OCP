#include <onlp/platformi/attributei.h>
#include <onlp/stdattrs.h>
#include <onlplib/file.h>

int
onlp_attributei_onie_info_get(onlp_oid_t oid, onlp_onie_info_t* rp)
{
    if(oid != ONLP_OID_CHASSIS) {
        return ONLP_STATUS_E_UNSUPPORTED;
    }

    if(rp == NULL) {
        return ONLP_STATUS_OK;
    }

    //return onlp_onie_decode_file(rp, IDPROM_PATH);
    return ONLP_STATUS_E_UNSUPPORTED;
}

int
onlp_attributei_asset_info_get(onlp_oid_t oid, onlp_asset_info_t* rp)
{
    if(oid != ONLP_OID_CHASSIS) {
        return ONLP_STATUS_E_UNSUPPORTED;
    }

    if(rp == NULL) {
        return ONLP_STATUS_OK;
    }

    rp->oid = oid;
    rp->firmware_revision = aim_strdup("fixme");
    return ONLP_STATUS_OK;
}
