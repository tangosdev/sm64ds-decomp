#include "types.h"
enum { false, true };

extern int func_ov002_020d5c6c(char* c);
extern int _ZN6Player7IsStateERNS_5StateE(char* self, void* st);
extern int func_ov002_020d674c(char* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(char* self, void* st);
extern int func_ov002_020dde74(char* thiz);

extern u8 data_020a0e40;
extern u16 data_0209f49e[];

extern int data_ov002_021101e4;
extern int data_ov002_0211004c;
extern int data_ov002_021105a4;
extern int data_ov002_021105d4;
extern int data_ov002_0211019c;
extern int data_ov002_021101b4;
extern int data_ov002_02110724;
extern int data_ov002_021101fc;
extern int data_ov002_0211052c;
extern int data_ov002_0211055c;
extern int data_ov002_02110694;
extern int data_ov002_02110034;
extern int data_ov002_021105bc;
extern int data_ov002_02110574;

int func_ov002_020e2664(char* self)
{
    u16 flags;
    int ptr;
    int b1;
    int b2;

    if (func_ov002_020d5c6c(self)) return 1;

    flags = *(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18);
    if (flags & 0x400) {
        if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_021101e4)) {
            *(s16*)(self + 0x8e) += 0x8000;
        }
        if (func_ov002_020d674c(self)) {
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_0211004c);
        } else {
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_021105a4);
        }
        return 1;
    }

    if (!(flags & 1)) goto ret0;

    ptr = *(int*)(self + 0x358);
    b1 = !!ptr;
    if (b1 != false) {
        b2 = *(int*)(ptr + 0xb0);
        b2 = ((b2 & 0x200) != 0);
        if (b2 == false) {
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_021105d4);
        }
        return 1;
    }

    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211019c) && *(u8*)(self + 0x6e1) != 2) {
        if (func_ov002_020dde74(self)) return 1;
        goto ret0;
    }

    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_021101b4) ||
        _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110724) ||
        _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_021101fc) ||
        _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211052c) ||
        _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211055c) ||
        _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110694))
        return 0;

    if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_021101e4)) {
        *(s16*)(self + 0x8e) += 0x8000;
    }
    *(s16*)(self + 0x94) = *(s16*)(self + 0x8e);

    if (*(u8*)(self + 0x703) == 0) {
        if (*(int*)(self + 8) == 3) {
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_02110034);
        } else {
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_021105bc);
        }
    } else {
        *(u8*)(self + 0x6e2) = 2;
        _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_02110574);
    }

    *(s16*)(self + 0x8e) = *(s16*)(self + 0x94);
    return 1;

ret0:
    return 0;
}
