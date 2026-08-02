//cpp
// @symbol _ZN15TextureSequence8LoadFileER13SharedFilePtr
#include "TextureSequence.h"

struct SharedFilePtr {
    u16 fileID;      /* 0x00 */
    u8 numRefs;      /* 0x02 */
    char pad;        /* 0x03 */
    void *filePtr;   /* 0x04 */

    void LoadFile();
};

void *TextureSequence::LoadFile(SharedFilePtr &ptr)
{
    void *filePtr;
    ptr.LoadFile();
    filePtr = ptr.filePtr;
    if (ptr.numRefs == 1 && filePtr != 0) {
        UpdateFileOffsets(*(BTP_File *)filePtr);
    }
    return filePtr;
}
