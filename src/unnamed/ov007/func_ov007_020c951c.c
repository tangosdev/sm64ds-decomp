/* Bind the ov007 front-end message bank's INF1 and DAT1 sections.
 * ROM code reads sectionCount at +0x0c, starts sections at +0x20,
 * skips each section by its stored size, and ignores the other three tags.
 * Constants are the words compared by the ARM code: do not reverse them.
 * No ownership/validation is added: absent sections retain their prior values.
 * A caller supplies a third diagnostic-name argument; retail does not read it.
 */
#include "types.h"
extern void *data_ov007_02104c20; /* bank header */
extern void *data_ov007_02104c24; /* DAT1 payload */
extern void *data_ov007_02104c1c; /* INF1 header */
extern void *data_ov007_02104c18; /* INF1 entries */

struct Header {
    u32 opaque[3];
    u32 sectionCount;
};
struct Chunk {
    u32 tag;
    u32 size;
};
enum MessageSectionTag {
    MESSAGE_INFO = 0x494e4631,
    MESSAGE_FLOW = 0x464c5731,
    MESSAGE_DATA = 0x44415431,
    MESSAGE_STRINGS = 0x53545231,
    MESSAGE_FLOW_INDEX = 0x464c4931
};

void func_ov007_020c951c(int unused, struct Header *bank) {
    u32 sectionIndex;
    char *section = (char *)bank;
    data_ov007_02104c20 = bank;
    section += 0x20;
    for (sectionIndex = 0; sectionIndex < bank->sectionCount; sectionIndex++) {
        switch (((struct Chunk *)section)->tag) {
        case MESSAGE_INFO:
            data_ov007_02104c1c = section;
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
        section += ((struct Chunk *)section)->size;
    }
}
