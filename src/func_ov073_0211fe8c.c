// @symbol func_ov073_0211fe8c
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
struct ActorBase;

extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int c, struct Vector3* v, unsigned int d);
extern void* _ZN5Actor13ClosestPlayerEv(void* actor);
extern void _ZN6Player9StartTalkER9ActorBaseb(void* self, void* actor, int b);
extern int _ZN5Actor18HorzAngleToCPlayerEv(void* actor);
extern void _ZN6Camera9SetFlag_3Ev(void* cam);
extern void Matrix4x3_FromRotationY(void* m, short ang);
extern void MulVec3Mat4x3(void* a, void* m, void* b);
extern void _ZN6Camera9SetLookAtERK7Vector3(void* cam, struct Vector3* v);
extern void _ZN6Camera6SetPosERK7Vector3(void* cam, struct Vector3* v);
extern int ChiefChilly_ChangeState(void* c, void* p);

extern void* data_0209f318;
extern int data_020a0e68[];

int func_ov073_0211fe8c(char* c) {
    struct Vector3 look, pos, in, out;
    void* player;
    void* cam;

    *(unsigned int*)(c + 0x500) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(unsigned int*)(c + 0x500), 3, 0x170, (struct Vector3*)(c + 0x74), 0);

    player = _ZN5Actor13ClosestPlayerEv(c);
    if (player != 0 && *(unsigned char*)((char*)player + 0x6de) == 0) {
        int angle;
        _ZN6Player9StartTalkER9ActorBaseb(player, c, 1);
        angle = _ZN5Actor18HorzAngleToCPlayerEv(c);
        *(short*)((char*)player + 0x8c) = 0;
        *(short*)((char*)player + 0x8e) = angle + 0x8000;
        *(short*)((char*)player + 0x90) = 0;
    }

    func_ov073_0211f144(c);

    cam = data_0209f318;
    _ZN6Camera9SetFlag_3Ev(cam);

    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;

    look.x = *(int*)(c + 0x5c);
    look.y = *(int*)(c + 0x60);
    look.z = *(int*)(c + 0x64);
    pos.x = *(int*)(c + 0x3d8);
    pos.y = *(int*)(c + 0x3dc);
    pos.z = *(int*)(c + 0x3e0);

    look.y += 0x100000;
    in.z = -0x59c000;

    Matrix4x3_FromRotationY(data_020a0e68, *(short*)(c + 0x8e));
    MulVec3Mat4x3(&in, data_020a0e68, &out);

    pos.y -= 0x100000;
    pos.x = pos.x + out.x;
    pos.z = pos.z + out.z;

    _ZN6Camera9SetLookAtERK7Vector3(cam, &look);
    _ZN6Camera6SetPosERK7Vector3(cam, &pos);

    if (*(unsigned short*)(c + 0x100) == 0) {
        ChiefChilly_ChangeState(c, &data_ov073_021233e0);
    }
    return 1;
}
