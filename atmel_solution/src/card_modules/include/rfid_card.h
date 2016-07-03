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
typedef enum rfid_status RFID_STATUS;

typedef RFID_STATUS (*RFID_INIT) (void);
typedef RFID_STATUS (*RFID_ENABLE) (void);
typedef RFID_STATUS (*RFID_DISABLE) (void);
typedef RFID_STATUS (*RFID_UID) (u8 *uid_data);
typedef RFID_STATUS (*RFID_READ) (RFID_READ_DATA *read_data);
typedef RFID_STATUS (*RFID_WRITE) (RFID_WRITE_DATA *write_data);
typedef RFID_STATUS (*VIRTUAL_INIT) (RFID_MODULE_API *module);

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

enum rfid_status {
    RFID_OPERATION_SUCCEED = 0x00,
    RFID_NO_TAG,
    RFID_LOGIN_SUCCEED,
    RFID_LOGIN_FAIL,
    RFID_READ_FAIL,
    RFID_WRITE_FAIL,
    RFID_UNABLE_READ_AFTER_WRITE,
    RFID_ADDRESS_OVERFLOW,
    RFID_DOWNLOAD_KEY_FAIL,
    RFID_NOT_AUTHENTICATE,
    RFID_NOT_VALUE_BLOCK,
    RFID_INVALID_LEN_OF_COMMAND_FORMAT,
    RFID_CHECKSUM_ERROR,
    RFID_COMMAND_CODE_ERROR,
};

RFID_STATUS Get_RFID_Card (RFID_MODULE_API *module, u32 module_num);
void Load_RFID_Modules (void);


#endif
