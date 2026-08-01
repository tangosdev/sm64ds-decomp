#include "types.h"
// @symbol _ZN15TextureSequence8LoadFileER13SharedFilePtr
/* recovered: named members + shared header */
#include "TextureSequence.h"
/* TextureSequence::LoadFile (static) at 0x020178e4
 * Loads a SharedFilePtr; if first reference and file is non-null, calls TextureSequence::UpdateFileOffsets.
 * Returns the BTP_File pointer.
 */
struct SharedFilePtr {
    unsigned short fileID;  /* 0x00 */
    u8 numRefs;             /* 0x02 */
    char pad;               /* 0x03 */
    void *filePtr;          /* 0x04 */
};

struct BTP_File;

extern void _ZN13SharedFilePtr8LoadFileEv(struct SharedFilePtr *sfp);
extern void _ZN15TextureSequence17UpdateFileOffsetsER8BTP_File(struct BTP_File *file);

void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(struct SharedFilePtr *sfp)
{
    void *filePtr;
    _ZN13SharedFilePtr8LoadFileEv(sfp);
    filePtr = sfp->filePtr;
    if (sfp->numRefs == 1 && filePtr != 0)
    {
        _ZN15TextureSequence17UpdateFileOffsetsER8BTP_File((struct BTP_File *)filePtr);
    }
    return filePtr;
}
