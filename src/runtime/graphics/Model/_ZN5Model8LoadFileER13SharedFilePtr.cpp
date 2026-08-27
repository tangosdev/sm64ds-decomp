//cpp
// @symbol _ZN5Model8LoadFileER13SharedFilePtr
#include "Model.h"

struct SharedFilePtr {
    u16 fileID;      /* 0x00 */
    u8 numRefs;      /* 0x02 */
    char pad;        /* 0x03 */
    void *filePtr;   /* 0x04 */

    void LoadFile();                 /* 0x0201285c */
    void ReallocateModelFile();      /* 0x020129cc */
};

void *Model::LoadFile(SharedFilePtr &ptr)
{
    void *filePtr;
    ptr.LoadFile();
    filePtr = ptr.filePtr;
    if (ptr.numRefs == 1 && filePtr != 0) {
        UpdateFileOffsets(*(BMD_File *)filePtr);
        AddToCommonModelDataArr(*(BMD_File *)filePtr);
        ptr.ReallocateModelFile();
    }
    return filePtr;
}
