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

typedef struct rfid_module RFID_MODULE;
typedef struct rfid_module_ops RFID_MODULE_OPS;
typedef struct rfid_module_api RFID_MODULE_API;
typedef struct rfid_data RFID_READ_DATA;
typedef struct rfid_data RFID_WRITE_DATA;
typedef enum rfid_status RFID_STATUS;
typedef struct rfid_uid_data RFID_UID_DATA;

typedef RFID_STATUS (*RFID_INIT) (void);
typedef RFID_STATUS (*RFID_ENABLE) (void);
typedef RFID_STATUS (*RFID_DISABLE) (void);
typedef RFID_STATUS (*RFID_UID) (RFID_UID_DATA *uid_data);
typedef RFID_STATUS (*RFID_READ) (RFID_READ_DATA *read_data);
typedef RFID_STATUS (*RFID_WRITE) (RFID_WRITE_DATA *write_data);

typedef RFID_STATUS (*VIRTUAL_INIT) (RFID_MODULE_API *pthis);
typedef RFID_STATUS (*VIRTUAL_UID) (RFID_MODULE_API *pthis, RFID_UID_DATA *uid_data);

struct rfid_module_ops {
    RFID_INIT init;
    RFID_ENABLE enable;
    RFID_DISABLE disable;
    RFID_UID uid;
    RFID_READ read;
    RFID_WRITE write;
    void *pthis;
    void *current_module;
};

struct rfid_module_api {
    VIRTUAL_INIT init;
    VIRTUAL_UID uid;
    RFID_MODULE_OPS *module_ops;
};

struct rfid_data {
    u8 *data;
    u8 block_number;
    u8 data_size;
};

struct rfid_uid_data {
    u8 *uid;
    u8 type;
    u8 uid_size;
};

enum rfid_status {
    /* SL025 error numbers*/
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
    /* Don't change above order */    
    RFID_UNEXPECTED_CMD_ERROR,
    RFID_CMD_NOT_RCVED,
    RFID_VALID_CMD_FORMAT,
    RFID_TX_HAL_ERROR,
    RFID_RX_HAL_ERROR,
};


RFID_STATUS Get_RFID_Card (RFID_MODULE_API *module, u32 module_num);
void Load_RFID_Modules (void);


#endif
