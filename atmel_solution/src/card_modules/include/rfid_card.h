#ifndef __rfid_card_h__
#define __rfid_card_h__

#include <datatypes.h>

/*
 * rfid card module generic operations
 *
 * init card module
 * enable card module
 * disable card module
 * read uid >> buffer
 * read data from card>> buffer, block number, number of blocks 
 * write data to card>> buffer, block number, number of blocks
 * data related to specific card
 * test module
 * terminate card module
 */

typedef struct rfid_module_ops RFID_MODULE_OPS;
typedef struct rfid_module_api RFID_MODULE_API;
typedef struct rfid_read_data RFID_READ_DATA;
typedef struct rfid_write_data RFID_WRITE_DATA;

typedef u32 (*RFID_INIT) (void);
typedef u32 (*RFID_ENABLE) (void);
typedef u32 (*RFID_DISABLE) (void);
typedef u32 (*RFID_UID) (u8 *uid_data);
typedef u32 (*RFID_READ) (RFID_READ_DATA *read_data);
typedef u32 (*RFID_WRITE) (RFID_WRITE_DATA *write_data);
typedef u32 (*VIRTUAL_INIT) (RFID_MODULE_API *module);

struct rfid_module_ops {
    RFID_INIT init;
    RFID_ENABLE enable;
    RFID_DISABLE disable;
    RFID_UID uid;
    RFID_READ read;
    RFID_WRITE write;
};

struct rfid_module_api {
    VIRTUAL_INIT init;
    RFID_MODULE_OPS *module_ops;
};

struct rfid_read_data {
    u8 *data;
    u8 block_number;
    u8 number_of_blocks;
    u8 data_size;
};

struct rfid_write_data {
    u8 *data;
    u8 block_number;
    u8 number_of_blocks;
    u8 data_size;
};

u32 Get_RFID_Card (RFID_MODULE_API *module, u32 module_num);
void Load_RFID_Modules (void);


#endif
