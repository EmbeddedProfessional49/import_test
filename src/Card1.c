#include <rfid_card_pvt.h>
#include <Card1.h>

RFID_MODULE Card_1 = {
    .cfg.uart_cfg = {
        .uartnumber = 1,
        .baudrate = 1152000,
    },
    .ops = {
        .init = Card1_init,
    }
};


u32 Card1_init (void) {

    // module init code

    return 0;
}

RFID_MODULE_OPS * Get_Card1_Ops (void) {

    return &Card_1.ops;
}



