#include "types.h"
// @symbol func_ov062_02116010
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern void *_ZN8dActor_c10FindWithIDEj(u32 id);
extern void _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(void *c, void *v, void *a, int flag);
extern void func_02012694(int a, void *p);
extern int _ZN8dActor_c18HorzAngleToCPlayerEv(void *c);
extern int _ZN6Player7TryGrabER8dActor_c(void *p, void *a);
extern void Chuckya_ChangeState(void *c, void *p);
extern int func_ov002_020db5f4(void *c, void *arg);
extern int data_ov062_0211dea0[];



void func_ov062_02116010(char *c)
{
    u32 id;
    void *a;
    int b;
    int angle;
    struct Vector3_16 v;

    id = *(u32 *)(c + 0x134);
    if (id != 0 &&
        (a = _ZN8dActor_c10FindWithIDEj(id)) != 0 &&
        (*(int *)(c + 0x130) & 0x10) != 0) {
        v.x = 0x1000;
        v.y = 0;
        v.z = 0;
        _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(c, &v, a, 0);
        func_02012694(0x125, c + 0x74);
        return;
    }

    *(int *)(((long long)(int)(c + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x80;

    id = *(u32 *)(c + 0x134);
    if (id == 0)
        return;
    a = _ZN8dActor_c10FindWithIDEj(id);
    if (a == 0)
        return;
    b = (int)(*(u16 *)((char *)a + 0xc) == 0xbf);
    if (b == 0)
        return;
    angle = _ZN8dActor_c18HorzAngleToCPlayerEv(c);
    angle = AngleDiff(angle, *(s16 *)(c + 0x8e));
    if (angle > 0x2000) {
        if ((*(int *)(c + 0x130) & 0x1000) == 0)
            return;
        *(int *)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFFULL) |= 0x80;
        if (_ZN6Player7TryGrabER8dActor_c(a, c) == 0)
            return;
        *(int *)(c + 0x3f8) = (int)a;
        *(int *)(((long long)(int)(c + 0x128)) & 0xFFFFFFFFFFFFFFFFLL) |= 2;
        *(int *)(c + 0x98) = 0;
        Chuckya_ChangeState(c, data_ov062_0211dea0);
        return;
    }
    if (func_ov002_020db5f4(a, c) == 0)
        return;
    *(int *)(c + 0x3f8) = (int)a;
    *(int *)(c + 0x98) = 0;
    Chuckya_ChangeState(c, data_ov062_0211def0);
}
