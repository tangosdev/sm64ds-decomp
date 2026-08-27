#include "types.h"
struct SharedFilePtr { u16 fileID; u8 numRefs; char* filePtr; };
struct BMD_File { u32 data; };
struct ModelBase { u32 vtable; u32 unk04; };

extern struct BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(struct SharedFilePtr *filePtr);
extern s32 _ZN9ModelBase7SetFileEP8BMD_Fileii(struct ModelBase *self, struct BMD_File *file, s32 arg1, s32 arg2);
extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *self);

s32 func_020179b4(struct SharedFilePtr *r0, struct ModelBase *r1, s32 r2)
{
    struct ModelBase *model = r1;
    struct SharedFilePtr *filePtr = r0;
    s32 extra = r2;
    s32 result = 0;
    struct BMD_File *file;
    if (!model) goto done;
    file = _ZN5Model8LoadFileER13SharedFilePtr(filePtr);
    if (!file) goto done;
    result = _ZN9ModelBase7SetFileEP8BMD_Fileii(model, file, extra, -1);
    if (!result) {
        _ZN13SharedFilePtr7ReleaseEv(filePtr);
    }
done:
    return result;
}
