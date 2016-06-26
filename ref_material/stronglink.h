/** \file   stronglink.c
 *  \brief  This file contains mifare communication command related declairation of functions.
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
 *	\ingroup    MIFARE_MODULE_DATATYPES
 */




#ifndef 	__stronglink_h__
#define 	__stronglink_h__


#include <file_structure.h>

/**
*   \enum       __tag_STRONGLINK_RET_STATUS
*
*   \brief     	This indicates error returned during stronglink module communication.
*
*   \ingroup    MIFARE_MODULE_DATATYPES
*/
/**
*   \typedef    STRONGLINK_RET_STATUS
*
*   \brief     	This indicates error returned during stronglink module communication.
*
*   \ingroup    MIFARE_MODULE_DATATYPES
*/
typedef	enum	__tag_STRONGLINK_RET_STATUS
{
	STRONGLINK_DATA_VALID,					// 0
	STRONGLINK_DATA_INVALID,				// 1
	STRONGLINK_DATA_TIMEOUT,				// 2
	STRONGLINK_DATA_LOGIN_FAILED,			// 3	
	STRONGLINK_DATA_INVALID_PARAMETER,		// 4
	STRONGLINK_DATA_CHECKSUM_ERROR,			// 5

}STRONGLINK_RET_STATUS;




/**
*   \typedef    UID_DATA_ASCII_BUFFER
*
*   \brief      During card UID read this contains card uid data.
*
*   \ingroup    MIFARE_MODULE_DATATYPES
*/
typedef	char* UID_DATA_ASCII_BUFFER;

/**
*   \typedef    BLOCK_NUMBER
*
*   \brief      During card data read this contains block address of card from where data to be read.
*
*   \ingroup    MIFARE_MODULE_DATATYPES
*/
typedef	unsigned char BLOCK_NUMBER;

/**
*   \typedef    MEM_ADDR
*
*   \brief      During card data read this contains read data.
*
*   \ingroup    MIFARE_MODULE_DATATYPES
*/
typedef	char* DATA_BUFFER;



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
STRONGLINK_RET_STATUS Read_UID(TRANSACTION_TYPE in_type,UID_DATA_ASCII_BUFFER ascii);


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
STRONGLINK_RET_STATUS Read_Block_Data(TRANSACTION_TYPE entry_type,BLOCK_NUMBER blk_no,DATA_BUFFER buffer);

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
STRONGLINK_RET_STATUS Write_Block(TRANSACTION_TYPE entry_type,BLOCK_NUMBER blk_no,DATA_BUFFER buffer);


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
STRONGLINK_RET_STATUS Mifare_PowerDown(TRANSACTION_TYPE in_type);

#define READ_CARD_BLOCK_NUMBER	0x04
extern unsigned char CardPresentStatusFlag;


#endif
