//cpp
// NONMATCHING: size 0x304-0x308 vs 0x310; matrix/speed/regperm residual
typedef void (*VoidFn)();

extern "C" {
extern int _ZN9Animation8FinishedEv(void* a);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* m, int ang);
extern void MulMat4x3Mat4x3(void* d, void* a, void* b);
extern void SubVec3(void* d, void* a, void* b);
extern void Vec3_LslInPlace(void* v, int sh);
extern void AddVec3(void* d, void* a, void* b);
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int a, unsigned int b, int x, int y, int z, const void* v, void* cb);
extern void _ZN9Animation7AdvanceEv(void* a);
extern void func_ov089_02131f54(void* c);
extern void _ZN9ActorBase18MarkForDestructionEv(void* c);
extern int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(void* c, void* w);
extern void _ZN12CylinderClsn5ClearEv(void* c);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* c, void* cyl);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void* c, void* v);
extern void _ZN12CylinderClsn6UpdateEv(void* c);

extern char data_020a0e68;
extern int data_ov089_02132c40[];
extern int data_ov089_02132b40[];
extern int data_ov089_02132ca4[];
}

struct C { virtual void dummy(); };
typedef void (C::*PMF)();
struct PmfEntry { PMF pmf; };
extern PmfEntry data_ov089_02132cec[];
#define PMFTABLE data_ov089_02132cec

extern "C" int _ZN3Key8BehaviorEv(char* c)
{
    int vec[3];
    int p7[3];
    int pe[3];
    int v = *(int*)(c + 0x448);

    if (v != 0) {
        if (v == 3) {
            {
                char* o = *(char**)(c + 0x110);
                if (o != 0) {
                    int* s = (int*)(o + 0x5c);
                    *(int*)(c + 0x5c) = s[0];
                    *(int*)(c + 0x60) = s[1];
                    *(int*)(c + 0x64) = s[2];
                    {
                        char* o2 = *(char**)(c + 0x110);
                        int ang = *(short*)(o2 + 0x8e);
                        *(short*)(c + 0x8e) = ang;
                    }
                }
            }
            if (_ZN9Animation8FinishedEv(c + 0x164) == 0) {
                Matrix4x3_FromTranslation(&data_020a0e68, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
                Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(c + 0x8e));
                MulMat4x3Mat4x3(*(void**)(c + 0x128), &data_020a0e68, &data_020a0e68);
                {
                    char* m = &data_020a0e68;
                    vec[0] = *(int*)(m + 0x24);
                    vec[1] = *(int*)(m + 0x28);
                    vec[2] = *(int*)(m + 0x2c);
                }
                SubVec3(vec, c + 0x5c, vec);
                Vec3_LslInPlace(vec, 3);
                AddVec3(vec, c + 0x5c, vec);
                {
                    int y = vec[1];
                    int sc = *(int*)(*(char**)(c + 0x124) + 0xc);
                    y = sc * 0x23 + y;
                    y = y - 0x48000;
                    vec[1] = y;
                }
                *(void**)(c + 0x464) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*(unsigned int*)(c + 0x464), 0x82, vec[0], vec[1], vec[2], 0, 0);
                *(void**)(c + 0x468) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*(unsigned int*)(c + 0x468), 0x83, vec[0], vec[1], vec[2], 0, 0);
            }
        }

        _ZN9Animation7AdvanceEv(c + 0x164);
        func_ov089_02131f54(c);
        if (_ZN9Animation8FinishedEv(c + 0x164)) {
            int b = (*(unsigned short*)(c + 0xc) == 0x11a);
            if (b != 0) {
                if (*(int*)(c + 0x174) != data_ov089_02132c40[1])
                    _ZN9ActorBase18MarkForDestructionEv(c);
            }
        }
        return 1;
    }

    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(c, c + 0x260)) {
        func_ov089_02131f54(c);
        _ZN12CylinderClsn5ClearEv(c + 0x220);
        return 1;
    }
    *(int*)(c + 0xd0) = 0;
    {
        short* spd = (short*)(c + 0x440);
        short s = *spd;
        if (s > 0x400) {
            *spd = s - 0x100;
        } else if (s == 0) {
            *spd = 0x400;
        }
    }
    {
        short* ang = (short*)(c + 0x8e);
        *ang = *ang + *(short*)(c + 0x440);
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
    (((C*)c)->*PMFTABLE[*(int*)(c + 0x444)].pmf)();
    func_ov089_02131f54(c);
    _ZN12CylinderClsn5ClearEv(c + 0x220);
    if (*(int*)(c + 0x444) == 7) {
        p7[0] = data_ov089_02132b40[0];
        p7[1] = data_ov089_02132b40[1];
        p7[2] = data_ov089_02132b40[2];
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x220, p7);
    } else {
        pe[0] = data_ov089_02132ca4[0];
        pe[1] = data_ov089_02132ca4[1];
        pe[2] = data_ov089_02132ca4[2];
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x220, pe);
    }
    _ZN12CylinderClsn6UpdateEv(c + 0x220);
    return 1;
}
