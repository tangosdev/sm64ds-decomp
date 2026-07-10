extern int _ZN5Model8LoadFileER13SharedFilePtr(int);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
extern void func_ov095_0213597c(char*);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern int _ZN12MeshCollider8LoadFileER13SharedFilePtr(int);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*,int,void*,int,short,int);
extern void func_020393d4(void*,void*);
extern void func_020393c4(void*,void*);

extern char data_ov095_021374a0[];
extern char data_ov095_021374a4[];
extern char data_ov095_021374a8[];
extern int _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_[];
extern int func_ov095_02135e90[];

int _ZN9SeesawBob13InitResourcesEv(char *c)
{
    unsigned char idx;
    int f;
    switch (*(unsigned short*)(c+0xc)) {
        case 0x1c: c[0x31e] = 0; break;
        case 0x27: c[0x31e] = 1; break;
        case 0x85: c[0x31e] = 2; break;
        case 0x8f: c[0x31e] = 3; break;
        case 0x95: c[0x31e] = 4; break;
        case 0x96: c[0x31e] = 5; break;
        case 0x80: c[0x31e] = 6; break;
    }
    idx = (unsigned char)c[0x31e];
    f = _ZN5Model8LoadFileER13SharedFilePtr(*(int*)(data_ov095_021374a0 + idx*0xc));
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, f, 1, -1);
    func_ov095_0213597c(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    {
        unsigned char i = (unsigned char)c[0x31e];
        f = _ZN12MeshCollider8LoadFileER13SharedFilePtr(*(int*)(data_ov095_021374a4 + i*0xc));
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            c+0x124, f, c+0x2ec, 0x1000, *(short*)(c+0x8e), *(int*)(data_ov095_021374a8 + i*0xc));
    }
    func_020393d4(c+0x124, _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    func_020393c4(c+0x124, func_ov095_02135e90);
    return 1;
}
