#include <rfid_card_pvt.h>
#include <sl025x.h>
#include <string.h>
#include <timer_utilities.h>

#define PREAMBLE_BYTE 0xBA
#define MAX_RCV_CMD 0x05

static RFID_STATUS sl025x_init (void);
static RFID_STATUS sl025x_uid (u8 *buffer);
static u8 xor_checksum (u8 *command_packet);

/* Buffer related operations */
static struct {
    SL025X_RCV_CMD cmd_buffer [MAX_RCV_CMD];
    u8 put_pointer;
    u8 get_pointer;
}received_cmd;

static SL025X_RCV_CMD *get_received_cmd (void) {
    
    if (received_cmd.put_pointer == received_cmd.get_pointer) {
        return NULL;
    }
    else {
        u8 temp_position = 0x00;
        
        temp_position = received_cmd.get_pointer;
        received_cmd.get_pointer ++;
        if (received_cmd.get_pointer >= MAX_RCV_CMD) {
            received_cmd.get_pointer = 0x00;
        }       
        
        return &received_cmd.cmd_buffer [temp_position];
    }
}

static void put_received_cmd (SL025X_RCV_CMD *rcv_cmd) {
    
    memcpy (&received_cmd.cmd_buffer [received_cmd.put_pointer], rcv_cmd, sizeof (SL025X_RCV_CMD));
    
    received_cmd.put_pointer ++;
    if (received_cmd.put_pointer >= MAX_RCV_CMD) {
        received_cmd.put_pointer = 0x00;
    }
    if (received_cmd.put_pointer == received_cmd.get_pointer){
        received_cmd.get_pointer ++;
        if (received_cmd.get_pointer >= MAX_RCV_CMD) {
            received_cmd.get_pointer = 0x00;
        }
    }   
}

static void clear_received_cmd (void) {
    memset ((u8 *) &received_cmd, 0x00, sizeof (received_cmd));
    
    received_cmd.get_pointer = 0x00;
    received_cmd.put_pointer = 0x00;    
}

static RFID_STATUS send_cmd (SL025X_SEND_CMD *cmd) {
    
    // using command len fieled we send data to uart
    
    return RFID_OPERATION_SUCCEED;
}    

/* End of buffer related operations */


static RFID_MODULE Card_1 = {
    .cfg.uart_cfg = {
        .uartnumber = 1,
        .baudrate = 1152000,
    },
    .ops = {
        .init = sl025x_init,
        .enable = NULL,
        .disable = NULL,
        .uid = sl025x_uid,
        .read = NULL,
        .write = NULL,
    }
};

RFID_MODULE_OPS* Get_Sl025x_Ops (void) {

    return &Card_1.ops;
}

static u8 xor_checksum (u8 *command_packet) {
    
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

static RFID_STATUS sl025x_init (void) {

    // module init code

    return 0;
}

static RFID_STATUS sl025x_uid (u8 *usr_buffer) {
    
    RFID_STATUS ret_value = RFID_LOGIN_SUCCEED;
    SL025X_RCV_CMD *rcv_cmd;
    SL025X_SEND_CMD snd_cmd;
    
    snd_cmd.select_mifare_card.preamble = PREAMBLE_BYTE;
    snd_cmd.select_mifare_card.len = 0x02;
    snd_cmd.select_mifare_card.cmd = SELECT_MIFARE_CARD;
    snd_cmd.select_mifare_card.chksum = xor_checksum ((u8 *) &snd_cmd.select_mifare_card);
    
    // clear buffer
    clear_received_cmd ();
    
    // from here send command to sl025x module
    ret_value = send_cmd (&snd_cmd);
    if (ret_value != RFID_OPERATION_SUCCEED) {
        return ret_value;
    }
    
    // add some delay in range of msec.
    time_delay_ms (5);
    
    // get response from card module
    rcv_cmd = get_received_cmd ();
    
    if (rcv_cmd->select_mifare_card.cmd != SELECT_MIFARE_CARD) {
        return RFID_COMMAND_CODE_ERROR;
    }
    
    return ret_value;
}

static RFID_STATUS sl025x_login_to_sector (LOGIN_SECTOR_DETAILS *login_details) {
    
    RFID_STATUS ret_value = RFID_OPERATION_SUCCEED;
    SL025X_RCV_CMD *rcv_cmd;
    SL025X_SEND_CMD cmd;
    
    cmd.login_sector.preamble = PREAMBLE_BYTE;
    cmd.login_sector.len = 0x0A;
    cmd.login_sector.cmd = LOGIN_TO_SECTOR;
    cmd.login_sector.sector = login_details->sector;
    cmd.login_sector.type = login_details->key_type;
    cmd.login_sector.key = login_details->key;
    cmd.login_sector.chksum = xor_checksum ((u8 *) &cmd.login_sector);
    
    clear_received_cmd ();
    
    ret_value = send_cmd (&cmd);
    
    // delay
    time_delay_ms (5);
    
    // get response from card module
    rcv_cmd = get_received_cmd ();
    
    if (rcv_cmd->login_sector.cmd != LOGIN_TO_SECTOR) {
        return RFID_COMMAND_CODE_ERROR;
    }

    return ret_value;
}



