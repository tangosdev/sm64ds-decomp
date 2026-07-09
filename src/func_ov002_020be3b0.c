typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct Vector3 { int x, y, z; };

extern u32 _ZNK6Player14GetBodyModelIDEjb(char* c, u32 a, char b);
extern void _ZN5Model14SetPolygonModeEi(void* self, int mode);
extern int func_ov002_020becf4(char* self, int v, int arg2);
extern int __aeabi_idiv(int a, int b);
extern int __aeabi_idivmod(int a, int b);
extern void func_ov002_020e6b74(char* c, int* src);
extern void func_ov002_020e6b3c(void* model);
extern void func_ov002_020d80d0(char* c);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void* f);
extern void _ZN10ModelAnim24CopyERKS_Pcj(void* self, void* src, char* p, u32 n);
extern void _ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt(void* self, u32 a, int b, int c, unsigned short d);
extern void _ZN13SharedFilePtr7ReleaseEv(void* self);
extern int _ZN6Player7IsStateERNS_5StateE(char* self, void* s);
extern unsigned int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, const struct Vector3* v);
extern void _ZN6Player18TurnOffToonShadingEj(char* c, u32 j);
extern void func_ov002_020e69c0(void* unused);
extern void func_ov002_020e6a78(char* c, int b);
extern void func_ov002_020bdb50(char* c, int arg);

extern u8 data_ov002_020ff14c[];
extern void* data_ov002_020ff480[];
extern void* data_ov002_020ff790[];
extern u8 data_02092128[];
extern int data_ov002_021104b4;
extern char data_ov002_02110804[];

void func_ov002_020be3b0(char* c)
{
    void* case3_m1;
    void* case3_m2;
    int var_r5;
    int var_r4;
    u16 flags;
    int idx;

    flags = *(u16*)(c + 0x73c);
    var_r5 = 3;
    var_r4 = 0;
    if (flags & 0x8000) {
        var_r5 = 2;
        var_r4 = 5;
    }
    idx = flags & 7;

    switch (idx) {
    case 0:
        return;
    case 1: {
        void* m1 = *(void**)(c + (_ZNK6Player14GetBodyModelIDEjb(c, (u8)(*(u32*)(c + 8)), 0) * 4) + 0xdc);
        _ZN5Model14SetPolygonModeEi(m1, 2);
        {
            void* m2 = *(void**)(c + (func_ov002_020becf4(c, (u8)(*(u32*)(c + 8)), 0) * 4) + 0x154);
            _ZN5Model14SetPolygonModeEi(m2, 2);
        }
        *(u16*)(c + 0x740) = 0;
        *(u16*)(c + 0x73e) = data_ov002_020ff14c[var_r4];
        *(u16*)(((long long)(int)(c + 0x73c)) & 0xFFFFFFFFFFFFFFFFLL) = (u16)(*(u16*)(((long long)(int)(c + 0x73c)) & 0xFFFFFFFFFFFFFFFFLL) + 1);
        *(u32*)(c + 8) = *(u8*)(c + 0x6dc);
        break;
    }
    case 2:
        *(u16*)(((long long)(int)(c + 0x740)) & 0xFFFFFFFFFFFFFFFFLL) = (u16)(*(u16*)(((long long)(int)(c + 0x740)) & 0xFFFFFFFFFFFFFFFFLL) + 0x199);
        if ((u32)*(u16*)(c + 0x740) >= 0x1000) {
            *(u16*)(c + 0x740) = 0x1000;
        }
        if (*(u16*)(c + 0x73e) == 0) {
            *(u16*)(((long long)(int)(c + 0x73c)) & 0xFFFFFFFFFFFFFFFFLL) = (u16)(*(u16*)(((long long)(int)(c + 0x73c)) & 0xFFFFFFFFFFFFFFFFLL) + 1);
            *(u16*)(c + 0x73e) = data_ov002_020ff14c[var_r4 + 1];
        }
        *(u32*)(c + 8) = *(u8*)(c + 0x6dc);
        break;
    case 3: {
        if ((*(u16*)(c + 0x73e) % var_r5) == 0) {
            case3_m1 = *(void**)(c + (_ZNK6Player14GetBodyModelIDEjb(c, (u8)(*(u32*)(c + 8)), 0) * 4) + 0xdc);
            case3_m2 = *(void**)(c + (func_ov002_020becf4(c, (u8)(*(u32*)(c + 8)), 0) * 4) + 0x154);
            if ((*(u16*)(c + 0x73e) % (var_r5 * 2)) == 0) {
                func_ov002_020e6b74(case3_m1, *(int**)(c + (*(u32*)(c + 8) * 4) + 0x27c));
                func_ov002_020e6b74(case3_m2, *(int**)(c + (func_ov002_020becf4(c, (u8)(*(u32*)(c + 8)), 0) * 4) + 0x28c));
            } else {
                func_ov002_020e6b3c(case3_m1);
                func_ov002_020e6b3c(case3_m2);
            }
        }
        if (*(u16*)(c + 0x73e) == 0) {
            void* t1;
            void* t2;
            func_ov002_020e6b3c(case3_m1);
            func_ov002_020e6b3c(case3_m2);
            *(u16*)(((long long)(int)(c + 0x73c)) & 0xFFFFFFFFFFFFFFFFLL) = (u16)(*(u16*)(((long long)(int)(c + 0x73c)) & 0xFFFFFFFFFFFFFFFFLL) + 1);
            *(u16*)(c + 0x73e) = data_ov002_020ff14c[var_r4 + 2];
            t1 = *(void**)(c + (_ZNK6Player14GetBodyModelIDEjb(c, *(u8*)(c + 0x6dd), 0) * 4) + 0xdc);
            _ZN5Model14SetPolygonModeEi(t1, 2);
            func_ov002_020e6b3c(t1);
            t2 = *(void**)(c + (func_ov002_020becf4(c, *(u8*)(c + 0x6dd), 0) * 4) + 0x154);
            _ZN5Model14SetPolygonModeEi(t2, 2);
            func_ov002_020e6b3c(t2);
            *(u8*)(c + 0x6eb) = 0;
            *(u8*)(c + 0x6ea) = *(u8*)(c + 0x6eb);
            *(u16*)(c + 0x6ba) = *(u8*)(c + 0x6ea);
            *(u8*)(c + 0x6ec) = 0;
            func_ov002_020d80d0(c);
            *(u8*)(c + 0x6f4) = 0;
            *(u8*)(c + 0x714) = 0;
            _ZN9Animation8LoadFileER13SharedFilePtr(data_ov002_020ff480[*(u32*)(c + 0x63c) + *(u8*)(c + 0x6dd)]);
            {
                u32 mm1 = _ZNK6Player14GetBodyModelIDEjb(c, *(u8*)(c + 0x6dd), 0);
                _ZN10ModelAnim24CopyERKS_Pcj(
                    *(void**)(c + mm1 * 4 + 0xdc),
                    *(void**)(c + _ZNK6Player14GetBodyModelIDEjb(c, *(u8*)(c + 0x6dc), 0) * 4 + 0xdc),
                    *(char**)((char*)data_ov002_020ff480[*(u32*)(c + 0x63c) + *(u8*)(c + 0x6dd)] + 4),
                    0);
            }
            *(u8*)(((long long)(int)(c + 0x718)) & 0xFFFFFFFFFFFFFFFFLL) = (u8)(*(u8*)(((long long)(int)(c + 0x718)) & 0xFFFFFFFFFFFFFFFFLL) | 0x40);
        }
        *(u32*)(c + 8) = *(u8*)(c + 0x6dc);
        break;
    }
    case 4:
        if ((*(u16*)(c + 0x73e) % var_r5) == 0) {
            if ((*(u16*)(c + 0x73e) % (var_r5 * 2)) == 0) {
                *(u32*)(c + 8) = *(u8*)(c + 0x6dc);
            } else {
                *(u32*)(c + 8) = *(u8*)(c + 0x6dd);
            }
        }
        if (*(u16*)(c + 0x73e) == 0) {
            u32 mm;
            *(u16*)(((long long)(int)(c + 0x73c)) & 0xFFFFFFFFFFFFFFFFLL) = (u16)(*(u16*)(((long long)(int)(c + 0x73c)) & 0xFFFFFFFFFFFFFFFFLL) + 1);
            *(u16*)(c + 0x73e) = data_ov002_020ff14c[var_r4 + 3];
            *(u32*)(c + 8) = *(u8*)(c + 0x6dd);
            data_02092128[*(u8*)(c + 0x6d8)] = (u8)(*(u32*)(c + 8));
            if (*(u8*)(c + 0x6dd) == *(u8*)(c + 0x6d9)) {
                *(u8*)(c + 0x71a) = 0;
            }
            func_ov002_020e6b74(
                *(void**)(c + _ZNK6Player14GetBodyModelIDEjb(c, *(u8*)(c + 0x6dd), 0) * 4 + 0xdc),
                *(int**)(c + *(volatile u8*)(c + 0x6dd) * 4 + 0x27c));
            func_ov002_020e6b74(
                *(void**)(c + _ZNK6Player14GetBodyModelIDEjb(c, *(u8*)(c + 0x6dc), 0) * 4 + 0xdc),
                *(int**)(c + *(volatile u8*)(c + 0x6dc) * 4 + 0x27c));
            {
                char* p1 = c + func_ov002_020becf4(c, *(u8*)(c + 0x6dd), 0) * 4;
                func_ov002_020e6b74(*(void**)(p1 + 0x154), *(int**)(p1 + 0x28c));
            }
            {
                char* p2 = c + func_ov002_020becf4(c, *(u8*)(c + 0x6dc), 0) * 4;
                func_ov002_020e6b74(*(void**)(p2 + 0x154), *(int**)(p2 + 0x28c));
            }
            mm = *(u32*)(c + 8);
            _ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt(
                *(void**)(c + _ZNK6Player14GetBodyModelIDEjb(c, (u8)mm, 0) * 4 + 0xdc),
                *(u32*)((char*)data_ov002_020ff790[mm] + 4),
                0, 0x1000, 0);
            _ZN13SharedFilePtr7ReleaseEv(data_ov002_020ff480[*(u32*)(c + 0x63c) + *(u8*)(c + 0x6dc)]);
            *(u8*)(((long long)(int)(c + 0x718)) & 0xFFFFFFFFFFFFFFFFLL) = (u8)(*(u8*)(((long long)(int)(c + 0x718)) & 0xFFFFFFFFFFFFFFFFLL) & ~0x40);
            func_ov002_020bdb50(c, 0);
        } else {
            u8 a, b;
            u32 idxA;
            b = *(u8*)(c + 0x6dc);
            if (b == *(u32*)(c + 8)) {
                a = *(u8*)(c + 0x6dd);
            } else {
                a = b;
                b = *(u8*)(c + 0x6dd);
            }
            idxA = _ZNK6Player14GetBodyModelIDEjb(c, a, 0);
            _ZN10ModelAnim24CopyERKS_Pcj(
                *(void**)(c + idxA * 4 + 0xdc),
                *(void**)(c + _ZNK6Player14GetBodyModelIDEjb(c, b, 0) * 4 + 0xdc),
                *(char**)((char*)data_ov002_020ff480[*(u32*)(c + 0x63c) + a] + 4),
                0);
        }
        break;
    case 5:
        if (*(u16*)(c + 0x73e) == 0) {
            *(u16*)(((long long)(int)(c + 0x73c)) & 0xFFFFFFFFFFFFFFFFLL) = (u16)(*(u16*)(((long long)(int)(c + 0x73c)) & 0xFFFFFFFFFFFFFFFFLL) + 1);
            *(u16*)(c + 0x73e) = data_ov002_020ff14c[var_r4 + 4];
            if (*(u8*)(c + 0x6dd) != *(u8*)(c + 0x6d9) &&
                _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021104b4) == 0) {
                _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8*)(c + 0x6d9), 0x21, (struct Vector3*)(c + 0x74));
            }
        }
        break;
    case 6: {
        int divv = data_ov002_020ff14c[var_r4 + 4];
        *(u16*)(((long long)(int)(c + 0x740)) & 0xFFFFFFFFFFFFFFFFLL) = (u16)(*(u16*)(((long long)(int)(c + 0x740)) & 0xFFFFFFFFFFFFFFFFLL) - __aeabi_idiv(0x1000, divv));
        if (*(s16*)(c + 0x740) < 0) {
            *(u16*)(c + 0x740) = 0;
        }
        if (*(u16*)(c + 0x73e) == 0) {
            *(u16*)(((long long)(int)(c + 0x73c)) & 0xFFFFFFFFFFFFFFFFLL) = (u16)(*(u16*)(((long long)(int)(c + 0x73c)) & 0xFFFFFFFFFFFFFFFFLL) + 1);
        }
        break;
    }
    case 7:
        _ZN6Player18TurnOffToonShadingEj(c, *(u8*)(c + 0x6dd));
        _ZN6Player18TurnOffToonShadingEj(c, *(u8*)(c + 0x6dc));
        *(u16*)(c + 0x73c) = 0;
        break;
    }

    if (*(u16*)(c + 0x73e) != 0) {
        *(u16*)(((long long)(int)(c + 0x73e)) & 0xFFFFFFFFFFFFFFFFLL) = (u16)(*(u16*)(((long long)(int)(c + 0x73e)) & 0xFFFFFFFFFFFFFFFFLL) - 1);
    }
    {
        char* dummy = data_ov002_02110804;
        func_ov002_020e69c0(dummy);
        func_ov002_020e6a78(dummy, *(u16*)(c + 0x740));
    }
}
