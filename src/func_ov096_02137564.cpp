//cpp
extern "C" {
extern int _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* f, int a, int b);
extern int _ZN9Animation8LoadFileER13SharedFilePtr(void* sfp);
extern void func_02016aac(void* thiz, int a, int b);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* f, int a, int fix, unsigned int u);
extern void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(void* a, void* b);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void* thiz, void* f, int a, int fix, unsigned int u);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* thiz, void* actor, int a, int b, unsigned int c, unsigned int d);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* actor, int a, int b, void* v, int e);
extern int data_ov096_02137ba8[];
extern int data_ov096_02137bb0[];
extern int func_02112968[];

int func_ov096_02137564(char* c)
{
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x2c4, (void*)_ZN5Model8LoadFileER13SharedFilePtr(&data_ov096_02137ba8), 1, 0x15);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov096_02137bb0);
    func_02016aac(c + 0x2c4, 0x16, 1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x2c4, (void*)data_ov096_02137bb0[1], 0, 0x1000, 0);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File((void*)data_ov096_02137ba8[1], func_02112968);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(c + 0x328, func_02112968, 0, 0x1000, 0);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0xd4, c, 0, 0, 0x200002, 0);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x108, c, 0x50000, 0x50000, 0, 0);

    *(int*)(c + 0x340) = *(int*)(c + 0x5c);
    *(int*)(c + 0x344) = *(int*)(c + 0x60);
    *(int*)(c + 0x348) = *(int*)(c + 0x64);
    *(short*)(c + 0x35a) = 0;
    *(int*)(c + 0x35c) = 0;
    *(unsigned char*)(c + 0x360) = 0;
    *(int*)(c + 0x9c) = -0x1000;
    *(int*)(c + 0xa0) = -0x1e000;
    {
        unsigned int t = *(int*)(c + 8) & 0xff;
        if (t == 0xff)
            *(int*)(c + 0x34c) = 0x5dc000;
        else
            *(int*)(c + 0x34c) = t * 0x64000;
    }
    *(int*)(c + 0x364) = 0;
    *(int*)(c + 0x368) = 0;
    return 1;
}
}
