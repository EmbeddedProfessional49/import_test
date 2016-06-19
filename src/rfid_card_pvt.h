#ifndef __rfid_card_pvt_h__
#define __rfid_card_pvt_h__

#include <datatypes.h>
#include <rfid_card.h>

typedef union rfid_module_cfg RFID_MODULE_CFG;
typedef struct rfid_module RFID_MODULE;
union rfid_module_cfg {
    struct {
        u8 module;
        u8 uartnumber;
        u32 baudrate;
    }uart_cfg;
};

struct rfid_module {
    RFID_MODULE_CFG cfg;
    RFID_MODULE_OPS ops;
};


#endif
