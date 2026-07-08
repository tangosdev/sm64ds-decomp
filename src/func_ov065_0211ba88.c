typedef int Fix12;
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void*);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void*);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void*, void*, void*, Fix12, short, void*);

extern char data_ov065_0211d98c[];
extern char data_ov065_0211d97c[];
extern char func_021121b8[];
extern unsigned char LEVEL_SPECIFIC_SETTING[];
extern char data_ov065_0211c0d4[];
extern char data_ov065_0211c0d0[];

int func_ov065_0211ba88(char* c)
{
    void* mdl;
    void* kcl;
    unsigned int ip;
    unsigned int b;
    mdl = _ZN5Model8LoadFileER13SharedFilePtr(data_ov065_0211d98c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, mdl, 1, -1);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov065_0211d97c);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c + 0x124, kcl, c + 0x2ec, 0x199, *(short*)(c + 0x8e), func_021121b8);
    *(int*)(c + 0x320) = *(int*)(c + 0x5c);
    *(int*)(c + 0x324) = *(int*)(c + 0x60);
    *(int*)(c + 0x328) = *(int*)(c + 0x64);
    *(unsigned char*)(c + 0x32e) = 0;
    ip = LEVEL_SPECIFIC_SETTING[0];
    b = *(unsigned char*)(c + 0x32e);
    *(short*)(c + 0x32c) = *(unsigned short*)(data_ov065_0211c0d4 + (ip << 4) + (b << 3));
    b = *(unsigned char*)(c + 0x32e);
    *(int*)(c + 0xa8) = *(int*)(data_ov065_0211c0d0 + (ip << 4) + (b << 3));
    *(int*)(c + 0xa0) = -0x3c000;
    return 1;
}
