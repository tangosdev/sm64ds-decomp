#include "types.h"
extern u8 data_020a0e40;
extern u16 data_0209f49c[];
extern char data_ov002_02110424;

struct Player {
    char pad0[0x8e];
    s16 field_8e;
    char pad2[0x94 - 0x90];
    s16 field_94;
    char pad3[0xa0 - 0x96];
    int field_a0;
    char pad4[0x6de - 0xa4];
    u8 field_6de;
};

extern void func_ov002_020e28d4(struct Player *thiz, int a, int b);
extern void Player_AdvanceAnims(struct Player *thiz);
extern void _ZN6Player11ChangeStateERNS_5StateE(struct Player *thiz, void *st);

int _ZN6Player12St_Spin_MainEv(struct Player *thiz)
{
    func_ov002_020e28d4(thiz, 0x1000, 0x1000);
    if (thiz->field_6de == 0) {
        thiz->field_8e = thiz->field_94;
        _ZN6Player11ChangeStateERNS_5StateE(thiz, &data_ov002_02110424);
    }
    if (*(u16*)((char*)data_0209f49c + (&data_020a0e40)[0] * 0x18) & 2) {
        thiz->field_a0 = -0x9000;
        *(s16 *)(int)(((long long)(int)((char *)thiz + 0x8e))) += 0x2000;
    } else {
        thiz->field_a0 = -0xc000;
        *(s16 *)(int)(((long long)(int)((char *)thiz + 0x8e))) += 0x1800;
    }
    Player_AdvanceAnims(thiz);
    return 1;
}
