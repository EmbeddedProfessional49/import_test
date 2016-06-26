#include <rfid_card_pvt.h>
#include <sl025x.h>

#define PREAMBLE_BYTE 0xBA

static RFID_STATUS sl025x_init (void);
RFID_STATUS sl025x_uid (u8 *buffer);
static u8 xor_checksum (u8 *command_packet);


typedef struct login_sector_details {
    u8 sector;
    u8 key_type;
    u8 *key;
}LOGIN_SECTOR_DETAILS;

static RFID_MODULE Card_1 = {
    .cfg.uart_cfg = {
        .uartnumber = 1,
        .baudrate = 1152000,
    },
    .ops = {
        .init = sl025x_init,
        .enable = NULL,
        .disable = NULL,
        .uid = NULL,
        .read = NULL,
        .write = NULL,
    }
};

static u8 xor_checksum (u8 *command_packet)
{
    u8 checksum =0x00;
    u8 byte_counter;
    u8 len;

    checksum = 0x00;
    len = command_packet[1];
    len = len + 0x01;
    for(byte_counter = 0; byte_counter < len; byte_counter++) {
        checksum = checksum ^ command_packet[byte_counter];
    }
    
    return checksum;
}

RFID_STATUS sl025x_uid (u8 *usr_buffer) {
    RFID_STATUS ret_value = RFID_LOGIN_SUCCEED;
    SL025X_SEND_CMD cmd;
    cmd.select_mifare_card.preamble = PREAMBLE_BYTE;
    cmd.select_mifare_card.len = 0x02;
    cmd.select_mifare_card.cmd = SELECT_MIFARE_CARD;
    cmd.select_mifare_card.chksum = xor_checksum ((u8 *) &cmd.select_mifare_card);
    
    // from here send command to sl025x module
    
    return ret_value;
}

RFID_STATUS sl025x_init (void) {

    // module init code

    return 0;
}

RFID_MODULE_OPS* Get_Sl025x_Ops (void) {

    return &Card_1.ops;
}

static RFID_STATUS sl025x_login_to_sector (LOGIN_SECTOR_DETAILS *login_details) {
    RFID_STATUS ret_value = RFID_OPERATION_SUCCEED;
    SL025X_SEND_CMD cmd;
    cmd.login_sector.preamble = PREAMBLE_BYTE;
    cmd.login_sector.len = 0x0A;
    cmd.login_sector.cmd = LOGIN_TO_SECTOR;
    cmd.login_sector.sector = login_details->sector;
    cmd.login_sector.type = login_details->key_type;
    cmd.login_sector.key = login_details->key;
    cmd.login_sector.chksum = xor_checksum ((u8 *) &cmd.login_sector);
    
    
    
    return ret_value;
}



