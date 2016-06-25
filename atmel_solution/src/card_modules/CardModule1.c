#include <rfid_card_pvt.h>
#include <CardModule1.h>

static RFID_MODULE Card_1 = {
    .cfg.uart_cfg = {
        .uartnumber = 1,
        .baudrate = 1152000,
    },
    .ops = {
        .init = CardModule1_init,
        .enable = NULL,
        .disable = NULL,
        .uid = NULL,
    }
};


u32 CardModule1_init (void) {

    // module init code

    return 0;
}

RFID_MODULE_OPS* Get_CardModule1_Ops (void) {

    return &Card_1.ops;
}



