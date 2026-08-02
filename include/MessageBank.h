#ifndef MESSAGE_BANK_H
#define MESSAGE_BANK_H

#include "types.h"

/* The BMG-family message bank, as the five western msg_data_*.bin carry it.
 *
 * WHAT THE ROM PINS. ParseMessageBankSections (func_0201cb2c) does all of the
 * offset arithmetic this file encodes, and only this:
 *
 *   info    = base + 0x20                 -> the header is 0x20 bytes
 *   entries = base + 0x30 = info + 0x10   -> the info section header is 0x10
 *   data    = info + *(u32 *)(info + 4)   -> sectionSize sits at info + 4
 *
 * The magics are read out of the ROM and are stored 4-byte-reversed against
 * stock BMG: the image holds "GSEM1gmb" and "1FNI" five times each, once per
 * western bank, and "MESGbmg1"/"INF1" appear nowhere. Do not "correct" them.
 *
 * EVERYTHING ELSE BELOW IS INFERRED from the BMG family, not from this game's
 * code -- no ROM function reads these fields. Marked (?) and worth confirming
 * against a bank before anything depends on it.
 */
typedef struct MessageBankHeader {
    char magic[8];          /* 0x00 - "GSEM1gmb", confirmed in the image */
    u32 fileSize;           /* 0x08 - (?) */
    u32 sectionCount;       /* 0x0c - (?) */
    u32 unk_10;             /* 0x10 - (?) */
    u8 reserved_14[0x0c];   /* 0x14 - (?) pads the header to the pinned 0x20 */
} MessageBankHeader;

typedef struct MessageInfoSection {
    char magic[4];          /* 0x00 - "1FNI", confirmed in the image */
    u32 sectionSize;        /* 0x04 - pinned: the parser adds it to reach data */
    u16 messageCount;       /* 0x08 - (?) */
    u16 unk_0a;             /* 0x0a - (?) */
    u32 unk_0c;             /* 0x0c - (?) pads to the pinned 0x10 */
} MessageInfoSection;

/* (?) ENTIRELY INFERRED. The parser only computes where the entry array starts;
   nothing in the ROM reads an element, so both the field split and the 8-byte
   stride are BMG-family guesses. */
typedef struct MessageEntry {
    u32 textOffset;
    u8 info[4];
} MessageEntry;

/* (?) INFERRED apart from the magic. "1TAD" occurs three times in the image,
   not five, so the data section is not yet understood -- treat with care. */
typedef struct MessageDataSection {
    char magic[4];
    u32 sectionSize;
} MessageDataSection;

typedef char MessageBankHeader_SizeIs20[sizeof(MessageBankHeader) == 0x20 ? 1 : -1];
typedef char MessageInfoSection_SizeIs10[sizeof(MessageInfoSection) == 0x10 ? 1 : -1];
typedef char MessageEntry_SizeIs8[sizeof(MessageEntry) == 8 ? 1 : -1];
typedef char MessageDataSection_SizeIs8[sizeof(MessageDataSection) == 8 ? 1 : -1];

/* Readable aliases retain the current symbols and therefore their relocations. */
#define LoadMessageBankForLanguage   func_0201fe08
#define ParseMessageBankSections     func_0201cb2c
#define gMessageBankHeader           data_0209d6e8
#define gMessageDataSection          data_0209d6ec
#define gMessageBankBase             data_0209d6fc
#define gMessageEntries              data_0209d708
#define gMessageInfoSection          data_0209d70c

#define MESSAGE_BANK_ENG_ASSET   0x0431
#define MESSAGE_BANK_FRN_ASSET   0x0432
#define MESSAGE_BANK_GMN_ASSET   0x0433
#define MESSAGE_BANK_ITL_ASSET   0x0434
#define MESSAGE_BANK_SPN_ASSET   0x0435

#endif
