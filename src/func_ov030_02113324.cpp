//cpp
// NONMATCHING: different op / idiom (div=46). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct Vector3 { int x, y, z; };
struct Mtx43 { int a[12]; };

extern "C" int _ZN5Actor17DetectRaycastClsnER7Vector3S1_b(void* self, Vector3* a, Vector3* out, int doStore);
extern "C" int Vec3_Dist(const Vector3* a, const Vector3* b);
extern "C" void func_ov030_021141a8(char* c, int v);
extern "C" int _ZN6Player9StartTalkER9ActorBaseb(void* self, void* actor, int b);
extern "C" short Vec3_HorzAngle(const Vector3* v0, const Vector3* v1);
extern "C" void Matrix4x3_FromTranslation(Mtx43* m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationY(Mtx43* m, short angY);
extern "C" void Matrix4x3_ApplyInPlaceToTranslation(Mtx43* m, int x, int y, int z);
extern "C" int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void* self, void* actor, unsigned int msgId, const Vector3* pos, unsigned int d, unsigned int e);
extern "C" int func_0201267c(int a, void* b);
extern "C" int _ZN6Player12GetTalkStateEv(void* self);
extern "C" void _ZN6Player9DropActorEv(void* self);
extern "C" void _ZN9Animation7AdvanceEv(void* self);
extern "C" void _ZN12CylinderClsn5ClearEv(void* self);
extern "C" unsigned char DecIfAbove0_Byte(unsigned char* p);

extern void* CAMERA;
extern Mtx43 MATRIX_SCRATCH_PAPER;
extern unsigned char MESSAGE_RESULT;

extern "C" int func_ov030_02113324(void* thiz)
{
    char *c = (char*)thiz;

    *(short*)(c + 0x8e) = *(short*)((char*)(*(void**)(c + 0x3a8)) + 0x8e);
    *(short*)(c + 0x94) = *(short*)(c + 0x8e);

    {
        unsigned int flags = *(unsigned int*)(c + 0xb0);
        int f1 = (flags & 0x100) != 0;
        if (!f1)
            goto do_raycast;
        {
            int f2 = (flags & 0x2000) != 0;
            if (f2)
                goto do_raycast;
        }
    }
    goto skip_raycast;
do_raycast:
    {
        char *other = (char*)(*(void**)(c + 0x3a8));
        Vector3 v;
        int oy = *(int*)(other + 0x60);
        int oz = *(int*)(other + 0x64);
        v.x = *(int*)(other + 0x5c);
        v.y = oy + 0x32000;
        v.z = oz;
        _ZN5Actor17DetectRaycastClsnER7Vector3S1_b(c, &v, (Vector3*)(c + 0x5c), 1);

        if (Vec3_Dist((Vector3*)(c + 0x380), (Vector3*)(c + 0x5c)) < 0x514000 &&
            *(int*)(c + 0x60) > *(int*)(c + 0x384) - 0x12c000) {
            func_ov030_021141a8(c, 1);
        } else {
            func_ov030_021141a8(c, 9);
        }
        *(void**)(c + 0x3a8) = 0;
        return 1;
    }
skip_raycast:
    switch (*(unsigned char*)(c + 0x3c7)) {
    case 0:
        if (Vec3_Dist((Vector3*)(c + 0x380), (Vector3*)(c + 0x5c)) < 0x514000 &&
            *(int*)(c + 0x60) > *(int*)(c + 0x384) - 0x12c000) {
            if (_ZN6Player9StartTalkER9ActorBaseb(*(void**)(c + 0x3a8), c, 1) != 0) {
                Vector3 camPos;
                {
                    Vector3 *src = (Vector3*)((char*)CAMERA + 0x8c);
                    camPos.x = src->x;
                    camPos.y = src->y;
                    camPos.z = src->z;
                }
                short ang = Vec3_HorzAngle(&camPos, (Vector3*)((char*)(*(void**)(c + 0x3a8)) + 0x5c));
                {
                    Vector3 *op = (Vector3*)((char*)(*(void**)(c + 0x3a8)) + 0x5c);
                    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, op->x, op->y, op->z);
                }
                Matrix4x3_ApplyInPlaceToRotationY(&MATRIX_SCRATCH_PAPER, ang);
                Matrix4x3_ApplyInPlaceToTranslation(&MATRIX_SCRATCH_PAPER, 0, 0, -0x64000);

                Vector3 msgPos;
                msgPos.x = MATRIX_SCRATCH_PAPER.a[9];
                msgPos.y = MATRIX_SCRATCH_PAPER.a[10];
                msgPos.z = MATRIX_SCRATCH_PAPER.a[11];
                msgPos.y = *(int*)(c + 0x60) + 0x64000;

                if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(*(void**)(c + 0x3a8), c, 0xc0, &msgPos, 0, 2) != 0) {
                    func_0201267c(0xd1, c + 0x74);
                    {
                        unsigned char *p = (unsigned char*)(c + 0x3c7);
                        (*p)++;
                    }
                }
            }
        }
        *(unsigned char*)(c + 0x3c6) = 0x3c;
        break;
    case 1:
        if (_ZN6Player12GetTalkStateEv(*(void**)(c + 0x3a8)) == -1) {
            unsigned char g = MESSAGE_RESULT;
            if (g == 1) {
                _ZN6Player9DropActorEv(*(void**)(c + 0x3a8));
                func_ov030_021141a8(c, 8);
            } else if (g == 2) {
                unsigned char *p = (unsigned char*)(c + 0x3c7);
                (*p)++;
            }
        }
        break;
    case 2:
        if (DecIfAbove0_Byte((unsigned char*)(c + 0x3c6)) == 0)
            *(unsigned char*)(c + 0x3c7) = 0;
        break;
    }

    _ZN9Animation7AdvanceEv(c + 0x124);
    _ZN12CylinderClsn5ClearEv(c + 0x160);
    return 1;
}
