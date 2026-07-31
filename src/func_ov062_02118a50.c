// @symbol func_ov062_02118a50
/* recovered: shared common types, declarations from a shared header */
#include "decl_Animation.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
typedef short s16;
typedef unsigned short u16;

extern int _ZNK12WithMeshClsn8IsOnWallEv(void *w);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *s, struct Vector3 *v);
extern s16 _ZN5Actor12ReflectAngleE5Fix12IiES1_s(void *self, int a, int b, s16 ang);
extern int _ZN9Animation8FinishedEv(void *anim);

void func_ov062_02118a50(char *c)
{
    if (*(int *)(c + 0x98) != 0) {
        if (_ZNK12WithMeshClsn8IsOnWallEv(c + 0x144) != 0) {
            void *sr = _ZNK12WithMeshClsn13GetWallResultEv(c + 0x144);
            _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)sr + 4, (struct Vector3 *)(c + 0xe0));
            *(s16 *)(c + 0x94) = _ZN5Actor12ReflectAngleE5Fix12IiES1_s(
                c, *(int *)(c + 0xe0), *(int *)(c + 0xe8), *(s16 *)(c + 0x94));
        }
        func_ov062_02118004(c, 0x4cc);
        return;
    }

    {
        if (*(u16 *)(c + 0x3c6) != 0) {
            u16 *p = (u16 *)(((long long)(int)(c + 0x3c6)));
            *p = (u16)(*p - 1);
            if (*(u16 *)((c + 0x300) + 0xc6) != 0) return;
            func_ov062_02117994(c, 8); return;
        }
    }

    if (_ZNK9Animation12WillHitFrameEi(c + 0x350, 0x1e) != 0)
        func_ov062_021175c0(c);
    if (_ZN9Animation8FinishedEv(c + 0x350) == 0)
        return;
    *(int *)(c + 0x38c) = 1;
    func_ov062_02117994(c, 2);
}
