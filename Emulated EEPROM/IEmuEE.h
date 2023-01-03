/***************************************************************************//**
 * @brief Emulated EEPROM Interface Header File
 * 
 * @author Matthew Spinks
 * 
 * @date 12/24/22  Original creation
 * 
 * @file IEmuEE.h
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#ifndef IEMUEE_H
#define IEMUEE_H

#include <stdint.h>
#include <stdbool.h>

// ***** Defines ***************************************************************

#define EMUEE_PAGE_SIZE_1KB     0x400
#define EMUEE_PAGE_SIZE_2KB     0x800
#define EMUEE_PAGE_SIZE_4KB     0x1000

// ***** Global Variables ******************************************************

typedef enum EmuEEFormatTag
{
    /* Format is X.Y bits, where
    X = num of entries (2^X)
    Y = max size of each entry (2^Y bytes)
    Total is 14 bits. Bits [15:14] are reserved 
    Your flash page may not be able to hold the desired number of entries
    depending on its size. Choose accordingly.*/
    EMUEE_FORMAT_12_2 = 0xC2,
    EMUEE_FORMAT_11_3 = 0xB3,
    EMUEE_FORMAT_10_4 = 0xA4,
    EMUEE_FORMAT_9_5 = 0x95,
    EMUEE_FORMAT_8_6 = 0x86,
    EMUEE_FORMAT_7_7 = 0x77, // default
    EMUEE_FORMAT_6_8 = 0x68,
    EMUEE_FORMAT_5_9 = 0x59,
    EMUEE_FORMAT_4_10 = 0x4A,
    EMUEE_FORMAT_3_11 = 0x3B,
    EMUEE_FORMAT_2_12 = 0x2C,
    EMUEE_FORMAT_1_13 = 0x1D,
} EmuEEFormat;

typedef enum EmuEEFlashWordSizeTag
{
    EMUEE_WORD_SIZE_2_BYTES = 2, // default
    EMUEE_WORD_SIZE_4_BYTES = 4,
} EmuEEFlashWordSize;

typedef enum EmuEEErrorTag
{
    EMUEE_ERROR_NONE = 0,
    EMUEE_ERROR_INVALID_ADDRESS,
    EMUEE_ERROR_INVALID_FORMAT,
    EMUEE_ERROR_PAGE_UNFORMATTED,
    EMUEE_ERROR_PAGE_FULL,
} EmuEEError;

/**
 * Description of struct members:
 * 
 */

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

EmuEEError EmuEE_Init(uint32_t page0Address, uint32_t page1Address, 
    uint16_t pageSizeInBytes, EmuEEFlashWordSize flashWordSize, bool isLittleEndian);

EmuEEError EmuEE_GetFormat(uint32_t pageAddress, EmuEEFormat *retFormatType);

void EmuEE_Format(uint32_t page0Address, uint32_t page1Address, EmuEEFormat format);

uint16_t EmuEE_AddEntry(uint8_t *src, uint16_t size); // returns virtual address

uint16_t EmuEE_GetEntrySizeBytes(uint16_t virtualAddress);

EmuEEError EmuEE_Read(uint16_t virtualAddress, uint8_t *dst, uint16_t dstSize);

EmuEEError EmuEE_Write(uint16_t virtualAddress, uint8_t *src, uint16_t srcSize);


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////



#endif  /* IEMUEE_H */