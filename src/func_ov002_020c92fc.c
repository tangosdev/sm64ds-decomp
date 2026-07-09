typedef unsigned char u8;

extern int _ZNK6Player14GetBodyModelIDEjb(char *self, unsigned int a, int b);
extern int _ZNK9Animation12WillHitFrameEi(void *anim, int f);
extern int _ZN6Player12FinishedAnimEv(char *self);
extern int _ZN8SaveData19IsCharacterUnlockedEj(unsigned int a);
extern void func_0200d3f8(void *thiz, unsigned char playerID, void *ptr);
extern void _ZN6Player11ChangeStateERNS_5StateE(char *self, void *st);
extern int data_0209b454;
extern unsigned char data_0209f2d8;
extern int data_0209caa0[];
extern void *data_0209f318;
extern int data_ov002_0211067c;
extern int data_ov002_0211013c;

int func_ov002_020c92fc(char *self)
{
    int flag = 0;
    *(int *)(self + 0xa8) = 0;

    if (*(int *)(self + 0x688) != -1) {
        if (*(u8 *)(self + 0x706) == 0) {
            int id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, flag);
            char *anim = (char *)((int *)(self + 0xdc))[id] + 0x50;
            if (_ZNK9Animation12WillHitFrameEi(anim, 0x20)) flag = 1;
        } else {
            if (_ZN6Player12FinishedAnimEv(self)) flag = 1;
        }
    }

    if (flag != 0) {
        *(unsigned short *)(self + 0x6a4) = 0x14;
        *(u8 *)(self + 0x6e3) = 0x14;
        *(int *)(self + 0xa8) = 0;
        *(int *)((int)(self + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) |= 0x800000;
        data_0209b454 |= 0x800000;
        return 0;
    }

    if (_ZN6Player12FinishedAnimEv(self)) {
        if (*(u8 *)(self + 0x6e3) == 9) {
            if ((int)(data_0209f2d8 == 1) == 0) {
                if ((data_0209caa0[2] & 0x10000000) == 0
                    && _ZN8SaveData19IsCharacterUnlockedEj(0) == 0
                    && *(u8 *)(self + 0x6dd) == 0
                    && *(u8 *)(self + 0x6dc) == 3) {
                    *(u8 *)(self + 0x6e3) = 0xa;
                    *(u8 *)(self + 0x6e6) = 0;
                    func_0200d3f8(data_0209f318, *(u8 *)(self + 0x6d8), 0);
                    return 0;
                }
            }
        }
        if (*(u8 *)(self + 0x706) != 0)
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_0211067c);
        else
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_0211013c);
    }
    return 0;
}
