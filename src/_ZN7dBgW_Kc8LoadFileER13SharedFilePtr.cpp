//cpp
// @symbol _ZN7dBgW_Kc8LoadFileER13SharedFilePtr
#include "dBgW_Kc.h"

struct SharedFilePtr {
    u16 fileID;      /* 0x00 */
    u8 numRefs;      /* 0x02 */
    char pad;        /* 0x03 */
    char *filePtr;   /* 0x04 */

    void LoadFile();
};

char *dBgW_Kc::LoadFile(SharedFilePtr &ptr)
{
    u8 refs;
    char *file;

    ptr.LoadFile();
    refs = ptr.numRefs;
    file = ptr.filePtr;
    if (refs == 1 && file != 0) {
        UpdateFileOffsets(*(KCL_File *)file);
    }
    return file;
}
