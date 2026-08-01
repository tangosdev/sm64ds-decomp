#include "types.h"
/* BlendModelAnim::DoSetFile at 0x02016604 */
struct BlendModelAnim {
    u32 vtable;
};

extern s32 _ZN5Model9DoSetFileEPcii(struct BlendModelAnim *self, char *file, s32 arg2, s32 arg3);
extern s32 func_020165c4(struct BlendModelAnim *self, char *file);

s32 _ZN14BlendModelAnim9DoSetFileEPcii(struct BlendModelAnim *self, char *file, s32 arg2, s32 arg3)
{
    char *f;
    s32 r;

    f = file;
    r = _ZN5Model9DoSetFileEPcii(self, file, arg2, arg3);
    if (r == 0)
        return 0;

    return func_020165c4(self, f);
}
