typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

extern u8 data_020a1040[];
extern u8 data_020a1154[];
extern u16 data_020a117c[4];
extern u16 data_020a11a0[4];
extern u16 data_020a11c4[4];

#define H(base, off) (*(u16 *)((char *)(base) + (off)))

void func_0203e0ac(void)
{
    int i;
    u16 te;
    u8 *p1040 = data_020a1040;
    u8 *p1154 = data_020a1154;

    te = H(p1040, 0xc) & 0x7fff;
    H(p1040, 0xc) = te;
    H(p1154, 0x78) = H(p1040, 0xc);
    H(p1154, 0x7a) = H(p1040, 0xe);
    H(p1154, 0x56) = H(p1154, 0x7a);
    H(p1154, 0x54) = H(p1154, 0x78);
    H(p1154, 0x30) = H(p1154, 0x54);
    H(p1154, 0x32) = H(p1154, 0x56);
    H(p1154, 0xc) = H(p1154, 0x30);
    H(p1154, 0xe) = H(p1154, 0x32);
    H(p1154, 0x70) = H(p1040, 4);
    H(p1154, 0x72) = H(p1040, 6);
    H(p1154, 0x74) = H(p1040, 8);
    H(p1154, 0x76) = H(p1040, 0xa);

    {
        u16 *p11c4 = (u16 *)data_020a11c4;
        H(p1154, 0x4c) = p11c4[0];
        H(p1154, 0x4e) = p11c4[1];
        H(p1154, 0x50) = p11c4[2];
        H(p1154, 0x52) = p11c4[3];
    }
    {
        u16 *p11a0 = (u16 *)data_020a11a0;
        H(p1154, 0x28) = p11a0[0];
        H(p1154, 0x2a) = p11a0[1];
        H(p1154, 0x2c) = p11a0[2];
        H(p1154, 0x2e) = p11a0[3];
    }
    {
        u16 *p117c = (u16 *)data_020a117c;
        H(p1154, 4) = p117c[0];
        H(p1154, 6) = p117c[1];
        H(p1154, 8) = p117c[2];
        {
            u16 last = p117c[3];
            u16 h10 = H(p1040, 0x10);
            H(p1154, 0x7c) = h10;
            h10 = H(p1154, 0x7c);
            H(p1154, 0xa) = last;
            H(p1154, 0x58) = h10;
            h10 = H(p1154, 0x58);
            H(p1154, 0x34) = h10;
            h10 = H(p1154, 0x34);
            H(p1154, 0x10) = h10;
        }
    }

    i = 0;
    do {
        p1154[0x7e] = p1040[0x12];
        i += 1;
        p1040 += 1;
        p1154[0x5a] = p1154[0x7e];
        p1154[0x36] = p1154[0x5a];
        p1154[0x12] = p1154[0x36];
        p1154 += 1;
    } while (i < 0x11);
}
