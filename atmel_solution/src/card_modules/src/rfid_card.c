#include <rfid_card.h>
#include <sl025x.h>

static RFID_STATUS rfid_init (RFID_MODULE_API *pthis);
static RFID_STATUS rfid_uid (RFID_MODULE_API *pthis, RFID_UID_DATA *uid_data);

static RFID_MODULE_API CardModule1_api = {
    .init = rfid_init,
    .uid = rfid_uid,
};

static RFID_STATUS rfid_init (RFID_MODULE_API *pthis) {
    
    if (pthis ->module_ops ->init != 0x00) {
        pthis ->module_ops ->current_module = pthis ->module_ops ->pthis;
        pthis ->module_ops ->init ();
    }
    
    return 0;
}

static RFID_STATUS rfid_uid (RFID_MODULE_API *pthis, RFID_UID_DATA *uid_data) {
    
    if (pthis ->module_ops ->uid != 0x00) {
        pthis ->module_ops ->current_module = pthis ->module_ops ->pthis;
        pthis ->module_ops ->uid (uid_data);
    }
    
    return 0;
}

RFID_STATUS Get_RFID_Card (RFID_MODULE_API *module, u32 module_num) {

    switch (module_num) {

        case 0:
        {
            module = &CardModule1_api;
        }
        break;

        default:
            break;
    }

    return 0;
}

void Load_RFID_Modules (void) {
    CardModule1_api.module_ops = Get_Sl025x_Ops ();
}

