//cpp
/* unit020c94a0 -- ov007 unit 0x020c94a0..0x020c95fc: 3 functions, .text only.
 *
 * The menu's message bank. ov007 is the title and file-select menu program
 * that dScDSMT_c hosts; outside dScDSMT_c it has no ROM RTTI or vtable, so
 * no class name is claimed and the file keeps the unit<addr> name.
 *
 * What the code does. func_ov007_020c951c binds a message bank (a BMG-style
 * file: section count at +0x0c, sections from +0x20, each led by a tag and
 * its size) and keeps pointers to its INF1 and DAT1 sections;
 * func_ov007_020c94a0 turns a message index into the address of the
 * message's text; func_ov007_020c94e4 forgets the bank. The bank itself
 * belongs to the caller: nothing here allocates or frees it.
 *
 * Edges: the bss block 0x02104c18..0x02104c28 is referenced by these three
 * functions and nothing else in the overlay. Neither neighbour,
 * func_ov007_020c946c before or func_ov007_020c95fc after, touches it.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 * The functions keep their ROM names under C linkage.
 */
#include "types.h"

extern "C" {

/* The bank's file header; only the section count is read. */
typedef struct MessageBank {
    u32 opaque[3];
    u32 sectionCount; /* 0x0c */
} MessageBank;

/* The tag and size every section starts with. */
typedef struct MessageSection {
    u32 tag;
    u32 size;
} MessageSection;

/* The INF1 section header; the entries follow at +0x10. */
typedef struct MessageInfo {
    MessageSection section;
    u16 entryCount;
    /* the lookup scales the index by it and takes the result in bits */
    u16 entryStrideBits; /* 0x0a */
} MessageInfo;

/* Section tags, as the words the code compares. */
enum MessageSectionTag {
    MESSAGE_INFO = 0x494e4631,
    MESSAGE_FLOW = 0x464c5731,
    MESSAGE_DATA = 0x44415431,
    MESSAGE_STRINGS = 0x53545231,
    MESSAGE_FLOW_INDEX = 0x464c4931
};

extern char *data_ov007_02104c18;        /* INF1 entries */
extern MessageInfo *data_ov007_02104c1c; /* INF1 header */
extern MessageBank *data_ov007_02104c20; /* the bound bank */
extern char *data_ov007_02104c24;        /* DAT1 payload */

/* Every section but INF1 and DAT1 is skipped. A caller passes a third,
 * diagnostic-name argument that retail never reads; sections the bank lacks
 * keep their previous pointers. */
// @symbol func_ov007_020c951c
void func_ov007_020c951c(int unused, MessageBank *bank) {
    u32 sectionIndex;
    char *section = (char *)bank;
    data_ov007_02104c20 = bank;
    section += 0x20;
    for (sectionIndex = 0; sectionIndex < bank->sectionCount; sectionIndex++) {
        switch (((MessageSection *)section)->tag) {
        case MESSAGE_INFO:
            data_ov007_02104c1c = (MessageInfo *)section;
            data_ov007_02104c18 = section + 0x10;
            break;
        case MESSAGE_FLOW:
            break;
        case MESSAGE_DATA:
            data_ov007_02104c24 = section + 8;
            break;
        case MESSAGE_STRINGS:
            break;
        case MESSAGE_FLOW_INDEX:
            break;
        }
        section += ((MessageSection *)section)->size;
    }
}

// @symbol func_ov007_020c94e4
void func_ov007_020c94e4(void) {
    data_ov007_02104c20 = 0;
    data_ov007_02104c1c = 0;
    data_ov007_02104c18 = 0;
    data_ov007_02104c24 = 0;
}

/* Each INF1 entry starts with the message's offset into DAT1. The index is
 * narrowed to 16 bits before the multiply; the first argument is unused but
 * every caller passes it. */
// @symbol func_ov007_020c94a0
int func_ov007_020c94a0(int unused, unsigned int messageIndex) {
    unsigned short entryStrideBits = data_ov007_02104c1c->entryStrideBits;
    unsigned int entryBitOffset = (unsigned int)(entryStrideBits * (messageIndex & 0xffff));
    return (int)(data_ov007_02104c24 + *(int *)(data_ov007_02104c18 + (entryBitOffset >> 3)));
}
}
