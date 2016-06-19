#include <rfid_card.h>

#include <rfid_card_pvt.h>
#include <Card1.h>

RFID_MODULE Card_1 = {
    .cfg.uart_cfg = {
        .uartnumber = 1,
        .baudrate = 1152000,
        },
    .ops = {
        .init = Card_1_init,
        }
};

u32 Init_RFID_Card (RFID_MODULE_OPS *module_ops, u32 module_num) {

    return 0;
}


u32 Card_1_init (RFID_MODULE_OPS *module) {

    return 0;
}

