struct Mtx { int w[12]; };
extern struct Mtx data_020a0e68;

extern char data_ov006_0213ae48[];
extern char data_ov006_0213ae60[];
extern char data_ov006_0213aea8[];
extern char data_ov006_0213ae00[];
extern char data_ov006_0213ae30[];
extern char data_ov006_0213ae78[];
extern char data_ov006_0213ae90[];
extern char data_ov006_0213ae18[];

extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp);
extern void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *fp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *file, int b, int c);
extern void func_02016b24(void *o, unsigned int mask);
extern void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void *bmd, void *btp);
extern void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(void *bmd, void *bta);
extern void func_ov006_020c2848(char *c);
extern void func_ov006_020c2290(char *c);
extern void Matrix4x3_FromTranslation(struct Mtx *m, int x, int y, int z);

int func_ov006_020c3050(char *c)
{
    void *r5;
    void *r4;

    r5 = _ZN5Model8LoadFileER13SharedFilePtr(c + 0xf0);
    r4 = _ZN5Model8LoadFileER13SharedFilePtr(c + 0xf8);
    _ZN9Animation8LoadFileER13SharedFilePtr(c + 0x100);
    _ZN9Animation8LoadFileER13SharedFilePtr(c + 0x108);
    _ZN9Animation8LoadFileER13SharedFilePtr(c + 0x110);
    _ZN9Animation8LoadFileER13SharedFilePtr(c + 0x118);
    _ZN9Animation8LoadFileER13SharedFilePtr(c + 0x120);
    _ZN9Animation8LoadFileER13SharedFilePtr(c + 0x128);
    _ZN9Animation8LoadFileER13SharedFilePtr(c + 0x130);
    _ZN9Animation8LoadFileER13SharedFilePtr(c + 0x138);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(c + 0x140);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(c + 0x148);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(c + 0x150);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(c + 0x158);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(c + 0x160);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(c + 0x168);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(c + 0x170);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(c + 0x178);

    if (!_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 8, r5, 1, -1))
        return 0;

    func_02016b24(c + 8, 2);

    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, *(void **)(c + 0x144));
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, *(void **)(c + 0x14c));
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, *(void **)(c + 0x154));
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, *(void **)(c + 0x15c));
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, *(void **)(c + 0x164));
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, *(void **)(c + 0x16c));
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, *(void **)(c + 0x174));
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, *(void **)(c + 0x17c));

    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, data_ov006_0213ae48);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, data_ov006_0213ae60);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, data_ov006_0213aea8);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, data_ov006_0213ae00);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, data_ov006_0213ae30);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, data_ov006_0213ae78);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, data_ov006_0213ae90);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, data_ov006_0213ae18);

    func_ov006_020c2848(c);
    *(int *)(c + 0x180) = 0;
    *(int *)(c + 0x184) = 0;
    *(int *)(c + 0x188) = 0;
    func_ov006_020c2290(c);

    if (!_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x78, r4, 1, -1))
        return 0;

    Matrix4x3_FromTranslation(&data_020a0e68, 0, 0, 0);
    *(struct Mtx *)(c + 0x94) = data_020a0e68;
    return 1;
}
