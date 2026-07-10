/* func_ov002_020c8540 at 0x020c8540 (ov002)
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;

extern int _ZNK6Player14GetBodyModelIDEjb(void* thiz, unsigned int a, int b);
extern int _ZNK9Animation12WillHitFrameEi(void* thiz, int f);
extern void func_0201f32c(int arg0);
extern int _ZN6Player12FinishedAnimEv(char* self);
extern void _ZN6Player11ChangeStateERNS_5StateE(char* self, void* st);

extern u16 data_ov002_020ff138[];
extern u16 data_ov002_020ff13c[];
extern u16 data_ov002_020ff110[];
extern s16 data_ov002_020ff108[];
extern int data_0209b454;
extern u8 data_0209d660;
extern int data_ov002_0211013c;

int func_ov002_020c8540(char* self)
{
    u32 s8;
    int id;
    char* anim;

    s8 = *(u32*)(self + 8);
    if (s8 <= 1) {
        id = _ZNK6Player14GetBodyModelIDEjb(self, s8 & 0xff, 0);
        anim = (char*)((int*)(self + 0xdc))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, data_ov002_020ff138[s8])) {
            *(u8*)(self + 0x71a) = 1;
        }
        s8 = *(u32*)(self + 8);
        id = _ZNK6Player14GetBodyModelIDEjb(self, s8 & 0xff, 0);
        anim = (char*)((int*)(self + 0xdc))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, data_ov002_020ff13c[s8])) {
            *(u8*)(self + 0x71a) = 0;
        }
    }
    if (*(u8*)(self + 0x70c) == 0) {
        s8 = *(u32*)(self + 8);
        id = _ZNK6Player14GetBodyModelIDEjb(self, s8 & 0xff, 0);
        anim = (char*)((int*)(self + 0xdc))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, data_ov002_020ff110[s8])) {
            char* o;
            { int* p = (int*)((long long)(int)(self + 0xb0) & 0xFFFFFFFFFFFFFFFFLL); *p |= 0x800000; }
            o = *(char**)(self + 0x368);
            if (o != 0) {
                int* p = (int*)((long long)(int)(o + 0xb0) & 0xFFFFFFFFFFFFFFFFLL);
                *p &= ~0x800000;
            }
            data_0209b454 |= 0x800000;
            func_0201f32c(data_ov002_020ff108[*(u32*)(self + 8)]);
            *(u8*)(self + 0x70c) = 1;
            return 1;
        }
    } else {
        char* o;
        if (data_0209d660 != 0) return 1;
        data_0209b454 &= ~0x800000;
        { int* p = (int*)((long long)(int)(self + 0xb0) & 0xFFFFFFFFFFFFFFFFLL); *p &= ~0x800000; }
        o = *(char**)(self + 0x368);
        if (o != 0) {
            int* p = (int*)((long long)(int)(o + 0xb0) & 0xFFFFFFFFFFFFFFFFLL);
            *p |= 0x800000;
            *(char**)(self + 0x368) = 0;
        }
        if (_ZN6Player12FinishedAnimEv(self)) {
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_0211013c);
        }
    }
    return 0;
}
