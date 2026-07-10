/* func_ov002_020c1ad8 at 0x020c1ad8 (ov002)
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern int func_ov002_020c0434(char* c);
extern void func_ov002_020c0364(char* c, unsigned int arg);
extern int _ZN6Player7IsStateERNS_5StateE(char* self, void* st);
extern int func_ov002_020c19d0(char* self, int arg1, int arg2);
extern int AngleDiff(int a, int b);
extern void _ZN6Player11ChangeStateERNS_5StateE(char* self, void* st);

extern int data_ov002_0211013c;
extern int data_ov002_0211043c;
extern u8 data_020a0e40;
extern s16 data_0209f4a0[];

int func_ov002_020c1ad8(char* self, int ang)
{
    int d;
    int a = ang;

    if (*(u8*)(self + 0x6de) != 0) return 0;
    if (func_ov002_020c0434(self) != 0) {
        func_ov002_020c0364(self, 3);
        return 0;
    }
    if (!_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211013c)) {
        if (!_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211043c)) return 0;
    }
    if (func_ov002_020c19d0(self, 0x50, 0x32) == 0) return 0;

    a = (int)((long long)(a + 0x8000) & 0xFFFFFFFFFFFFFFFFLL);
    d = AngleDiff(*(s16*)(self + 0x6d2), (s16)a);
    if (d < 0x4000) {
        if (d < 0x1555) {
            if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211043c)) {
                if (*(u8*)(self + 0x6e3) == 2) return 0;
            }
            if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) == 0) return 0;
            *(u8*)(self + 0x6e3) = 2;
        } else {
            if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211043c)) {
                if (*(u8*)(self + 0x6e3) != 2) return 0;
            }
            *(u8*)(self + 0x6e3) = 0;
            if ((s16)a == (s16)(*(s16*)(self + 0x8e) + d)) {
                *(u8*)(self + 0x6e3) = 1;
            }
        }
        *(u16*)(self + 0x8e) = a;
        _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_0211043c);
        return 1;
    }
    return 0;
}
