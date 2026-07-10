extern int _ZN5Model8LoadFileER13SharedFilePtr(int);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
extern void Matrix4x3_FromRotationXYZExt(void*,int,int,int);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern int _ZN12MeshCollider8LoadFileER13SharedFilePtr(int);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,int,short,int);
extern void func_020393d4(void*,void*);
extern void func_020393c4(void*,void*);
extern int _ZN16MeshColliderBase6EnableEP5Actor(void*,void*);
extern void func_ov073_021223a4(void*,void*);

extern unsigned char data_ov073_02123424[];
extern unsigned char data_ov073_02123420[];
extern char data_ov073_021231bc[];
extern char data_ov073_021231c0[];
extern char data_ov073_021231c4[];
extern int _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_[];
extern int func_ov073_021227d0[];
extern void* data_ov073_021234b0;

int _ZN8CccArena13InitResourcesEv(char *c)
{
    unsigned char idx;
    int f;

    switch (*(unsigned short*)(c + 0xc)) {
        case 0xaa:
            *(unsigned short*)(c + 0x330) = 3;
            c[0x32c] = 0;
            break;
        case 0xab:
            *(unsigned short*)(c + 0x330) = 3;
            c[0x32d] = data_ov073_02123424[0];
            data_ov073_02123424[0]++;
            c[0x32c] = 1;
            break;
        case 0xac:
            *(unsigned short*)(c + 0x330) = 3;
            c[0x32d] = data_ov073_02123420[0];
            data_ov073_02123420[0]++;
            c[0x32c] = 2;
            break;
    }

    idx = (unsigned char)c[0x32c];
    f = _ZN5Model8LoadFileER13SharedFilePtr(*(int*)(data_ov073_021231bc + idx * 0xc));
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, f, 1, -1);
    Matrix4x3_FromRotationXYZExt(c + 0xf0, *(short*)(c + 0x8c), *(short*)(c + 0x8e), *(short*)(c + 0x90));
    *(int*)(c + 0x114) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x118) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x11c) = *(int*)(c + 0x64) >> 3;
    _ZN8Platform19UpdateClsnPosAndRotEv(c);

    {
        unsigned char i = (unsigned char)c[0x32c];
        f = _ZN12MeshCollider8LoadFileER13SharedFilePtr(*(int*)(data_ov073_021231c0 + i * 0xc));
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            c + 0x124, f, c + 0x2ec, 0x1000, *(short*)(c + 0x8e), *(int*)(data_ov073_021231c4 + i * 0xc));
    }

    func_020393d4(c + 0x124, _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    func_020393c4(c + 0x124, func_ov073_021227d0);
    _ZN16MeshColliderBase6EnableEP5Actor(c + 0x124, c);

    *(int*)(c + 0x338) = 0;
    *(int*)(c + 0x334) = 0;
    func_ov073_021223a4(c, &data_ov073_021234b0);

    return 1;
}
