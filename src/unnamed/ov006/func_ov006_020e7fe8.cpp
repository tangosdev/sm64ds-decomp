//cpp
extern "C" {
int func_020179b4(void* r0, void* r1, int r2);
void _ZN15TextureSequence8LoadFileER13SharedFilePtr(void* r);
void _ZN9Animation8LoadFileER13SharedFilePtr(void* r);
void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void* bmd, void* btp);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void* self, void* btp, int a, int b, unsigned int d);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int b, unsigned int d);
void func_ov006_020e7954(void* c);
void func_ov006_020e7b0c(char* c);
void func_ov006_020e7fb0(char* c);
}

extern void* data_ov006_02141e54;
extern void* data_ov006_02141e64;
extern void* data_ov006_02141e5c;
extern void* data_ov006_02141e7c;
extern void* data_ov006_02141e8c;
extern void* data_ov006_02141e84;

extern "C" int func_ov006_020e7fe8(char* c)
{
    if (func_020179b4(&data_ov006_02141e54, c + 0xc, 1) == 0)
        return 0;
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(&data_ov006_02141e64);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02141e5c);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02141e7c);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02141e8c);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02141e84);
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(*((void**)&data_ov006_02141e54 + 1), *((void**)&data_ov006_02141e64 + 1));
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x70, *((void**)&data_ov006_02141e64 + 1), 0, 0x1000, 0);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xc, *((void**)&data_ov006_02141e5c + 1), 0x40000000, 0x800, 0);
    func_ov006_020e7954(c + 0x84);
    func_ov006_020e7b0c(c);
    func_ov006_020e7fb0(c);
    *(int*)(c + 0x208) = 0;
    return 1;
}
