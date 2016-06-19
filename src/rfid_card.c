#include <rfid_card.h>

#include <rfid_card.h>
#include <Card1.h>

u32 Get_RFID_Card (RFID_MODULE_OPS *module_ops, u32 module_num) {

    switch (module_num) {

        case 0:
        {
            module_ops = Get_Card1_Ops ();
        }
        break;

        default:
            break;
    }

    return 0;
}

