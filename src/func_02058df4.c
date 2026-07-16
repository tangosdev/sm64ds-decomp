typedef unsigned int u32;
typedef int s32;

extern char data_023c0000;
extern void func_00000600(void);
extern void func_00000000(void);

int func_02058df4(unsigned int idx)
{
    switch (idx) {
    case 0:
        return 0x23c0000;
    case 2:
        return 0x2700000;
    case 3:
        return 0x2000000;
    case 4: {
        u32 irqStackLo = (u32)&data_023c0000 + 0x3f80 - (s32)func_00000600;
        u32 sysStackLo;

        if (!(s32)func_00000000) {
            sysStackLo = (u32)&data_023c0000;
            if (sysStackLo < 0x23c0020) {
                sysStackLo = 0x23c0020;
            }
        } else if ((s32)func_00000000 < 0) {
            sysStackLo = (u32)&data_023c0000 - (s32)func_00000000;
        } else {
            sysStackLo = irqStackLo - (s32)func_00000000;
        }
        return sysStackLo;
    }
    case 5:
        return 0x27ff800;
    case 6:
        return 0x37f8000;
    default:
        return 0;
    }
}
