// NONMATCHING: different op / idiom (div=51). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* bmd, int a, int b);
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* bca, int a, int fx, unsigned int f);
extern void _ZN11ShadowModel10InitCuboidEv(void* thiz);
extern void func_ov102_02149da8(void* c, int i);
extern void func_ov102_02149ff0(char* c);
extern void func_ov102_02149e38(char* self);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, void* kcl, void* mtx, int fix, short s, void* clps);
extern unsigned char _ZN5Actor9TrackStarEjj(void* c, unsigned int a, unsigned int b);

extern char data_ov102_0214e7e8, data_ov102_0214e808, data_ov102_0214e7f8;
extern char BOB_OMB_MODEL_PTR, data_ov102_0214e800, data_ov102_0214e7f0;
extern char MARIO_CAP_MODEL_PTR, data_ov102_0214e7d8, LUIGI_CAP_MODEL_PTR;
extern char data_ov102_0214e7e0, WARIO_CAP_MODEL_PTR, data_ov102_0214e7d0;
extern char data_ov002_0210d954, FEATHER_MODEL_PTR, SHELL_MODEL_PTRS;
extern char ONE_UP_MUSHROOM_MODEL_PTR, SUPER_MUSHROOM_MODEL_PTR, POWER_FLOWER_OPEN_MODEL_PTR;
extern char POWER_FLOWER_CLOSED_MODEL_PTR;

struct M12 { int w[12]; };

int func_ov102_0214a4a4(char* c)
{
    void* r5 = 0;
    switch (*(unsigned short*)(c + 0xc) - 0x14) {
    case 0:
        r5 = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov102_0214e7e8);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x320, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov102_0214e808), 1, 0x19);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x320, _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov102_0214e7f8), 0, 0x1000, 0);
        _ZN5Model8LoadFileER13SharedFilePtr(&BOB_OMB_MODEL_PTR);
        break;
    case 1:
    case 2:
        r5 = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov102_0214e800);
        break;
    case 3:
        r5 = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov102_0214e7f0);
        _ZN5Model8LoadFileER13SharedFilePtr(&MARIO_CAP_MODEL_PTR);
        _ZN5Model8LoadFileER13SharedFilePtr(&BOB_OMB_MODEL_PTR);
        break;
    case 5:
        r5 = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov102_0214e7d8);
        _ZN5Model8LoadFileER13SharedFilePtr(&LUIGI_CAP_MODEL_PTR);
        _ZN5Model8LoadFileER13SharedFilePtr(&BOB_OMB_MODEL_PTR);
        break;
    case 4:
        r5 = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov102_0214e7e0);
        _ZN5Model8LoadFileER13SharedFilePtr(&WARIO_CAP_MODEL_PTR);
        _ZN5Model8LoadFileER13SharedFilePtr(&BOB_OMB_MODEL_PTR);
        break;
    }

    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, r5, 1, -1);
    _ZN11ShadowModel10InitCuboidEv(c + 0x384);
    func_ov102_02149da8(c, 0);
    *(int*)(c + 0xa0) = -0x3c000;
    *(int*)(c + 0x80) = 0x1000;
    *(int*)(c + 0x84) = 0x1000;
    *(int*)(c + 0x88) = 0x1000;
    func_ov102_02149ff0(c);
    func_ov102_02149e38(c);
    *(struct M12*)(c + 0x3ac) = *(struct M12*)(c + 0xf0);
    {
        void* kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov102_0214e7d0);
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c + 0x124, kcl, c + 0x2ec, 0x199, *(short*)(c + 0x8e), &data_ov002_0210d954);
    }
    *(int*)(c + 0x3e0) = *(int*)(c + 0x60);
    *(unsigned char*)(c + 0x3f3) = (unsigned char)*(int*)(c + 8);
    if (*(unsigned char*)(c + 0x3f3) == 0xff)
        *(unsigned char*)(c + 0x3f3) = 0;
    if (*(unsigned char*)(c + 0x3f3) == 1) {
        *(unsigned char*)(c + 0x3f1) = (unsigned char)(*(unsigned int*)(c + 8) >> 8);
        if (*(unsigned char*)(c + 0x3f1) == 0xff)
            *(unsigned char*)(c + 0x3f1) = 0;
        *(unsigned char*)(c + 0x3f0) = _ZN5Actor9TrackStarEjj(c, *(unsigned char*)(c + 0x3f1), 2);
    }

    {
        int b16 = (*(unsigned short*)(c + 0xc) == 0x16);
        if (b16 != 0) {
            *(unsigned char*)(c + 0x3f3) = 0;
            _ZN5Model8LoadFileER13SharedFilePtr(&FEATHER_MODEL_PTR);
        }
    }

    {
        int b14 = (*(unsigned short*)(c + 0xc) == 0x14);
        if (b14 == 0) {
            int b15 = (*(unsigned short*)(c + 0xc) == 0x15);
            if (b15 == 0)
                return 1;
        }
    }

    switch (*(unsigned char*)(c + 0x3f3)) {
    case 0:
    case 1:
        break;
    case 3:
        _ZN5Model8LoadFileER13SharedFilePtr(&SHELL_MODEL_PTRS);
        break;
    case 2:
        _ZN5Model8LoadFileER13SharedFilePtr(&ONE_UP_MUSHROOM_MODEL_PTR);
        break;
    case 4:
        _ZN5Model8LoadFileER13SharedFilePtr(&SUPER_MUSHROOM_MODEL_PTR);
        break;
    case 6:
        _ZN5Model8LoadFileER13SharedFilePtr(&POWER_FLOWER_OPEN_MODEL_PTR);
        _ZN5Model8LoadFileER13SharedFilePtr(&POWER_FLOWER_CLOSED_MODEL_PTR);
        _ZN5Model8LoadFileER13SharedFilePtr(&BOB_OMB_MODEL_PTR);
        break;
    case 5:
        _ZN5Model8LoadFileER13SharedFilePtr(&FEATHER_MODEL_PTR);
        _ZN5Model8LoadFileER13SharedFilePtr(&POWER_FLOWER_OPEN_MODEL_PTR);
        _ZN5Model8LoadFileER13SharedFilePtr(&POWER_FLOWER_CLOSED_MODEL_PTR);
        break;
    case 7:
        _ZN5Model8LoadFileER13SharedFilePtr(&POWER_FLOWER_OPEN_MODEL_PTR);
        _ZN5Model8LoadFileER13SharedFilePtr(&POWER_FLOWER_CLOSED_MODEL_PTR);
        break;
    }
    return 1;
}
