#ifndef __CardModule1_h__
#define __CardModule1_h__

#include <rfid_card.h>

union sl025X_module_send_cmd {
    struct {
        u8 preamble;
        u8 len;
        u8 cmd;
        u8 chksum;
    }select_mifare_card;    
    
    struct {
        u8 preamble;
        u8 len;
        u8 cmd;
        u8 sector;
        u8 type;
        u8 key;
        u8 chksum;
    }login_sector, download_key;
      
    struct {
        u8 preamble;
        u8 len;
        u8 cmd;
        u8 sector;
        u8 type;
        u8 chksum;
    }login_sector_stored_key;
    
    struct {
        u8 preamble;
        u8 len;
        u8 cmd;
        u8 block;
        u8 chksum;
    }read_data_block, read_value_block;
        
    struct {
        u8 preamble;
        u8 len;
        u8 cmd;
        u8 block;
        u8 data[16];
        u8 chksum;
    }write_data_block;        
    
    struct {
        u8 preamble;
        u8 len;
        u8 cmd;
        u8 block;
        u8 value[4];
        u8 chksum;
    }initialize_value_block, increment_value, decrement_value;

    struct {
        u8 preamble;
        u8 len;
        u8 cmd;
        u8 sector;
        u8 key;
        u8 chksum;
    }write_master_key;
    
    struct {
        u8 preamble;
        u8 len;
        u8 cmd;
        u8 source;
        u8 destination;
        u8 chksum;
    }copy_value;
       
    struct {
        u8 preamble;
        u8 len;
        u8 cmd;
        u8 page;
        u8 chksum;
    }read_data_page;

    struct {
        u8 preamble;
        u8 len;
        u8 cmd;
        u8 page;
        u8 data[4];
        u8 chksum;
    }write_data_page;
   
    struct {
        u8 preamble;
        u8 len;
        u8 cmd;
        u8 code;
        u8 chksum;
    }manage_red_led;

    struct {
        u8 preamble;
        u8 len;
        u8 cmd;
        u8 chksum;
    }get_firmware_version;    
};

union sl025X_module_rcv_cmd {
    struct {
        u8 preamble;
        u8 len;
        u8 cmd;
        u8 status;
        u8 uid[7];
        u8 type;
        u8 chksum;
    }select_mifare_card;
    
    struct {
        u8 preamble;
        u8 len;
        u8 cmd;
        u8 status;
        u8 chksum;
    }login_sector, download_key, login_sector_stored_key;
      
    struct {
        u8 preamble;
        u8 len;
        u8 cmd;
        u8 status;
        u8 value[4];
        u8 chksum;
    }read_value_block, initialize_value_block, increment_value, decrement_value, copy_value;
 
    struct {
        u8 preamble;
        u8 len;
        u8 cmd;
        u8 status;
        u8 data[16];
        u8 chksum;
    }read_data_block, write_data_block, read_data_page;
            
    struct {
        u8 preamble;
        u8 len;
        u8 cmd;
        u8 status;
        u8 key;
        u8 chksum;
    }write_master_key;
           
    struct {
        u8 preamble;
        u8 len;
        u8 cmd;
        u8 status;
        u8 data[4];
        u8 chksum;
    }write_data_page;

    struct {
        u8 preamble;
        u8 len;
        u8 cmd;
        u8 code;
        u8 status;
    }manage_red_led;

    struct {
        u8 preamble;
        u8 len;
        u8 cmd;
        u8 status;
        u8 data;
        u8 chksum;
    }get_firmware_version;    
};

u32 CardModule1_init (void);
RFID_MODULE_OPS* Get_CardModule1_Ops (void);

#endif
