typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

extern int _ZN9Animation8FinishedEv(void* a);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* f, int a, int b, unsigned int c);
extern void func_ov079_02123bcc(void* c);
extern void _Z14ApproachLinearRsss(s16* p, s16 a, s16 b);
extern void func_ov079_02125504(void* c);
extern void _ZN9Animation7AdvanceEv(void* a);
extern void _ZN5Sound22StopLoadedMusic_Layer3Ev(void);
extern void func_02011cfc(void);

extern void* data_ov079_021275ec[];
extern int data_ov079_021275dc[];

void func_ov079_021256d4(char* c)
{
    void* r;

    if (*(int*)(c + 0x32c) != *(int*)((char*)data_ov079_021275ec[*(u8*)(c + 0x414) * 5 + 3] + 4)) {
        if (_ZN9Animation8FinishedEv(c + 0x31c) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                c + 0x2cc,
                *(void**)((char*)data_ov079_021275ec[*(u8*)(c + 0x414) * 5 + 3] + 4),
                0, 0x1000, 0);
        }
    }

    *(int*)(c + 0x98) = data_ov079_021275dc[*(u8*)(c + 0x414)];
    *(int*)(c + 0x328) = 0x1000;
    func_ov079_02123bcc(c);

    {
        char *base = c + 0x300;
        _Z14ApproachLinearRsss((s16*)(c + 0x94), *(s16*)(base + 0xb8), 0x200);
    }
    *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);

    if (*(u16*)(c + 0x100) > 0x1e) {
        if (*(int*)(c + 0x3f8) < 0x5dc000) {
            *(int*)(c + 0x98) = 0xd800;
            *(int*)(c + 0x328) = 0x3000;
        }
        r = *(void**)(c + (*(int*)(c + 0x3f0) << 2) + 0x39c);
        if (*(u8*)((char*)r + 0x703) == 0) {
            if (*(int*)(c + 0x3f8) < 0x190000) {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                    c + 0x2cc,
                    *(void**)((char*)data_ov079_021275ec[*(u8*)(c + 0x414) * 5] + 4),
                    0x40000000, 0x1000, 0);
                *(int*)(c + 0x3b0) = 3;
            }
        }
    }

    func_ov079_02125504(c);
    _ZN9Animation7AdvanceEv(c + 0x31c);

    r = *(void**)(c + (*(int*)(c + 0x3ec) << 2) + 0x39c);
    if (r != 0) {
        if (*(int*)(c + 0x60) - *(int*)((char*)r + 0x60) <= 0x37f000)
            return;
    }

    {
        s16 t;
        char *base;
        void *anim;
        int five;
        void **tab;
        void *ma;

        _ZN5Sound22StopLoadedMusic_Layer3Ev();
        func_02011cfc();
        *(int*)(c + 0x3b0) = 0;
        *(int*)(c + 0x5c) = *(int*)(c + 0x3bc);
        base = c + 0x300;
        *(int*)(c + 0x60) = *(int*)(c + 0x3c0);
        five = 5;
        *(int*)(c + 0x64) = *(int*)(c + 0x3c4);
        tab = data_ov079_021275ec;
        t = *(s16*)(base + 0xe6);
        ma = c + 0x2cc;
        *(s16*)(c + 0x8c) = t;
        t = *(s16*)(base + 0xe8);
        *(s16*)(c + 0x8e) = t;
        t = *(s16*)(base + 0xea);
        *(s16*)(c + 0x90) = t;
        t = *(s16*)(base + 0xe6);
        *(s16*)(c + 0x92) = t;
        t = *(s16*)(base + 0xe8);
        *(s16*)(c + 0x94) = t;
        t = *(s16*)(base + 0xea);
        *(s16*)(c + 0x96) = t;
        *(int*)(c + 0x338) = 0;
        *(int*)(c + 0x98) = 0;
        anim = *(void**)((char*)tab[*(u8*)(c + 0x414) * five + 3] + 4);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ma, anim, 0, 0x1000, 0);
    }
}
