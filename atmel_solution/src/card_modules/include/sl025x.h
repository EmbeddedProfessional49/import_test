#ifndef __Sl025x_h__
#define __Sl025x_h__

#include <rfid_card.h>

typedef enum sl025x_cmd SL025X_CMD;
typedef union sl025X_send_cmd SL025X_SEND_CMD;
typedef union sl025X_rcv_cmd SL025X_RCV_CMD;

enum sl025x_cmd {
    SELECT_MIFARE_CARD = 0x01,
    LOGIN_TO_SECTOR,
    READ_DATA_BLOCK,
    WRITE_DATA_BLOCK,
    READ_VALUE_BLOCK,
    INITIALIZE_VALUE_BLOCK,
    WRITE_MASTER_KEY,
    INCREMENT_VALUE,
    DECREMENT_VALUE,
    COPY_VALUE,
    READ_DATA_PAGE,
    WRITE_DATA_PAGE,
    DOWNLOAD_KEY,
    LOGIN_SECTOR_VIA_STORED_KEY,
    MANAGE_RED_LED,
    GET_FIRMWARE_VERSION,
    SL025X_MAX_CMD,
};

union sl025X_send_cmd {
    struct {
        u8 preamble;
        u8 len;
        SL025X_CMD cmd;
        u8 chksum;
    }select_mifare_card;    
    
    struct {
        u8 preamble;
        u8 len;
        SL025X_CMD cmd;
        u8 sector;
        u8 type;
        u8 *key;
        u8 chksum;
    }login_sector, download_key;
      
    struct {
        u8 preamble;
        u8 len;
        SL025X_CMD cmd;
        u8 sector;
        u8 type;
        u8 chksum;
    }login_sector_stored_key;
    
    struct {
        u8 preamble;
        u8 len;
        SL025X_CMD cmd;
        u8 block;
        u8 chksum;
    }read_data_block, read_value_block;
        
    struct {
        u8 preamble;
        u8 len;
        SL025X_CMD cmd;
        u8 block;
        u8 data[16];
        u8 chksum;
    }write_data_block;        
    
    struct {
        u8 preamble;
        u8 len;
        SL025X_CMD cmd;
        u8 block;
        u8 value[4];
        u8 chksum;
    }initialize_value_block, increment_value, decrement_value;

    struct {
        u8 preamble;
        u8 len;
        SL025X_CMD cmd;
        u8 sector;
        u8 key;
        u8 chksum;
    }write_master_key;
    
    struct {
        u8 preamble;
        u8 len;
        SL025X_CMD cmd;
        u8 source;
        u8 destination;
        u8 chksum;
    }copy_value;
       
    struct {
        u8 preamble;
        u8 len;
        SL025X_CMD cmd;
        u8 page;
        u8 chksum;
    }read_data_page;

    struct {
        u8 preamble;
        u8 len;
        SL025X_CMD cmd;
        u8 page;
        u8 data[4];
        u8 chksum;
    }write_data_page;
   
    struct {
        u8 preamble;
        u8 len;
        SL025X_CMD cmd;
        u8 code;
        u8 chksum;
    }manage_red_led;

    struct {
        u8 preamble;
        u8 len;
        SL025X_CMD cmd;
        u8 chksum;
    }get_firmware_version;    
};

union sl025X_rcv_cmd {
    struct {
        u8 preamble;
        u8 len;
        SL025X_CMD cmd;
        u8 status;
        u8 uid[7];
        u8 type;
        u8 chksum;
    }select_mifare_card;
    
    struct {
        u8 preamble;
        u8 len;
        SL025X_CMD cmd;
        u8 status;
        u8 chksum;
    }login_sector, download_key, login_sector_stored_key;
      
    struct {
        u8 preamble;
        u8 len;
        SL025X_CMD cmd;
        u8 status;
        u8 value[4];
        u8 chksum;
    }read_value_block, initialize_value_block, increment_value, decrement_value, copy_value;
 
    struct {
        u8 preamble;
        u8 len;
        SL025X_CMD cmd;
        u8 status;
        u8 data[16];
        u8 chksum;
    }read_data_block, write_data_block, read_data_page;
            
    struct {
        u8 preamble;
        u8 len;
        SL025X_CMD cmd;
        u8 status;
        u8 key;
        u8 chksum;
    }write_master_key;
           
    struct {
        u8 preamble;
        u8 len;
        SL025X_CMD cmd;
        u8 status;
        u8 data[4];
        u8 chksum;
    }write_data_page;

    struct {
        u8 preamble;
        u8 len;
        SL025X_CMD cmd;
        u8 code;
        u8 status;
    }manage_red_led;

    struct {
        u8 preamble;
        u8 len;
        SL025X_CMD cmd;
        u8 status;
        u8 data;
        u8 chksum;
    }get_firmware_version;    
};

RFID_MODULE_OPS* Get_Sl025x_Ops (void);

#endif
