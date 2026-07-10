//cpp
extern "C" {
extern int _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* f, int a, int b);
extern int _ZN9Animation8LoadFileER13SharedFilePtr(void* sfp);
extern int _ZN15TextureSequence8LoadFileER13SharedFilePtr(void* sfp);
extern void func_02016b24(void* thiz, int a);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* f, int a, int fix, unsigned int u);
extern void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void* a, void* b);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void* thiz, void* f, int a, int fix, unsigned int u);
extern void _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File(void* a, void* b);
extern void _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(void* thiz, void* f, int a, int fix, unsigned int u);
extern void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(void* a, void* b);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void* thiz, void* f, int a, int fix, unsigned int u);

extern int data_ov060_0211b208[];
extern int data_ov060_0211b1f8[];
extern int data_ov060_0211b200[];
extern int func_021115e4[];
extern int func_021115f4[];

int _ZN16BowserShockwaves13InitResourcesEv(char* c)
{
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, (void*)_ZN5Model8LoadFileER13SharedFilePtr(&data_ov060_0211b208), 1, 0x13);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x174, (void*)data_ov060_0211b208[1], 1, 0x13);

    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov060_0211b1f8);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(&data_ov060_0211b200);

    func_02016b24(c + 0xd4, 0x4000);
    func_02016b24(c + 0x174, 0x4000);

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void*)data_ov060_0211b1f8[1], 0x40000000, 0x1000, 0);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x174, (void*)data_ov060_0211b1f8[1], 0x40000000, 0x1000, 0);

    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File((void*)data_ov060_0211b208[1], (void*)data_ov060_0211b200[1]);
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File((void*)data_ov060_0211b208[1], (void*)data_ov060_0211b200[1]);

    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x138, (void*)data_ov060_0211b200[1], 0x40000000, 0x1000, 0);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x1d8, (void*)data_ov060_0211b200[1], 0x40000000, 0x1000, 0);

    _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File((void*)data_ov060_0211b208[1], func_021115e4);
    _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File((void*)data_ov060_0211b208[1], func_021115e4);

    _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(c + 0x14c, func_021115e4, 0x40000000, 0x1000, 0);
    _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(c + 0x1ec, func_021115e4, 0x40000000, 0x1000, 0);

    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File((void*)data_ov060_0211b208[1], func_021115f4);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File((void*)data_ov060_0211b208[1], func_021115f4);

    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(c + 0x160, func_021115f4, 0x40000000, 0x1000, 0);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(c + 0x200, func_021115f4, 0x40000000, 0x1000, 0);

    *(short*)(c + 0x214) = 0;
    return 1;
}
}
