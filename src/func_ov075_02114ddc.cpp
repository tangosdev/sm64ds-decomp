//cpp
extern "C" {
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *o, void *f, int a, int b);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *o, void *f, int a, int fx, unsigned j);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *o, void *f, int a, int b, int fx, unsigned short t);
extern void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void *a, void *b);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *o, void *f, int a, int fx, unsigned j);
extern int RandomIntInternal(void *seed);
extern int _ZNK9Animation13GetFrameCountEv(void *o);
extern int _ZN11ShadowModel12InitCylinderEv(void *o);
extern char data_ov075_0211d404;
extern char data_ov075_0211d3c4;
extern char data_ov075_0211d414;
extern char *data_ov075_0211c678[];
extern char data_ov075_0211d40c;
extern char data_ov075_0211d3ac;
extern char RNG_STATE;
extern int data_ov075_0211b544[];
}

extern "C" int func_ov075_02114ddc(void *cc, int r6, int r5, int r4)
{
    char *c = (char*)cc;
    void *tex;

    _ZN9ModelBase7SetFileEP8BMD_Fileii(c, *(void**)(&data_ov075_0211d404 + 4), 1, 1);
    *(int*)(c + 0x148) = *(int*)(*(char**)(c + 0xc) + 0x20) + (r5 << 1);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x70, *(void**)(&data_ov075_0211d3c4 + 4), 1, 1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x70, *(void**)(&data_ov075_0211d414 + 4), 0, 0x1000, 0);

    if (r6 == 0 || r6 == 2) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c, *(void**)(data_ov075_0211c678[r5] + 4), 0, 0, 0x1000, 0);
        tex = *(void**)(&data_ov075_0211d40c + 4);
        *(int*)(c + 0x110) = 0;
    } else {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c, *(void**)(&data_ov075_0211d3ac + 4), 0, 0, 0x1000, 0);
        tex = *(void**)(&data_ov075_0211d40c + 4);
        *(int*)(c + 0x110) = 7;
    }

    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(*(void**)(&data_ov075_0211d3c4 + 4), tex);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0xd4, tex, 0, 0x1000, 0);

    {
        unsigned x = (unsigned)RandomIntInternal(&RNG_STATE);
        int fc = _ZNK9Animation13GetFrameCountEv(c + 0x50);
        *(int*)(c + 0xdc) = (((x >> 0x10) % (unsigned)fc) << 0x10) >> 4;
    }

    if (_ZN11ShadowModel12InitCylinderEv(c + 0xe8) == 0)
        return 0;

    *(int*)(c + 0x118) = r4;
    *(int*)(c + 0x11c) = 0;
    *(int*)(c + 0x120) = 0;
    *(int*)(c + 0x124) = *(int*)(c + 0x118);
    *(int*)(c + 0x128) = *(int*)(c + 0x11c);
    *(int*)(c + 0x12c) = *(int*)(c + 0x120);
    *(int*)(c + 0x140) = data_ov075_0211b544[r5];
    *(int*)(c + 0x114) = 0;
    *(unsigned char*)(c + 0x152) = (unsigned char)r5;
    return 1;
}
