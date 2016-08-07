#include <rfid_card_pvt.h>
#include <sl025x.h>
#include <string.h>
#include <timer_utilities.h>
#include <usart.h>
#include <usart_interrupt.h>


/*pointer to current operating module*/
static RFID_MODULE *pthis;


static RFID_STATUS sl025x_init (void);
static RFID_STATUS sl025x_uid (RFID_UID_DATA *uid_data);
static RFID_STATUS sl025x_read_data (RFID_READ_DATA *read_data);
static RFID_STATUS sl025x_write_data (RFID_WRITE_DATA *write_data);

static u8 xor_checksum (u8 *command_packet);
static RFID_STATUS verify_rcv_cmd (void *cmd);
static RFID_STATUS get_received_cmd (void *user_buff);
static void put_received_cmd (RFID_MODULE *module, void *cmd);
static void clear_received_cmd (void);
static RFID_STATUS send_cmd (void *cmd);


static void register_Card1_callback (void);
static struct usart_module Card1_usart;
static SL025X_RCV_CMD Card1_rcv_buffer [MAX_RCV_CMD];
static RFID_MODULE Card_1 = {
    .hal_cfg.uart_cfg = {
        .module = &Card1_usart,
        .baudrate = 9600,
        .register_callback = register_Card1_callback,
    },        
    .cmd =  {
        .put = put_received_cmd,
        .get = get_received_cmd,
        .clear = clear_received_cmd,
        .send = send_cmd,
        .buffer = (u8 *) Card1_rcv_buffer,
    },
    .ops =  {
        .init = sl025x_init,
        .uid = sl025x_uid,
        .read = sl025x_read_data,
        .write = sl025x_write_data,
        .pthis = &Card_1,
        .current_module = &pthis,
    }
};

static void Card1_read_callback (struct usart_module *const usart_module) {
#define READ_BUFFER_SIZE 50 
    u8 buffer_read [READ_BUFFER_SIZE];
    u8 byte_count = 0x00;
    
    usart_write_buffer_job (Card_1.hal_cfg.uart_cfg.module, buffer_read, READ_BUFFER_SIZE);
    
    for (byte_count = 0x00; byte_count < READ_BUFFER_SIZE; byte_count ++)
    {
        if (buffer_read [byte_count] == RECV_PREAMBLE)
        {   
            if (READ_BUFFER_SIZE <= (byte_count + buffer_read [byte_count + 2])) {
                Card_1.cmd.put (&Card_1, &buffer_read [byte_count]);
            }
            break;
        }
    }
}

static void register_Card1_callback (void) {
    usart_register_callback (pthis ->hal_cfg.uart_cfg.module, Card1_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
    usart_enable_callback(pthis ->hal_cfg.uart_cfg.module, USART_CALLBACK_BUFFER_RECEIVED);
}

RFID_MODULE_OPS* Get_Sl025x_Ops (void) {

    return &Card_1.ops;
}

static RFID_STATUS sl025x_init (void) {

    //! [setup_config]
    struct usart_config config_usart;
    //! [setup_config]
    //! [setup_config_defaults]
    usart_get_config_defaults(&config_usart);
    //! [setup_config_defaults]

    //! [setup_change_config]
    config_usart.baudrate    = pthis ->hal_cfg.uart_cfg.baudrate;
/*    config_usart.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
    config_usart.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
    config_usart.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
    config_usart.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
    config_usart.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;*/
    //! [setup_change_config]

    //! [setup_set_config]
//    while (usart_init(pthis ->hal_cfg.uart_cfg.module, EDBG_CDC_MODULE, &config_usart) != STATUS_OK) {
//    }
    //! [setup_set_config]

    //! [setup_enable]
    usart_enable(pthis ->hal_cfg.uart_cfg.module);
    //! [setup_enable]
    
    /* Callback functions register*/
    pthis ->hal_cfg.uart_cfg.register_callback();
    
    return RFID_OPERATION_SUCCEED;
}

static RFID_STATUS verify_rcv_cmd (void *cmd) {
    
    SL025X_RCV_CMD *rcv_cmd = (SL025X_RCV_CMD *) cmd;
    
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

static RFID_STATUS get_received_cmd (void *user_buff) {
    
    if (pthis ->cmd.put_pointer == pthis ->cmd.get_pointer) {
        return RFID_CMD_NOT_RCVED;
    }
    else {
        u8 temp_position = 0x00;
        RFID_STATUS ret_value = 0x00;
        
        temp_position = pthis ->cmd.get_pointer;
        pthis ->cmd.get_pointer ++;
        if (pthis ->cmd.get_pointer >= MAX_RCV_CMD) {
            pthis ->cmd.get_pointer = 0x00;
        }       
        ret_value = verify_rcv_cmd (&pthis ->cmd.buffer [temp_position]);
        if (ret_value == RFID_VALID_CMD_FORMAT) {
            user_buff = &pthis ->cmd.buffer [temp_position];
        }
        return ret_value;
    }
}

static void put_received_cmd (RFID_MODULE *module, void *cmd) {
    
    memcpy (&module ->cmd.buffer [module ->cmd.put_pointer], cmd, sizeof (SL025X_RCV_CMD));
    
    module ->cmd.put_pointer ++;
    if (module ->cmd.put_pointer >= MAX_RCV_CMD) {
        module ->cmd.put_pointer = 0x00;
    }
    if (module ->cmd.put_pointer == module ->cmd.get_pointer){
        module ->cmd.get_pointer ++;
        if (module ->cmd.get_pointer >= MAX_RCV_CMD) {
            module ->cmd.get_pointer = 0x00;
        }
    }
}

static void clear_received_cmd (void) {
    
    memset ((u8 *) pthis ->cmd.buffer, 0x00, (sizeof (SL025X_RCV_CMD) * MAX_RCV_CMD));
    pthis ->cmd.get_pointer = 0x00;
    pthis ->cmd.put_pointer = 0x00;    
}

static RFID_STATUS send_cmd (void *cmd) {

    SL025X_SEND_CMD *send_cmd = cmd;
    
    // using command len we send data to hal
    if (usart_write_buffer_wait (pthis ->hal_cfg.uart_cfg.module, (u8 *) send_cmd, ((send_cmd ->select_mifare_card.len) + 2)) != STATUS_OK) {
        return RFID_TX_HAL_ERROR;   
    }
    
    return RFID_OPERATION_SUCCEED;
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
    ret_value = pthis ->cmd.send (&snd_cmd);
    if (ret_value != RFID_OPERATION_SUCCEED) {
        return ret_value;
    }
    
    // add some delay in range of msec.
    time_delay_ms (5);
    
    // get response from card module
    ret_value = pthis ->cmd.get (rcv_cmd);
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
    
    ret_value = pthis ->cmd.send (&cmd);
    
    // delay
    time_delay_ms (5);
    
    // get response from card module
    ret_value = pthis ->cmd.get (rcv_cmd);
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
    
    ret_value = pthis ->cmd.send (&cmd);
    
    // delay
    time_delay_ms (5);
    
    // get response from card module
    ret_value = pthis ->cmd.get (rcv_cmd);
    if (ret_value != RFID_VALID_CMD_FORMAT) {
        return ret_value;
    }
    
    if (rcv_cmd ->read_data_block.cmd != READ_DATA_BLOCK) {
        return RFID_UNEXPECTED_CMD_ERROR;
    }
    if (rcv_cmd ->read_data_block.status != RFID_OPERATION_SUCCEED) {
        return rcv_cmd ->read_data_block.status;
    }
    
    memcpy (read_data ->data, rcv_cmd ->read_data_block.data, MAX_CMD_DATA_LEN);
    
    return ret_value;
}

static RFID_STATUS sl025x_write_data (RFID_WRITE_DATA *write_data) {
    RFID_STATUS ret_value = RFID_OPERATION_SUCCEED;
    SL025X_RCV_CMD *rcv_cmd = NULL;
    SL025X_SEND_CMD cmd = {{0}};;
    LOGIN_SECTOR_DETAILS login_sector = {0};
    u8 key [] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    u8 sector_no = 0x00;
    
    cmd.write_data_block.preamble = SEND_PREAMBLE;
    cmd.write_data_block.len = 0x13;
    cmd.write_data_block.cmd = WRITE_DATA_BLOCK;
    cmd.write_data_block.block = write_data ->block_number;
    cmd.write_data_block.data = (u8 (*) [MAX_CMD_DATA_LEN]) write_data ->data;
    cmd.write_data_block.chksum = xor_checksum ((u8 *) &cmd.write_data_block);
    
    if(write_data ->block_number < 0x80) {
        sector_no = (write_data ->block_number / 0x04);
    }
    else if (sector_no >= 0x80) {
        sector_no = (write_data ->block_number / 0x10);
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
    
    ret_value = pthis ->cmd.send (&cmd);
    
    // delay
    time_delay_ms (5);
    
    // get response from card module
    ret_value = pthis ->cmd.get (rcv_cmd);
    if (ret_value != RFID_VALID_CMD_FORMAT) {
        return ret_value;
    }
    
    if (rcv_cmd ->write_data_block.cmd != READ_DATA_BLOCK) {
        return RFID_UNEXPECTED_CMD_ERROR;
    }
    if (rcv_cmd ->write_data_block.status != RFID_OPERATION_SUCCEED) {
        return rcv_cmd ->write_data_block.status;
    }
       
    return ret_value;
}
