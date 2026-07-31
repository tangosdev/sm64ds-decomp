// @symbol _ZN5Model8LoadFileER13SharedFilePtr
/* recovered: named members + shared header */
#include "Model.h"
/* Model::LoadFile (static) at 0x02017a3c
 * Loads a SharedFilePtr; if it's the first reference and file is non-null,
 * calls UpdateFileOffsets, AddToCommonModelDataArr, and ReallocateModelFile.
 * Returns the BMD_File pointer.
 */

typedef unsigned char u8;

struct SharedFilePtr {
    unsigned short fileID;  /* 0x00 */
    u8 numRefs;             /* 0x02 */
    char pad;               /* 0x03 */
    void *filePtr;          /* 0x04 */
};

struct BMD_File;

extern void _ZN13SharedFilePtr8LoadFileEv(struct SharedFilePtr *sfp);
extern void _ZN5Model17UpdateFileOffsetsER8BMD_File(struct BMD_File *file);
extern void _ZN5Model23AddToCommonModelDataArrER8BMD_File(struct BMD_File *file);
extern void _ZN13SharedFilePtr19ReallocateModelFileEv(struct SharedFilePtr *sfp);

void *_ZN5Model8LoadFileER13SharedFilePtr(struct SharedFilePtr *sfp)
{
    void *filePtr;
    _ZN13SharedFilePtr8LoadFileEv(sfp);
    filePtr = sfp->filePtr;
    if (sfp->numRefs == 1 && filePtr != 0)
    {
        _ZN5Model17UpdateFileOffsetsER8BMD_File((struct BMD_File *)filePtr);
        _ZN5Model23AddToCommonModelDataArrER8BMD_File((struct BMD_File *)filePtr);
        _ZN13SharedFilePtr19ReallocateModelFileEv(sfp);
    }
    return filePtr;
}
