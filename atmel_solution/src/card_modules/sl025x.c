#include <rfid_card_pvt.h>
#include <sl025x.h>
#include <string.h>
#include <timer_utilities.h>


#define SEND_PREAMBLE 0xBA
#define RECV_PREAMBLE 0xBD
#define MAX_RCV_CMD 0x05


static RFID_STATUS sl025x_init (void);
static RFID_STATUS sl025x_uid (RFID_UID_DATA *uid_data);
static RFID_STATUS sl025x_read_data (RFID_READ_DATA *read_data);
static u8 xor_checksum (u8 *command_packet);


static RFID_STATUS verify_rcv_cmd (SL025X_RCV_CMD *rcv_cmd) {
    
    u8 chck_sum_cmd = 0x00;
    
    chck_sum_cmd = ((u8 *) rcv_cmd) [rcv_cmd ->select_mifare_card.len + 1];
    
    if (rcv_cmd ->select_mifare_card.cmd >= SL025X_MAX_CMD) {
        return RFID_COMMAND_CODE_ERROR;
    }
    else if (chck_sum_cmd != (xor_checksum ((u8 *) &rcv_cmd ->select_mifare_card))) {
        return RFID_CHECKSUM_ERROR; 
    }
    
    return RFID_VALID_CMD_FORMAT;    
}

/* Buffer related operations */
static struct {
    SL025X_RCV_CMD cmd_buffer [MAX_RCV_CMD];
    u8 put_pointer;
    u8 get_pointer;
}received_cmd;

static RFID_STATUS get_received_cmd (SL025X_RCV_CMD *cmd) {
    
    if (received_cmd.put_pointer == received_cmd.get_pointer) {
        return RFID_CMD_NOT_RCVED;
    }
    else {
        u8 temp_position = 0x00;
        RFID_STATUS ret_value = 0x00;
        
        temp_position = received_cmd.get_pointer;
        received_cmd.get_pointer ++;
        if (received_cmd.get_pointer >= MAX_RCV_CMD) {
            received_cmd.get_pointer = 0x00;
        }       
        ret_value = verify_rcv_cmd (&received_cmd.cmd_buffer [temp_position]);
        if (ret_value == RFID_VALID_CMD_FORMAT) {
            cmd = &received_cmd.cmd_buffer [temp_position];
        }
        return ret_value;
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

/* End of buffer related operations */

static RFID_STATUS send_cmd (SL025X_SEND_CMD *cmd) {
    
    // using command len fieled we send data to uart
    
    return RFID_OPERATION_SUCCEED;
}    

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
        .read = sl025x_read_data,
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

static RFID_STATUS sl025x_uid (RFID_UID_DATA *uid_data) {
    
    RFID_STATUS ret_value = RFID_LOGIN_SUCCEED;
    SL025X_RCV_CMD *rcv_cmd = NULL;
    SL025X_SEND_CMD snd_cmd = {{0}};
    u8 counter = 0x00;
    
    snd_cmd.select_mifare_card.preamble = SEND_PREAMBLE;
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
    ret_value = get_received_cmd (rcv_cmd);
    if (ret_value != RFID_VALID_CMD_FORMAT) {
        return ret_value;
    }

    if (rcv_cmd ->select_mifare_card.cmd != SELECT_MIFARE_CARD) {
        return RFID_UNEXPECTED_CMD_ERROR;
    }

    if (rcv_cmd ->select_mifare_card.status != RFID_OPERATION_SUCCEED) {
        return ret_value;
    }
    
    uid_data ->uid_size =  (rcv_cmd ->select_mifare_card.len - 4);
    
    for (counter = 0x00; counter < uid_data ->uid_size; counter ++) {
        uid_data ->uid [counter] = ((u8 *) &rcv_cmd ->select_mifare_card.uid) [counter];
    }
    uid_data ->type = ((u8 *) &rcv_cmd ->select_mifare_card.uid) [counter];
    
    return ret_value;
}

static RFID_STATUS sl025x_login_to_sector (LOGIN_SECTOR_DETAILS *login_details) {
    
    RFID_STATUS ret_value = RFID_OPERATION_SUCCEED;
    SL025X_RCV_CMD *rcv_cmd = NULL;
    SL025X_SEND_CMD cmd = {{0}};;
    
    cmd.login_sector.preamble = SEND_PREAMBLE;
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
    ret_value = get_received_cmd (rcv_cmd);
    if (ret_value != RFID_VALID_CMD_FORMAT) {
        return ret_value;
    }

    if (rcv_cmd ->login_sector.cmd != LOGIN_TO_SECTOR) {
        return RFID_UNEXPECTED_CMD_ERROR;
    }

    return rcv_cmd ->login_sector.status;
}

static RFID_STATUS sl025x_read_data (RFID_READ_DATA *read_data) {
    RFID_STATUS ret_value = RFID_OPERATION_SUCCEED;
    SL025X_RCV_CMD *rcv_cmd = NULL;
    SL025X_SEND_CMD cmd = {{0}};;
    LOGIN_SECTOR_DETAILS login_sector = {0};
    u8 key [] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    u8 sector_no = 0x00;
    
    cmd.read_data_block.preamble = SEND_PREAMBLE;
    cmd.read_data_block.len = 0x03;
    cmd.read_data_block.cmd = READ_DATA_BLOCK;
    cmd.read_data_block.block = read_data ->block_number;
    cmd.read_data_block.chksum = xor_checksum ((u8 *) &cmd.read_data_block);
        
    if(read_data ->block_number < 0x80) {
        sector_no = (read_data ->block_number / 0x04);
    }
    else if (sector_no >= 0x80) {
        sector_no = (read_data ->block_number / 0x10);
        sector_no = sector_no + 0x18;
    }
    
    login_sector.key = key;
    login_sector.key_type = 0xAA; 
    login_sector.sector = sector_no;
    
    ret_value = sl025x_login_to_sector (&login_sector);
    
    if (ret_value != RFID_LOGIN_SUCCEED) {
        return ret_value;
    }
        
    clear_received_cmd ();
    
    ret_value = send_cmd (&cmd);
    
    // delay
    time_delay_ms (5);
    
    // get response from card module
    ret_value = get_received_cmd (rcv_cmd);
    if (ret_value != RFID_VALID_CMD_FORMAT) {
        return ret_value;
    }
    
    if (rcv_cmd ->read_data_block.cmd != READ_DATA_BLOCK) {
        return RFID_UNEXPECTED_CMD_ERROR;
    }
    if (rcv_cmd ->read_data_block.status != RFID_OPERATION_SUCCEED) {
        return rcv_cmd ->read_data_block.status;
    }
    
    memcpy (read_data ->data, rcv_cmd ->read_data_block.data, sizeof (rcv_cmd ->read_data_block.data));
    
    return ret_value;
}    

