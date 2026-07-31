#include "types.h"
/* CommonModel::DoSetFile at 0x02016144, size=0x70
 * Loads model data from a BMD file, optionally sets material opacity and polygon ID.
 */
struct ModelComponents {
    void* modelFile;
    void* materials;
    void* bones;
    void* transforms;
    void* unk10;
};

struct CommonModel {
    void* vtable;
    void* unk04;
    struct ModelComponents* data;
    char mat4x3[0x30];
};

extern struct ModelComponents* Func_02016e70(void* file);
extern void _ZN15ModelComponents21UpdateVertsUsingBonesEv(struct ModelComponents* data);
extern void _ZN11CommonModel13Func_020160ACEj(struct CommonModel* thiz, u32 arg0);
extern void _ZN11CommonModel13Func_0201609CEj(struct CommonModel* thiz, u32 arg0);

s32 _ZN11CommonModel9DoSetFileEPcii(struct CommonModel* thiz, void* file, s32 arg1, s32 arg2)
{
    thiz->data = Func_02016e70(file);
    if (thiz->data == (void*)0)
        return 0;
    _ZN15ModelComponents21UpdateVertsUsingBonesEv(thiz->data);
    if (arg1 != 0)
        _ZN11CommonModel13Func_020160ACEj(thiz, 0x8000);
    if (arg2 < 0)
        return 1;
    _ZN11CommonModel13Func_0201609CEj(thiz, (u32)arg2 & 0xff);
    return 1;
}
