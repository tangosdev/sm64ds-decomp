typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;

extern void func_ov002_020f237c(u8* self);
extern void func_ov002_020f1fcc(u8* self);
extern void func_ov002_020f2340(u8* self);
extern void func_ov002_020f2210(u8* self);
extern void func_ov002_020f20f4(u8* self);

#pragma opt_strength_reduction off

extern void* data_0209f5bc;
extern u8 data_ov002_02111144;
extern u32 data_020a0db0;
extern u8 data_0209d454;

int func_ov002_020f23f0(u8* self)
{
    switch (*(u8*)(*(u8**)(self + 0xd4) + 0x34)) {
    case 0:
        {
            void* o = data_0209f5bc;
            if (((int (**)(void*))*(void**)o)[5](o) != 0) {
                {
                    u16* p = (u16*)(((int)*(u8**)(self + 0xd4) + 8));
                    *p = *p + 1;
                }
                {
                    u16* p = (u16*)(((int)*(u8**)(self + 0xd4) + 0xa));
                    *p = *p - 1;
                }
                if (*(u16*)(*(u8**)(self + 0xd4) + 8) == 8) {
                    u8* p = (u8*)(((int)*(u8**)(self + 0xd4) + 0x34));
                    *p = *p + 1;
                }
            }
        }
        break;
    case 1:
        if (*(u8*)(self + 0x100) == 3) {
            data_ov002_02111144 = 1;
            {
                u8* p = (u8*)(((int)*(u8**)(self + 0xd4) + 0x34));
                *p = *p + 1;
            }
        }
        break;
    case 2:
        if ((data_020a0db0 & 1) == 0) {
            {
                s16* p = (s16*)(((int)*(u8**)(self + 0xd4) + 6));
                *p = *p + 1;
            }
            {
                s16* p = (s16*)(((int)*(u8**)(self + 0xd4) + 4));
                *p = *p - 1;
            }
            if (*(s16*)(*(u8**)(self + 0xd4) + 6) >= 0x80) {
                int i;
                int v;
                {
                    u8* p = (u8*)(((int)*(u8**)(self + 0xd4) + 0x34));
                    *p = *p + 1;
                }
                i = 0;
                v = 1;
                for (; i < 3; i++) {
                    *(u16*)(*(u8**)(self + 0xd4) + i * 2 + 0x24) = v;
                    v += 0x40;
                }
            }
        }
        func_ov002_020f237c(self);
        func_ov002_020f1fcc(self);
        break;
    case 3:
        func_ov002_020f1fcc(self);
        func_ov002_020f237c(self);
        break;
    case 4:
        if (*(u8*)(self + 0x100) == 4) {
            u8* p = (u8*)(((int)*(u8**)(self + 0xd4) + 0x34));
            *p = *p + 1;
        }
        func_ov002_020f237c(self);
        break;
    case 5:
        if ((data_020a0db0 & 1) == 0) {
            {
                s16* p = (s16*)(((int)*(u8**)(self + 0xd4) + 6));
                *p = *p + 1;
            }
            {
                s16* p = (s16*)(((int)*(u8**)(self + 0xd4) + 4));
                *p = *p - 1;
            }
            if (*(s16*)(*(u8**)(self + 0xd4) + 6) >= 0xe0) {
                {
                    u8* p = (u8*)(((int)*(u8**)(self + 0xd4) + 0x34));
                    *p = *p + 1;
                }
                data_0209d454 = data_0209d454 & ~3;
                *(volatile u32*)0x4001000 = (*(volatile u32*)0x4001000 & ~0x1f00) | (data_0209d454 << 8);
            }
        }
        func_ov002_020f237c(self);
        break;
    case 6:
        break;
    }

    func_ov002_020f2340(self);
    func_ov002_020f2210(self);
    func_ov002_020f20f4(self);
    return 1;
}
