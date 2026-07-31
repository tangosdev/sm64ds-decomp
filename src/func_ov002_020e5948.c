#pragma opt_strength_reduction off
#pragma opt_common_subs off
#pragma opt_propagation off
#pragma opt_loop_invariants off
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef s32 Fix12;

extern void _ZN9Animation8LoadFileER13SharedFilePtr(void* sfp);
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN15TextureSequence8LoadFileER13SharedFilePtr(void* sfp);
extern void LoadSilverStarAndNumber(void);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* bmd, s32 a, s32 b);
extern void func_ov002_020bec2c(void* p);
extern void* _Znwj(u32 sz);
extern void* _ZN10ModelAnim2C1Ev(void* thiz);
extern void _ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt(void* thiz, u32 a, s32 b, Fix12 c, u16 d);
extern s32 func_ov002_020e6bb0(void* p);
extern void func_ov002_020e6780(void* p);
extern void* _ZN9ModelAnimC1Ev(void* thiz);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* f, s32 i, Fix12 fx, u32 j);
extern void* _ZN5ModelC1Ev(void* thiz);
extern void _ZN11ShadowModel12InitCylinderEv(void* thiz);
extern s32 _ZNK6Player14GetBodyModelIDEjb(void* thiz, u32 a, s32 b);
extern void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void* bmd, void* btp);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void* thiz, void* f, s32 i, Fix12 fx, u32 j);

extern s8 data_0209f310[];
extern u8 data_0209f2d8;
extern s8 data_0209f2f8;

extern int data_ov002_0210e798[];
extern int data_ov002_0210e4f0[];
extern int data_ov002_0210e3d8[];
extern int data_ov002_0210e6c0[];
extern int data_ov002_0210eb98[];
extern int data_ov002_0210eb88[];
extern int data_ov002_0210e4c0[];
extern int data_ov002_0210da40[];
extern int data_ov002_0210d9a0[];
extern int data_ov002_0210d9c0[];
extern int data_ov002_0210e500[];
extern int data_ov002_0210e1c8[];
extern int data_ov002_0210e770[];
extern int data_ov002_0210e230[];
extern int data_ov002_0210e288[];
extern int data_ov002_0210e478[];
extern int data_ov002_0210e9d0[];
extern int data_ov002_0210ea10[];
extern int data_ov002_0210e538[];
extern int data_ov002_0210eac8[];
extern int data_ov002_0210e898[];
extern int data_ov002_0210e9e8[];
extern int data_ov002_0210eba8[];
extern int data_ov002_0210eb70[];
extern int data_ov002_0210eaf0[];
extern int data_ov002_0210e640[];
extern int data_ov002_0210e670[];
extern int data_ov002_0210e680[];
extern int data_ov002_0210e438[];
extern int data_ov002_0210e3b0[];
extern int data_ov002_0210e3c8[];
extern int data_ov002_0210e408[];
extern int data_ov002_0210e400[];
extern int data_ov002_0210ecb8[];
extern int data_ov002_0210e8d0[];
extern int data_ov002_0210ebd8[];
extern int data_ov002_0210e750[];
extern int data_ov002_0210d9a8[];
extern int data_ov002_02110aa4[];
extern int data_ov002_0210e588[];
extern int data_ov002_0210e7e8[];
extern int data_ov002_0210e450[];
extern int data_ov002_0210e788[];
extern int data_ov002_0210e1e8[];
extern int data_ov002_0210e1e0[];
extern int data_ov002_0210e4d0[];
extern int data_ov002_0210e4e8[];
extern int data_ov002_0210e1d0[];
extern int data_ov002_0210e800[];
extern int data_ov002_0210e4b8[];
extern int data_ov002_0210e270[];
extern int data_ov002_0210e458[];
extern int data_ov002_0210ea70[];
extern int data_ov002_0210e540[];
extern int data_ov002_0210e8e8[];
extern int data_ov002_0210eca8[];
extern int data_ov002_0210ec98[];
extern int data_ov002_0210e600[];
extern int data_ov002_0210ea88[];
extern int data_ov002_0210e910[];
extern int data_ov002_0210e460[];
extern int data_ov002_0210e2f0[];
extern int data_ov002_0210e958[];
extern int data_ov002_0210eb38[];
extern int data_ov002_0210e660[];
extern int data_ov002_0210ea30[];
extern int data_ov002_0210e3e8[];
extern int data_ov002_0210eb90[];
extern int data_ov002_0210e6b8[];
extern int data_ov002_0210e620[];
extern int data_ov002_0210e6a0[];
extern int data_ov002_0210eb58[];
extern int data_ov002_0210e3e0[];
extern int data_ov002_0210eb10[];
extern int data_ov002_0210e708[];
extern int data_ov002_0210ebc8[];
extern int data_ov002_0210e6e0[];
extern int data_ov002_0210e430[];
extern int data_ov002_0210ec40[];
extern int data_ov002_0210e738[];
extern int data_ov002_0210e250[];
extern int data_ov002_0210ec10[];
extern int data_ov002_0210ec00[];
extern int data_ov002_0210e728[];

extern void* data_ov002_020ff480[];
extern void* data_ov002_0210a68c[];
extern void* data_ov002_0210a69c[];
extern void* data_ov002_0210a6ac[];
extern void* data_ov002_020ff2f0[];

extern int data_ov002_0210da38[];
extern int data_ov002_0210e780[];
extern int data_ov002_0210e6f8[];
extern int data_ov002_0210e7b0[];
extern int data_ov002_0210ebb8[];
extern int data_ov002_0210eb20[];

void func_ov002_020e5948(void* arg0)
{
    char* c = (char*)arg0;
    s32 i;
    s32 j;
    s32 bodyId;
    void* ma;
    void* t;
    s32 t2;
    void* p;
    s32 n;
    s32 k;
    s32 outer;
    void* texseq;
    void* entry;

    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov002_0210e798);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov002_0210e4f0);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov002_0210e3d8);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov002_0210e6c0);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov002_0210eb98);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov002_0210eb88);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov002_0210e4c0);

    if (data_0209f310[*(u8*)(c + 0x6D8)] != 0) {
        LoadSilverStarAndNumber();
        *(u8*)(((long long)(int)(c + 0x718))) |= 1;
    }

    {
        s32 b = *(s32*)(c + 8);
        s32 a = *(u8*)(c + 0x6D9);
        if (a != b) {
        switch (b) {
        case 0:
            _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210da40);
            *(u8*)(((long long)(int)(c + 0x718))) |= 2;
            break;
        case 1:
            _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210d9a0);
            *(u8*)(((long long)(int)(c + 0x718))) |= 4;
            break;
        case 2:
            _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210d9c0);
            *(u8*)(((long long)(int)(c + 0x718))) |= 8;
            break;
        }
    }
    }

    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e500);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e1c8);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e770);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e230);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e288);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e478);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e9d0);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210ea10);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e538);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210eac8);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e898);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e9e8);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210eba8);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210eb70);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210eaf0);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e640);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e670);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e680);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e438);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e3b0);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e3c8);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e408);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e400);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210ecb8);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e8d0);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210ebd8);

    {
        s32 mode;
        mode = data_0209f2d8;
        mode = (mode == 1);
        if (mode == 0) {
            _ZN9Animation8LoadFileER13SharedFilePtr(data_ov002_0210e750);
            _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210d9a8);
            _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_02110aa4);
            _ZN9Animation8LoadFileER13SharedFilePtr(data_ov002_0210e588);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e7e8);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e450);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e788);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e1e8);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e1e0);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e4d0);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e4e8);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e1d0);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e800);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e4b8);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e270);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e458);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210ea70);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e540);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e8e8);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210eca8);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210ec98);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e600);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210ea88);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e910);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e460);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e2f0);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e958);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210eb38);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e660);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210ea30);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e3e8);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210eb90);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e6b8);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e620);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e6a0);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210eb58);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e3e0);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210eb10);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e708);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210ebc8);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e6e0);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e430);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210ec40);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e738);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e250);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210ec10);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210ec00);
            _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov002_0210e728);
        }
    }

    {
        s32 mode;
        mode = data_0209f2d8;
        mode = (mode == 1);
        if (mode == 0) {
            *(s32*)(c + 0x63C) = 0;
        } else {
            *(s32*)(c + 0x63C) = 0x44;
            func_ov002_020bec2c(c);
        }
    }

    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov002_020ff480[*(s32*)(c + 0x63C)]);

    i = 0;
    do {
        p = _Znwj(0x78);
        if (p != 0) {
            p = _ZN10ModelAnim2C1Ev(p);
        }
        *(void**)(c + 0xDC + i * 4) = p;
        _ZN9ModelBase7SetFileEP8BMD_Fileii(*(void**)(c + 0xDC + i * 4), _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210a68c[i]), 1, 1);
        _ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt(*(void**)(c + 0xDC + i * 4), ((s32*)data_ov002_020ff480[i + 0xC4])[1], 0, 0x1000, 0);
        *(s32*)(c + 0x27C + i * 4) = func_ov002_020e6bb0(*(void**)(c + 0xDC + i * 4));
        i++;
    } while (i < 4);

    func_ov002_020e6780(c);

    i = 0;
    do {
        if (i == 3) {
            p = _Znwj(0x64);
            if (p != 0) {
                p = _ZN9ModelAnimC1Ev(p);
            }
            *(void**)(c + 0x154 + i * 4) = p;
            _ZN9ModelBase7SetFileEP8BMD_Fileii(*(void**)(c + 0x154 + i * 4), _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210a69c[i]), 1, 1);
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(*(void**)(c + 0x154 + i * 4), (void*)data_ov002_0210eb88[1], 0, 0x1000, 0);
        } else {
            p = _Znwj(0x50);
            if (p != 0) {
                p = _ZN5ModelC1Ev(p);
            }
            *(void**)(c + 0x154 + i * 4) = p;
            _ZN9ModelBase7SetFileEP8BMD_Fileii(*(void**)(c + 0x154 + i * 4), _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210a69c[i]), 1, 1);
        }
        *(s32*)(c + 0x28C + i * 4) = func_ov002_020e6bb0(*(void**)(c + 0x154 + i * 4));
        i++;
    } while (i < 4);

    j = 0;
    do {
        p = _Znwj(0x50);
        if (p != 0) {
            p = _ZN5ModelC1Ev(p);
        }
        {
            s32 idx = j + 4;
            *(void**)(c + 0x154 + idx * 4) = p;
            _ZN9ModelBase7SetFileEP8BMD_Fileii(*(void**)(c + 0x154 + idx * 4), _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210a6ac[j]), 1, 1);
            *(s32*)(c + 0x28C + idx * 4) = func_ov002_020e6bb0(*(void**)(c + 0x154 + idx * 4));
        }
        j++;
    } while (j < 4);

    {
        s32 mode;
        mode = data_0209f2d8;
        mode = (mode == 1);
        if (mode == 0) {
            p = _Znwj(0x50);
            if (p != 0) {
                p = _ZN5ModelC1Ev(p);
            }
            *(void**)(c + 0x1D8) = p;
            _ZN9ModelBase7SetFileEP8BMD_Fileii(*(void**)(c + 0x1D8), _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210da38), 1, -1);

            if (data_0209f2f8 != 2 && data_0209f2f8 != 5 && data_0209f2f8 != 4) {
                *(u8*)(((long long)(int)(c + 0x718))) |= 0x20;
                p = _Znwj(0x78);
                if (p != 0) {
                    p = _ZN10ModelAnim2C1Ev(p);
                }
                *(void**)(c + 0xEC) = p;
                _ZN9ModelBase7SetFileEP8BMD_Fileii(*(void**)(c + 0xEC), _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210e780), 1, -1);
                _ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt(*(void**)(c + 0xEC), ((s32*)data_ov002_020ff480[0xC6])[1], 0, 0x1000, 0);
                _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xF0, _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210e6f8), 1, 1);
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xF0, (void*)data_ov002_0210e750[1], 0, 0x1000, 0);
                _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x174, _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210e7b0), 1, 1);
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x174, (void*)data_ov002_0210e4c0[1], 0, 0x1000, 0);
            }
        } else {
            _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x174, _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210e7b0), 1, 1);
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x174, (void*)data_ov002_0210e4c0[1], 0, 0x1000, 0);
        }
    }

    _ZN11ShadowModel12InitCylinderEv(c + 0x2AC);

    bodyId = _ZNK6Player14GetBodyModelIDEjb(c, 3, 0);
    ma = *(void**)(c + 0xDC + bodyId * 4);
    {
        u8 pid = *(u8*)(c + 0x6D8);
        t = *(void**)((char*)ma + 0xC);
        t2 = *(s32*)((char*)t + 0x20);
        *(s32*)(c + 0x61C) = t2 + (pid << 1);
    }

    n = 0;
    {
        s32 mode;
        mode = data_0209f2d8;
        mode = (mode == 1);
        if (mode != 0)
            n = 0x11;
    }

    if (n < 0x19) {
        outer = n * 4;
        do {
            k = 0;
            texseq = c + 0x1DC;
            do {
                entry = data_ov002_020ff2f0[outer + k];
                _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File((void*)((s32*)data_ov002_0210a69c[k])[1], (void*)((s32*)entry)[1]);
                if (n == 0x18) {
                    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(texseq, (void*)((s32*)entry)[1], 0, 0x1000, 0);
                }
                k++;
                texseq = (char*)texseq + 0x14;
            } while (k < 4);
            n++;
            outer += 4;
        } while (n < 0x19);
    }

    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File((void*)data_ov002_0210ebb8[1], (void*)data_ov002_0210e8d0[1]);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x254, (void*)data_ov002_0210e8d0[1], 0, 0x1000, 0);
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File((void*)data_ov002_0210eb20[1], (void*)data_ov002_0210ebd8[1]);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x268, (void*)data_ov002_0210ebd8[1], 0, 0x1000, 0);
}
