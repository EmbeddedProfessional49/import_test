#ifndef __rfid_card_h__
#define __rfid_card_h__

#include <datatypes.h>

/*
 * rfid card module generic operations
 *
 * init card module
 * enable card module
 * disable card module
 * read uid
 * read data from card
 * write data to card
 * data related to specific card
 * test module
 * terminate card module
 */

typedef struct rfid_module_ops RFID_MODULE_OPS;

typedef u32 (*RFID_INIT) (RFID_MODULE_OPS *module);

struct rfid_module_ops {
    RFID_INIT init;
};

u32 Init_RFID_Card (RFID_MODULE_OPS *module_ops, u32 module_num);



#endif
