#include "types.h"
extern u32 func_02022a4c(Fix12i x, Fix12i y, Fix12i z);
extern u32 func_02022d00(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, void* dir);
extern int _ZN6Player7IsStateERNS_5StateE(char* self, void* st);
extern void _ZN6Player11ChangeStateERNS_5StateE(char* self, void* st);
extern int func_ov002_020c7f10(char* c);
extern void func_ov002_020ce9c8(char* self);
extern void func_ov002_020be008(char* self);

extern int data_0209f32c;
extern int data_ov002_021102d4;
extern int data_ov002_021106ac;
extern int data_ov002_021106c4;
extern int data_ov002_021102a4;
extern int data_ov002_0211067c;

int func_ov002_020c0d90(char* c, int y)
{
    int flag = (*(u8*)(c + 0x707) != 0);

    *(u8*)(c + 0x707) = 0;
    if (y == (int)0x80000000) return 0;

    if (*(u8*)(c + 0x706) != 0) flag = 1;
    if (y >= *(int*)(c + 0x60) + 0xa000) {
        *(u8*)(c + 0x707) = 1;
    }

    if (flag != (*(u8*)(c + 0x707) != 0)) {
        if (*(u8*)(c + 0x6de) != 0) {
            if (*(u8*)(c + 0x703) == 0) {
                func_02022a4c(*(int*)(c + 0x5c), y, *(int*)(c + 0x64));
            } else {
                *(int*)(c + 0x628) = func_02022d00(*(int*)(c + 0x628), 0x56, *(int*)(c + 0x5c), data_0209f32c, *(int*)(c + 0x64), 0);
            }
        }
    }

    if (((y - 0x64000) >> 12) < (*(int*)(c + 0x60) >> 12)) return 0;
    if (*(int*)(c + 0xa8) > 0) return 0;

    if (*(u8*)(c + 0x6fd) != 0
        || (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021102d4) && *(u8*)(c + 0x6e3) < 2)
        || *(u8*)(c + 0x706) != 0
        || _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021106ac)
        || _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021106c4)
        || _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021102a4)) {
        return 0;
    }

    if (func_ov002_020c7f10(c)) return 1;

    *(u16*)(c + 0x6b8) = 0xf;
    if (*(u8*)(c + 0x6f9) != 0 || *(u8*)(c + 0x703) != 0) {
        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021106ac);
    } else {
        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211067c);
        *(u16*)(c + 0x6a8) = 0x14;
    }

    func_ov002_020ce9c8(c);
    func_ov002_020be008(c);
    return 1;
}
