// @symbol _ZN12MeshCollider8LoadFileER13SharedFilePtr
/* recovered: named members + shared header */
#include "MeshCollider.h"
typedef unsigned char u8;

struct SharedFilePtr {
    unsigned short fileID;
    u8 numRefs;
    char* filePtr;
};

extern void _ZN13SharedFilePtr8LoadFileEv(struct SharedFilePtr* fp);
extern void _ZN12MeshCollider17UpdateFileOffsetsER8KCL_File(char* file);

char* _ZN12MeshCollider8LoadFileER13SharedFilePtr(struct SharedFilePtr* filePtr)
{
    u8 refs;
    char* file;

    _ZN13SharedFilePtr8LoadFileEv(filePtr);
    refs = filePtr->numRefs;
    file = filePtr->filePtr;
    if (refs == 1 && file != 0) {
        _ZN12MeshCollider17UpdateFileOffsetsER8KCL_File(file);
    }
    return file;
}
