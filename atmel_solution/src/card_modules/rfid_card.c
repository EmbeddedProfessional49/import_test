#include <rfid_card.h>
#include <CardModule1.h>

static u32 rfid_init (RFID_MODULE_API *module) {
    
    if (module->module_ops->init != 0x00) {
        module->module_ops->init ();
    }
    
    return 0;
}

static RFID_MODULE_API CardModule1_api = {
    .init = rfid_init,
};

u32 Get_RFID_Card (RFID_MODULE_API *module, u32 module_num) {

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
    CardModule1_api.module_ops = Get_CardModule1_Ops();
}

