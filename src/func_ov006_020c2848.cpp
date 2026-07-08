//cpp
extern "C" {
extern int _ZN9Animation8FinishedEv(void* a);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void*, void*, int, int, int, unsigned short);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern void* data_ov006_0213ae48;
extern void* SCENE_FADER;
struct P2 { void* a; void* b; };
extern P2 data_ov006_0213adc8;
}

struct VObj {
    virtual int v0();
    virtual int v1();
    virtual int v2();
    virtual int v3();
    virtual int v4();
    virtual int v5();
    virtual int v6();
};

extern "C" void func_ov006_020c2848(char* c)
{
    if (*(int*)(c + 0x68) != *(int*)(c + 0x104)) {
        int r5 = 0;
        if (((VObj*)SCENE_FADER)->v6() == 0) {
            if (_ZN9Animation8FinishedEv(c + 0x58) == 0 || *(int*)(c + 0x68) == *(int*)(c + 0x13c)) {
                r5 = 8;
            }
        }
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 8, *(void**)(c + 0x104), r5, 0, 0x800, 0);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0xc8, *(void**)(c + 0x144), 0, 0x800, 0);
        _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(c + 0xdc, (void*)&data_ov006_0213ae48, 0, 0x800, 0);
    }
    {
        void* lo = ((volatile P2*)&data_ov006_0213adc8)->a;
        void* hi = ((volatile P2*)&data_ov006_0213adc8)->b;
        *(void**)(c) = lo;
        *(void**)(c + 4) = hi;
    }
}
