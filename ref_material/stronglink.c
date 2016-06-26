/** \file   stronglink.c
 *  \brief  This file contains mifare communication command related functions.
 *
 *
 *
 * Copyright (C) 2014-2015 M-Tech Innovations LTD.  All Rights Reserved.
 *
 * This source is the copyright of, and contains proprietary information and trade secrets of
 * M-Tech Innovations LTD. It can only be copied, reproduced, modified, distributed or used under
 * a written license from M-Tech Innovations LTD. Licensee agrees to attach this notice on all copies
 * and derivative works.
 *
 *	\ingroup    MIFARE_MODULE_API
 *	\ingroup    MIFARE_MODULE_DRIVER
 */

//******************************************************************************
//  INCLUDE    FILES
//******************************************************************************
#include <string.h>
#include <serial.h>
#include <stronglink.h>


unsigned char CardPresentStatusFlag = 0x00;


/*! \brief  Login_ToSector
*
*			using this function we read or write operation on any sector in mifare card we need to login with login key value.
*
*   \param[in]   	in_trans_type : This indicates which mifare module (i.e IN OR OUT module) to be login.
*
*   \param[in]   	sector_no : This indicates to which sector to be login.
*
*   \param[in]   	Key : buffer in which card UID to be stored.
*
*   \return         if sector login successfully then return STRONGLINK_DATA_VALID otherwise respected error return.
*
*   \ingroup    MIFARE_MODULE_DRIVER
*/
static STRONGLINK_RET_STATUS Login_ToSector(TRANSACTION_TYPE in_trans_type,BLOCK_NUMBER sector_no, char *Key);


//******************************************************************************
//  CODE
//******************************************************************************


/*! \brief  UART_Ret_Parameter
*
*		This will return error type depending upon uart error.
*
*   \param[in]   	uart_ret : Input UART error.
*
*   \return         Depending upon uart error this will return strnonglink module error.
*
*   \ingroup    MIFARE_MODULE_DRIVER
*/
static STRONGLINK_RET_STATUS UART_Ret_Parameter(UART_DATA_STATUS uart_ret)
{
	STRONGLINK_RET_STATUS ret_status = STRONGLINK_DATA_VALID;

	switch(uart_ret)
	{
		case UART_DATA_VALID:
				ret_status = STRONGLINK_DATA_VALID;
				break;
		case UART_DATA_TIMEOUT:
				ret_status = STRONGLINK_DATA_TIMEOUT;			 	
				break;

		default:
					ret_status = STRONGLINK_DATA_INVALID_PARAMETER;
				break;	
	}
	return ret_status;
}
	
/*! \brief  HexToASCII_Strln
*
*			This function converts hex buffer values to equivalent ascii buffer values.
*
*   \param[in]   	ascii : buffer in which accii string to be stored.
*
*   \param[in]        hex	: buffer in which hex values are present.
*
*   \param[in]        j	: length of hex data.
*
*   \return         None.
*
*   \ingroup    MIFARE_MODULE_DRIVER
*/
static void HexToASCII_Strln(char *ascii,char *hex,int j)
{
	int i,temp;	 

	for(i=0;i<j;i=i+2)
	{
		temp =  *hex & 0xf0;
		temp >>=4;
		if((temp <= 0x09) && (temp >=0x00))
		ascii[i] = temp + 0x30;
		else
		ascii[i] = temp + 0x37;
		temp = *hex & 0x0f;
		if((temp <= 0x09) && (temp >=0x00))
		ascii[i+1] = temp + 0x30;
		else
		ascii[i+1] = temp + 0x37;
		hex++;
	}
	ascii[i] = '\0';
}


/*! \brief  XOR_Checksum
*
*			This function check sum for input buffer which contains command data to be send to mifare module
*
*   \param[in]   	Command_Packet : buffer in which contains command data of which check sum to be calculate.
*
*   \return         calculated check sum will be return.
*
*   \ingroup    MIFARE_MODULE_DRIVER
*/
static unsigned char XOR_Checksum(char Command_Packet[])
{
 	unsigned char checksum =0x00;
 	unsigned int byte_counter;
	unsigned int len;

	checksum = 0x00;
	len = Command_Packet[1];
	len = len + 0x01; 
	for(byte_counter = 0; byte_counter < len; byte_counter++)
	{	
		checksum = checksum ^ Command_Packet[byte_counter];	
	}
    return checksum;	
}


/*! \brief  Read_UID
*
*			This function reads mifare card UID and stored in input buffer in ASCII format.
*
*   \param[in]   	in_type : this indicates from which mifare reader UID to be read.
*
*   \param[in]   	ascii : buffer in which UID to be stored in ascii format.
*
*   \return         if card read successfully then return STRONGLINK_DATA_VALID otherwise respected error return.
*
*   \ingroup    MIFARE_MODULE_API
*/
STRONGLINK_RET_STATUS Read_UID(TRANSACTION_TYPE in_type,UID_DATA_ASCII_BUFFER ascii)
{
	char Select_Mfr_Card[] = {0xBA, 0x02, 0x01, 0x00};
	unsigned char byte_cont = 0 ;
	char CARD_ID[14] = {0};
	UART_DATA_STATUS uart_ret = UART_DATA_VALID;
	STRONGLINK_RET_STATUS ret_val = STRONGLINK_DATA_VALID;
	UART_NUMBER select_uart_num = UART_NUM_2;

	switch(in_type)
	{
		case MAIN_IN:
						{
							select_uart_num = UART_NUM_2;		// for uart 2 as input
						}
						break;

		case MAIN_OUT:
						{
							select_uart_num = UART_NUM_1;		// for uart 1 as output
						}
						break;

		default:
						{
							ret_val = STRONGLINK_DATA_INVALID;	
						}
						goto Out_Read_UID;
	}
	Select_Mfr_Card[3] = XOR_Checksum(Select_Mfr_Card);
	
	Clear_UART_Buffer(select_uart_num);

	for(byte_cont = 0; byte_cont < 4; byte_cont++)
	{
		Send_Char(select_uart_num,UART_TIME_OUT_3,Select_Mfr_Card[byte_cont]); 		
	}

 	
	for(byte_cont = 0; byte_cont < 2; byte_cont++)
	{
		uart_ret = Get_Char(select_uart_num,UART_TIME_OUT_3,&CARD_ID[byte_cont]);
		if(uart_ret != UART_DATA_VALID)
		{
			ret_val = UART_Ret_Parameter(uart_ret);
			goto Out_Read_UID;
		}
	}
	
	if(CARD_ID[0] == 0xBD)
	{
		if(CARD_ID[1] == 0x03)
		{
			for(byte_cont = 0; byte_cont < 3; byte_cont++)
			{
				uart_ret = Get_Char(select_uart_num,UART_TIME_OUT_3,&CARD_ID[byte_cont]);			
				if(uart_ret != UART_DATA_VALID)
				{
					ret_val = UART_Ret_Parameter(uart_ret);
					goto Out_Read_UID;
				}
			}
			if(CARD_ID[1] == 0x01)
			{
				ret_val = STRONGLINK_DATA_INVALID;
				goto Out_Read_UID;
			}
		}	 			

		else if(CARD_ID[1] == 0x08)
		{
			for(byte_cont = 0; byte_cont < 8; byte_cont++)
			{
				uart_ret = Get_Char(select_uart_num,UART_TIME_OUT_3,&CARD_ID[2 + byte_cont]);
				if(uart_ret != UART_DATA_VALID)
				{
					ret_val = UART_Ret_Parameter(uart_ret);
					goto Out_Read_UID;
				}
			}
		}
		else if(CARD_ID[1] == 0x0B)
		{
			for(byte_cont = 0; byte_cont < 11; byte_cont++)
			{
				uart_ret = Get_Char(select_uart_num,UART_TIME_OUT_3,&CARD_ID[2 + byte_cont]);
				if(uart_ret != UART_DATA_VALID)
				{
					ret_val = UART_Ret_Parameter(uart_ret);
					goto Out_Read_UID;
				}
			}
		}
		if(CARD_ID[3] == 0x00)
		{
			if(CARD_ID[1] == 0x08)
			{
				HexToASCII_Strln(ascii,&CARD_ID[4],8);
			}
			if(CARD_ID[1] == 0x0B)
			{
				HexToASCII_Strln(ascii,&CARD_ID[4],14);
			}
			ret_val = STRONGLINK_DATA_VALID;
			goto Out_Read_UID;
		}
		else
		{
			ret_val = STRONGLINK_DATA_INVALID;	
			goto Out_Read_UID;
		}
	}
	else
	{
		ret_val = STRONGLINK_DATA_INVALID;	
		goto Out_Read_UID;
	}

Out_Read_UID:
	return ret_val;
}
/*! \brief  Login_ToSector
*
*			using this function we read or write operation on any sector in mifare card we need to login with login key value.
*
*   \param[in]   	in_trans_type : This indicates which mifare module (i.e IN OR OUT module) to be login.
*
*   \param[in]   	sector_no : This indicates to which sector to be login.
*
*   \param[in]   	Key : buffer in which card UID to be stored.
*
*   \return         if sector login successfully then return STRONGLINK_DATA_VALID otherwise respected error return.
*
*   \ingroup    MIFARE_MODULE_DRIVER
*/
static STRONGLINK_RET_STATUS Login_ToSector(TRANSACTION_TYPE in_trans_type,BLOCK_NUMBER sector_no, char *Key)
{
	char Login_Sector[] = {0xBA, 0x0A, 0x02, 0x00, 0xAA, 
						   0x00, 0x00, 0x00, 0x00, 0x00,
						   0x00, 0x00};
	char LogSec_Resp[5];
	unsigned char byte_cont;

	UART_DATA_STATUS uart_ret = UART_DATA_VALID;
	STRONGLINK_RET_STATUS ret_val = STRONGLINK_DATA_VALID;
	UART_NUMBER select_uart_num = UART_NUM_2;

	memset(LogSec_Resp, 0x00, sizeof(LogSec_Resp));

	switch(in_trans_type)
	{
		case MAIN_IN:
						{
							select_uart_num = UART_NUM_2;		// for uart 2 as input
						}
						break;

		case MAIN_OUT:
						{
							select_uart_num = UART_NUM_1;		// for uart 1 as output
						}
						break;

		default:
						{
							ret_val = STRONGLINK_DATA_INVALID;	
						}
						goto Out_Login_ToSector;
	}

	Login_Sector[3] = sector_no;

	Login_Sector[5] = Key[0];
	Login_Sector[6] = Key[1];
	Login_Sector[7] = Key[2];
	Login_Sector[8] = Key[3];
	Login_Sector[9] = Key[4];
	Login_Sector[10] = Key[5];

	Login_Sector[11] = XOR_Checksum(Login_Sector);

	

	Clear_UART_Buffer(select_uart_num);

	for(byte_cont = 0; byte_cont < 12; byte_cont++)
	{
		Send_Char(select_uart_num,UART_TIME_OUT_3,Login_Sector[byte_cont]);		
	}
		
	for(byte_cont = 0; byte_cont < 5; byte_cont++)
	{
		uart_ret = Get_Char(select_uart_num,UART_TIME_OUT_3,&LogSec_Resp[byte_cont]);
		if(uart_ret != UART_DATA_VALID)
		{
			ret_val = UART_Ret_Parameter(uart_ret);
			goto Out_Login_ToSector;
		}
	}

	if(LogSec_Resp[3] == 0x02)
	{
		ret_val = STRONGLINK_DATA_VALID;
		goto Out_Login_ToSector;
	}
	else
	{
		ret_val = STRONGLINK_DATA_LOGIN_FAILED;	
		goto Out_Login_ToSector;
	}
Out_Login_ToSector:
		return ret_val;
}

/*! \brief  Read_Block_Data
*
*			Using this function read block of data from block input block number.
*
*   \param[in]   	entry_type : This indicates from which mifare module (i.e IN OR OUT module) data to be read.
*
*   \param[in]   	blk_no : This indicates from which block number data to be read.
*
*   \param[in]   	buffer : buffer in which read data to be stored.
*
*   \return         if card read successfully then return STRONGLINK_DATA_VALID otherwise respected error return.
*
*   \ingroup    MIFARE_MODULE_API
*/
STRONGLINK_RET_STATUS Read_Block_Data(TRANSACTION_TYPE entry_type,BLOCK_NUMBER blk_no,DATA_BUFFER buffer)
{
	char Read_Blk_Cmd[] = {0xBA,0x03,0x03,0x00,0x00};
	unsigned char byte_cont;
	unsigned char sector_no;
	char Default_Key[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	STRONGLINK_RET_STATUS login_resp;
	char UID_DATA_OUT[22];

	UART_DATA_STATUS uart_ret = UART_DATA_VALID;
	STRONGLINK_RET_STATUS ret_val = STRONGLINK_DATA_VALID;
	UART_NUMBER select_uart_num = UART_NUM_2;


	switch(entry_type)
	{
		case MAIN_IN:
						{
							select_uart_num = UART_NUM_2;		// for uart 2 as input
						}
						break;

		case MAIN_OUT:
						{
							select_uart_num = UART_NUM_1;		// for uart 1 as output
						}
						break;

		default:
						{
							ret_val = STRONGLINK_DATA_INVALID;	
						}
						goto Out_Read_Block_Data;
	}
	
	if(blk_no < 0x80)
	{
	 	sector_no = (blk_no/(0x04));
	}
	else if(blk_no >= 0x80)
	{
		sector_no = (blk_no/(0x10));
		sector_no = sector_no + 0x18;
	}

	login_resp = Login_ToSector(entry_type,sector_no,Default_Key);

	if(login_resp == STRONGLINK_DATA_VALID)
	{
		Read_Blk_Cmd[3] = blk_no;
		Read_Blk_Cmd[4] = XOR_Checksum(Read_Blk_Cmd);

		Clear_UART_Buffer(select_uart_num);
		for(byte_cont = 0; byte_cont < 5; byte_cont++)
		{
			Send_Char(select_uart_num,UART_TIME_OUT_3,Read_Blk_Cmd[byte_cont]);
		}

		for(byte_cont = 0; byte_cont < 21; byte_cont++)
		{
			uart_ret = Get_Char(select_uart_num,UART_TIME_OUT_3,&UID_DATA_OUT[byte_cont]);
			if(uart_ret != UART_DATA_VALID)
			{
				ret_val = UART_Ret_Parameter(uart_ret);
				goto Out_Read_Block_Data;
			}					
		}
	
		if(UID_DATA_OUT[3] == 0x00)
		{
			for(byte_cont = 0; byte_cont < 16; byte_cont++)
			{
				buffer[byte_cont] = UID_DATA_OUT[4 + byte_cont];
			}
			if(((blk_no == 0x80) || (blk_no == 0x99)) && (strlen(buffer) == 0))
			{
				ret_val = STRONGLINK_DATA_INVALID;
				goto Out_Read_Block_Data;
			} 
			ret_val = STRONGLINK_DATA_VALID;
			goto Out_Read_Block_Data;
		}
		else
		{
			ret_val = STRONGLINK_DATA_INVALID;
			goto Out_Read_Block_Data;
		}
	}

	ret_val = login_resp;
	goto Out_Read_Block_Data;

Out_Read_Block_Data:
	return ret_val;
}



/*! \brief  Write_Block
*
*			Using this function write block of data in block input block number.
*
*   \param[in]   	entry_type : This indicates from which mifare module (i.e IN OR OUT module) data to be Write.
*
*   \param[in]   	blk_no : This indicates in which block number data to be write.
*
*   \param[in]   	buffer : buffer from where data to be stored in card.
*
*   \return         if card write successfully then return STRONGLINK_DATA_VALID otherwise respected error return.
*
*   \ingroup    MIFARE_MODULE_API
*/
STRONGLINK_RET_STATUS Write_Block(TRANSACTION_TYPE entry_type,BLOCK_NUMBER blk_no,DATA_BUFFER buffer)	
{
	char Write_Blk_Cmd[22] = {0xBA,0x13,0x04,0x00,
									 0x00,0x00,0x00,0x00,
									 0x00,0x00,0x00,0x00,
									 0x00,0x00,0x00,0x00,
									 0x00,0x00,0x00,0x00,
									 0x00
									};
	unsigned char byte_cont;
	unsigned char sector_no;
	char Default_Key[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	char UID_DATA_OUT[22];

	UART_DATA_STATUS uart_ret = UART_DATA_VALID;
	STRONGLINK_RET_STATUS ret_val = STRONGLINK_DATA_VALID;
	UART_NUMBER select_uart_num = UART_NUM_2;
	STRONGLINK_RET_STATUS login_resp = STRONGLINK_DATA_VALID;;


	switch(entry_type)
	{
		case MAIN_IN:
						{
							select_uart_num = UART_NUM_2;		// for uart 2 as input
						}
						break;

		case MAIN_OUT:
						{
							select_uart_num = UART_NUM_1;		// for uart 1 as output
						}
						break;

		default:
						{
							ret_val = STRONGLINK_DATA_INVALID;	
						}
						goto Out_Write_Block;
	}



	if(blk_no < 0x80)
	{
	 	sector_no = (blk_no/(0x04));
	}
	else if(blk_no >= 0x80)
	{
		sector_no = (blk_no/(0x10));
		sector_no = sector_no + 0x18;
	}

	login_resp = Login_ToSector(entry_type,sector_no,Default_Key);
	if(login_resp == STRONGLINK_DATA_VALID)
	{
		Write_Blk_Cmd[3] = blk_no;
		
		for(byte_cont = 0; byte_cont < 16; byte_cont++)
		{
			Write_Blk_Cmd[4 + byte_cont] = buffer[byte_cont];
		}
		
		Write_Blk_Cmd[20] = XOR_Checksum(Write_Blk_Cmd);

		Clear_UART_Buffer(select_uart_num);
		for(byte_cont = 0; byte_cont < 21; byte_cont++)
		{
			Send_Char(select_uart_num,UART_TIME_OUT_3,Write_Blk_Cmd[byte_cont]);
		}
			
		 
		for(byte_cont = 0; byte_cont < 21; byte_cont++)
		{
			uart_ret = Get_Char(select_uart_num,UART_TIME_OUT_3,&UID_DATA_OUT[byte_cont]);
			if(uart_ret != UART_DATA_VALID)
			{
				ret_val = UART_Ret_Parameter(uart_ret);
				goto Out_Write_Block;
			}					
		}	

		if(UID_DATA_OUT[3] == 0x00)
		{
			ret_val = STRONGLINK_DATA_VALID;
			goto Out_Write_Block;
		}
		else
		{
			ret_val = STRONGLINK_DATA_INVALID;
			goto Out_Write_Block;
		}
	}
	else
	{
		ret_val = login_resp;
		goto Out_Write_Block;
	}
		

Out_Write_Block:
	return ret_val;
}




#if 0
char ST_Read_Card_Block(unsigned char blk_no, char *buffer)
{
	char Read_Blk_Cmd[] = {0xBA,0x03,0x03,0x00,0x00};
	unsigned char byte_cont;
	unsigned char sector_no;
	char Default_Key[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	unsigned char login_resp;
	char UID_DATA_OUT[22];

	if(blk_no < 0x80)
	{
	 	sector_no = (blk_no/(0x04));
	}
	else if(blk_no >= 0x80)
	{
		sector_no = (blk_no/(0x10));
		sector_no = sector_no + 0x18;
	}
	
	login_resp = Login_ToSector(sector_no,Default_Key,0x01);
		//	PrintHex(login_resp);
		
	if(login_resp == 0x00)
	{
		Read_Blk_Cmd[3] = blk_no;
		Read_Blk_Cmd[4] = XOR_Checksum(Read_Blk_Cmd);

		LPC_UART1->FCR = 0x07;
		for(byte_cont = 0; byte_cont < 5; byte_cont++)
		{
			while(!(LPC_UART1->LSR & 0x20));
			LPC_UART1->THR = Read_Blk_Cmd[byte_cont];
		}

		for(byte_cont = 0; byte_cont < 21; byte_cont++)
		{
			//UID_DATA_OUT[byte_cont] = Bitbang_ReceiveO();
			UID_DATA_OUT[byte_cont] = get_char(2);
		}
	
		if(UID_DATA_OUT[3] == 0x00)
		{
			for(byte_cont = 0; byte_cont < 16; byte_cont++)
			{
				buffer[byte_cont] = UID_DATA_OUT[4 + byte_cont];
			}
		//	printf("blk Data:%s\n",buffer);
			if(((blk_no == 0x80) || (blk_no == 0x99)) && (strlen(buffer) == 0))
			{
				return 0x02;	
			} 
			else
			{
				return 0x00;
			}
		}
		else
		{
			return 0x01;
		}
	}
	else
		return 0x01;
}


unsigned char Write_Block(unsigned char blk_no,char *buffer)
{
	char Write_Blk_Cmd[22] = {0xBA,0x13,0x04,0x00,
									 0x00,0x00,0x00,0x00,
									 0x00,0x00,0x00,0x00,
									 0x00,0x00,0x00,0x00,
									 0x00,0x00,0x00,0x00,
									 0x00
									};
	//unsigned char Read_Blk_Res[22];
	unsigned char byte_cont;
	unsigned char sector_no;
	char Default_Key[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	unsigned char login_resp;
	char UID_DATA_OUT[22];
//	unsigned int Time_out = 10;	

	if(blk_no < 0x80)
	{
	 	sector_no = (blk_no/(0x04));
	}
	else if(blk_no >= 0x80)
	{
		sector_no = (blk_no/(0x10));
		sector_no = sector_no + 0x18;
	}

	printf("Sector No:%d\n",(int)sector_no);

//	login_resp = Login_Sector(0x01,sector_no,Default_Key);
	login_resp = Login_ToSector(sector_no,Default_Key,0x01);
	PrintHex(login_resp);
	printf("\n");
	if(login_resp == 0x00)
	{
		Write_Blk_Cmd[3] = blk_no;
		
		for(byte_cont = 0; byte_cont < 16; byte_cont++)
		{
			Write_Blk_Cmd[4 + byte_cont] = buffer[byte_cont];
		}
		
		Write_Blk_Cmd[20] = XOR_Checksum(Write_Blk_Cmd);
		PrintHex(Write_Blk_Cmd[20]);
		printf("\n");

		for(byte_cont = 0; byte_cont < 21; byte_cont++)
		{
			PrintHex(Write_Blk_Cmd[byte_cont]);
		}
		
		LPC_UART1->FCR = 0x07;
		for(byte_cont = 0; byte_cont < 21; byte_cont++)
		{
			while(!(LPC_UART1->LSR & 0x20));
			LPC_UART1->THR = Write_Blk_Cmd[byte_cont];
		}
			
		 
		for(byte_cont = 0; byte_cont < 21; byte_cont++)
		{
			UID_DATA_OUT[byte_cont] = get_char(2);
		}	

		PrintHex(UID_DATA_OUT[3]);
		printf("\n");

		if(UID_DATA_OUT[3] == 0x00)
		{
			return 0x00;
		}
		else
		{
			return UID_DATA_OUT[3];
		}
	}
	else
		return login_resp;
}

#endif


/*! \brief  Mifare_PowerDown
*
*			This function used to powerdown mifare module.
*
*   \param[in]   	in_type : this indicates of which mifare to be powerdown.
*
*   \return         on successfully then return STRONGLINK_DATA_VALID otherwise respected error return.
*
*   \ingroup    MIFARE_MODULE_API
*/
STRONGLINK_RET_STATUS Mifare_PowerDown(TRANSACTION_TYPE in_type)
{
	char PowerDownMfr[] = {0xBA, 0x02, 0x50, 0x00};
	unsigned char byte_cont = 0 ;
	char CARD_ID[14] = {0};
	UART_DATA_STATUS uart_ret = UART_DATA_VALID;
	STRONGLINK_RET_STATUS ret_val = STRONGLINK_DATA_VALID;
	UART_NUMBER select_uart_num = UART_NUM_2;

	switch(in_type)
	{
		case MAIN_IN:
						{
							select_uart_num = UART_NUM_2;		// for uart 2 as input
						}
						break;

		case MAIN_OUT:
						{
							select_uart_num = UART_NUM_1;		// for uart 1 as output
						}
						break;

		default:
						{
							ret_val = STRONGLINK_DATA_INVALID;	
						}
						goto Out_Read_UID;
	}
	PowerDownMfr[3] = XOR_Checksum(PowerDownMfr);
	
	Clear_UART_Buffer(select_uart_num);

	for(byte_cont = 0; byte_cont < 4; byte_cont++)
	{
		Send_Char(select_uart_num,UART_TIME_OUT_3,PowerDownMfr[byte_cont]); 		
	}

 	
	for(byte_cont = 0; byte_cont < 2; byte_cont++)
	{
		uart_ret = Get_Char(select_uart_num,UART_TIME_OUT_3,&CARD_ID[byte_cont]);
		if(uart_ret != UART_DATA_VALID)
		{
			ret_val = UART_Ret_Parameter(uart_ret);
			goto Out_Read_UID;
		}
	}
	
	if(CARD_ID[0] == 0xBD)
	{
		if(CARD_ID[1] == 0x03)
		{
			for(byte_cont = 0; byte_cont < 3; byte_cont++)
			{
				uart_ret = Get_Char(select_uart_num,UART_TIME_OUT_3,&CARD_ID[byte_cont]);			
				if(uart_ret != UART_DATA_VALID)
				{
					ret_val = UART_Ret_Parameter(uart_ret);
					goto Out_Read_UID;
				}
			}
			if(CARD_ID[1] == 0xf0)
			{
				ret_val = STRONGLINK_DATA_CHECKSUM_ERROR;
				goto Out_Read_UID;
			}
			else if(CARD_ID[1] == 0x00)
			{
				ret_val = STRONGLINK_DATA_VALID;
				goto Out_Read_UID;
			}				
		}	 	
		ret_val = STRONGLINK_DATA_INVALID;
		goto Out_Read_UID;		
	}
	else
	{
		ret_val = STRONGLINK_DATA_INVALID;	
		goto Out_Read_UID;
	}

Out_Read_UID:
	return ret_val;
}
