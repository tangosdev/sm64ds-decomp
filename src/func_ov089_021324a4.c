extern void LoadKeyModels(int idx);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void* sfp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* bmd, int a, int b);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* bca, int a, int fx, unsigned int f);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void* thiz, void* actor, void* pos, int r, int s, unsigned int a, unsigned int b);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* actor, int a, int b, void* v, void* w);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void* pos);
extern int _ZN11ShadowModel12InitCylinderEv(void* thiz);
extern void _ZN12WithMeshClsn13SetLimMovFlagEv(void* thiz);
extern void _ZN5Event8ClearBitEj(unsigned int n);

extern char data_ov002_02110964;
extern char POWER_STAR_MODEL_PTR;
extern char data_ov089_02132b40;
extern char data_ov089_02132c60;
extern char data_ov089_02132c40;
extern char data_ov089_02132c70;
extern char data_ov089_02132c48;
extern int data_ov089_02132894[];
extern int data_ov089_021328b4[];
extern char data_ov089_02132ca4;
extern int DAT_0209cef0;

int func_ov089_021324a4(char* c)
{
    int v[3];
    int v2[3];
    int kind = *(int*)(c + 8) & 7;
    *(int*)(c + 0x444) = kind;
    LoadKeyModels(*(int*)(c + 0x444));
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_02110964);

    if (*(int*)(c + 0x444) == 7) {
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x114, *(void**)(&POWER_STAR_MODEL_PTR + 4), 1, 1) == 0)
            return 0;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x114, *(void**)(&data_ov002_02110964 + 4), 0x40000000, 0x1000, 0);
        *(int*)(c + 0x80) = 0x2000;
        *(int*)(c + 0x84) = 0x2000;
        *(int*)(c + 0x88) = 0x2000;
        v[0] = *(int*)(&data_ov089_02132b40);
        v[1] = *(int*)(&data_ov089_02132b40 + 4);
        v[2] = *(int*)(&data_ov089_02132b40 + 8);
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c + 0x220, c, v, 0x50000, 0xfa000, 0x800003, 0x8000);
        _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x260, c, 0xa0000, 0, 0, 0);
        *(unsigned short*)(c + 0x440) = 0x400;
        *(int*)(c + 0x9c) = 0;
        _ZN5Sound9PlayBank3EjRK7Vector3(0x57, c + 0x74);
    } else {
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov089_02132c60);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov089_02132c40);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov089_02132c70);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov089_02132c48);
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x114, *(void**)((char*)data_ov089_02132894[*(int*)(c + 0x444)] + 4), 1, 1) == 0)
            return 0;
        {
            void* m = (void*)data_ov089_021328b4[*(int*)(c + 0x444)];
            if (m != 0) {
                if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x178, *(void**)((char*)m + 4), 1, -1) == 0)
                    return 0;
            }
        }
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x114, *(void**)(&data_ov002_02110964 + 4), 0x40000000, 0x1000, 0);
        *(int*)(c + 0x80) = 0x3000;
        *(int*)(c + 0x84) = 0x3000;
        *(int*)(c + 0x88) = 0x3000;
        *(int*)(c + 0xa8) = 0x23000;
        v2[0] = *(int*)(&data_ov089_02132ca4);
        v2[1] = *(int*)(&data_ov089_02132ca4 + 4);
        v2[2] = *(int*)(&data_ov089_02132ca4 + 8);
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c + 0x220, c, v2, 0x50000, 0x64000, 0x800003, 0x8000);
        _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x260, c, 0x48000, 0, 0, 0);
        *(int*)(c + 0x9c) = -0x2000;
        *(unsigned short*)(c + 0x440) = 0;
    }

    if (_ZN11ShadowModel12InitCylinderEv(c + 0x1c8) == 0)
        return 0;
    _ZN12WithMeshClsn13SetLimMovFlagEv(c + 0x260);
    *(int*)(c + 0x41c) = *(int*)(c + 0x5c);
    *(int*)(c + 0x420) = *(int*)(c + 0x60);
    *(int*)(c + 0x424) = *(int*)(c + 0x64);
    *(unsigned short*)(c + 0x100) = 0;
    *(unsigned char*)(c + 0x442) = 0;
    *(unsigned char*)(c + 0x443) = 0;
    *(int*)(c + 0xa0) = -0x32000;
    *(int*)(c + 0x448) = 0;
    *(int*)(c + 0x110) = 0;
    *(int*)(c + 0x46c) = 0;
    *(int*)(c + 0x468) = *(int*)(c + 0x46c);
    *(int*)(c + 0x464) = *(int*)(c + 0x468);
    if (DAT_0209cef0 == 0)
        _ZN5Event8ClearBitEj(0x1d);
    return 1;
}
