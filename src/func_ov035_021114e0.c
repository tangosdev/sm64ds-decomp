typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *m, void *f, int a, int b);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *c);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *c);
extern void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *sfp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *mc, void *kcl, void *mtx, int fix, short s, void *clps);
extern void func_020396c0(void *p, int v);
extern void func_020393d4(void *p, void *v);
extern int _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;
extern int data_ov035_02112c78[];
extern int data_ov035_02112c70[];
extern int data_ov035_02112c60[];
extern int data_ov035_02112c68[];
extern int data_ov035_021121d8[];
extern u8 data_0209f2c0[];
extern s16 data_ov035_02111ef4[][4];
extern s16 data_ov035_02111ef0[];

int func_ov035_021114e0(char *c)
{
    int b;

    b = (int)(*(u16*)(c + 0xc) == 0x77);
    if (b != 0) {
        _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4,
            _ZN5Model8LoadFileER13SharedFilePtr(data_ov035_02112c78), 1, -1);
        _ZN8Platform21UpdateModelPosAndRotYEv(c);
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            c + 0x124,
            _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov035_02112c68),
            c + 0x2ec, 0x1000, *(s16*)(c + 0x8e), data_ov035_021121d8);
        func_020396c0(c + 0x124, 0);
        func_020393d4(c + 0x124, &_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
        *(int*)(c + 0x32c) = 0;
    } else {
        b = (int)(*(u16*)(c + 0xc) == 0x79);
        if (b != 0) {
            _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4,
                _ZN5Model8LoadFileER13SharedFilePtr(data_ov035_02112c70), 1, -1);
        } else {
            _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4,
                _ZN5Model8LoadFileER13SharedFilePtr(data_ov035_02112c60), 1, -1);
        }
        _ZN8Platform21UpdateModelPosAndRotYEv(c);
        *(int*)(c + 0x32c) = 1;
    }

    {
        int st = *(int*)(c + 0x32c);
        char *r = c + 0x300;
        *(s16*)(r + 0x1e) = data_ov035_02111ef4[st][data_0209f2c0[0]];
        *(s16*)(r + 0x24) = data_ov035_02111ef0[*(int*)(c + 0x32c)];
        *(s16*)(r + 0x26) = data_ov035_02111ef0[*(int*)(c + 0x32c)];
    }
    return 1;
}
