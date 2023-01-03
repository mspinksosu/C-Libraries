/***************************************************************************//**
 * @brief Foo Interface
 * 
 * @author Matthew Spinks
 * 
 * @date 12/24/22  Original creation
 * 
 * @file IEmuEE.c
 * 
 * @details
 *      TODO
 * 
 ******************************************************************************/

#include "IEmuEE.h"

// ***** Defines ***************************************************************

#define EMUEE_PAGE_STATUS_UNKOWN    0
#define EMUEE_PAGE_STATUS_ERASED    0xFFFF
#define EMUEE_PAGE_STATUS_VALID     0xAAAA
#define EMUEE_PAGE_STATUS_RECEIVE   0xCCCC

#define EMUEE_ENTRY_STATUS_MASK     0xC000

// ***** Global Variables ******************************************************

/* The first eight bytes of the flash page will contain a header which has 
information about the page status and its format. */
enum
{
    EMUEE_PAGE_STATUS_BYTE_LOW = 0,
    EMUEE_PAGE_STATUS_BYTE_HIGH,
    EMUEE_PAGE_FORMAT_BYTE,
    EMUEE_PAGE_PAD_0,
    EMUEE_PAGE_SIZE_IN_BYTES_LOW,
    EMUEE_PAGE_SIZE_IN_BYTES_HIGH,
    EMUEE_PAGE_WORD_SIZE_BYTE,
    EMUEE_PAGE_PAD_1,
};

/* The first two bytes of an entry will be a header which contains its status
and its size. Bits [15:14] of an entry will hold the status bits. Bits [13:0] 
will be X.Y, where X is the virtual address and Y length of the entry in bytes 
(both right justified) */
enum
{
    EMUEE_ENTRY_HEADER_LOW = 0,
    EMUEE_ENTRY_HEADER_HIGH,
};

enum EmuEntryStatus // TODO leave as enum or change to define?
{
    EMUEE_ENTRY_STATUS_UNKOWN = 0,
    EMUEE_ENTRY_STATUS_VALID,
    EMUEE_ENTRY_STATUS_ERASED,
};

static EmuEEFormat emueeFormat = EMUEE_FORMAT_7_7;
static EmuEEFlashWordSize emueeFlashWordSize = EMUEE_WORD_SIZE_2_BYTES;
static uint16_t emueePageSizeInBytes = EMUEE_PAGE_SIZE_1KB;
static bool littleEndian = true;
static uint32_t emueePage0Address, emueePage1Address;

// TODO check virtual address against format and page size to find end of page


// ***** Static Function Prototypes ********************************************

static enum EmuEntryStatus GetEntryStatus(uint16_t header);
static uint16_t GetEntryAddress(uint16_t header);
static uint16_t GetEntrySize(uint16_t header);

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Non-Interface Functions *********************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

EmuEEError EmuEE_Init(uint32_t page0Address, uint32_t page1Address, 
    uint16_t pageSizeInBytes, EmuEEFlashWordSize flashWordSize, bool isLittleEndian)
{
    // TODO check if address is in valid range
    if(page0Address == page1Address || page0Address == 0 || page1Address == 0
        || pageSizeInBytes < EMUEE_WORD_SIZE_2_BYTES)
    {
        return EMUEE_ERROR_INVALID_ADDRESS;
    }

    emueePage0Address = page0Address;
    emueePage1Address = page1Address;
    emueePageSizeInBytes = pageSizeInBytes;
    emueeFlashWordSize = flashWordSize;
    littleEndian = isLittleEndian;

    return EMUEE_ERROR_NONE;
}

EmuEEError EmuEE_GetFormat(uint32_t pageAddress, EmuEEFormat *retFormatType)
{
    EmuEEError error = 0;
    uint32_t dataBuffer[4], pageFormat = 0;

    // Read first eight bytes
    switch(emueeFlashWordSize)
    {
        case EMUEE_WORD_SIZE_2_BYTES:
            for(uint8_t i = 0; i < 4; pageAddress += 2)
            {
                error = EmuEE_ReadFlashWord(pageAddress, &dataBuffer[i]);
                if(error != EMUEE_ERROR_NONE)
                    break;
            }
            /* Get the format byte out of the word (byte 2) */
            if(error == EMUEE_ERROR_NONE)
            {
                if(littleEndian)
                {
                    pageFormat = dataBuffer[1] & 0x000000FF;
                }
                else
                {
                    pageFormat = (dataBuffer[1] >> 8) & 0x000000FF;
                }
            }
            break;
        case EMUEE_WORD_SIZE_4_BYTES:
            for(uint8_t i = 0; i < 2; pageAddress += 4)
            {
                error = EmuEE_ReadFlashWord(pageAddress, &dataBuffer[i]);
                if(error != EMUEE_ERROR_NONE)
                    break;
            }
            /* Get the format byte out of the word (byte 2) */
            if(error == EMUEE_ERROR_NONE)
            {
                if(littleEndian)
                {
                    pageFormat = (dataBuffer[0] >> 16) & 0x000000FF;
                }
                else
                {
                    pageFormat = (dataBuffer[0] >> 8) & 0x000000FF;
                }
            }
            break;
    }

    if(error != EMUEE_ERROR_NONE)
        return error;

    /* Check the entry format. All entries use 14 bits. Format_13_1 is not a 
    valid format because a flash word is going to be at least two bytes long */
    uint8_t upperNibble = pageFormat >> 4;
    uint8_t lowerNibble = pageFormat & 0x0F;
    if(upperNibble > 12 || (upperNibble + lowerNibble > 14))
    {
        error = EMUEE_ERROR_INVALID_FORMAT;
    }

    return error;
}

void EmuEE_Format(uint32_t page0Address, uint32_t page1Address, EmuEEFormat format)
{
    emueeFormat = format;

    // Write the page header
}

uint16_t EmuEE_AddEntry(uint8_t *src, uint16_t size)
{
    // returns virtual address
} 

uint16_t EmuEE_GetEntrySizeBytes(uint16_t virtualAddress)
{

}

EmuEEError EmuEE_Read(uint16_t virtualAddress, uint8_t *dst, uint16_t dstSize)
{

}

EmuEEError EmuEE_Write(uint16_t virtualAddress, uint8_t *src, uint16_t srcSize)
{

}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Interface Functions *************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


// *****************************************************************************


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ***** Local Functions *****************************************************//
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

static enum EmuEntryStatus GetEntryStatus(uint16_t header)
{
    return ((header & EMUEE_ENTRY_STATUS_MASK) >> 14);
}

/*
 End of File
 */