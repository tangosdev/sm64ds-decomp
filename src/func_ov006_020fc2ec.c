typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

extern u8 data_ov006_0212eb10[];
extern u8 data_ov006_0212eb1c[];
extern s32 data_ov006_0212eb60[];

#define ST(c,b)       (*(u8*)((c) + 0x4eb4 + (b)))
#define CTA(c,b)      (*(u16*)((c) + 0x4eb0 + (b)))
#define FB(c,b)       (*(u8*)((c) + 0x4eb6 + (b)))
#define OUTFLAG(c,b)  (*(u8*)((c) + 0x4eb5 + (b)))
#define OUTFLAG2(c,b) (*(u8*)((c) + 0x4eb3 + (b)))
#define ACC(c,b)      (*(s32*)((c) + 0x4ea0 + (b)))
#define VEL(c,b)      (*(s32*)((c) + 0x4ea8 + (b)))

void func_ov006_020fc2ec(char* c, int i)
{
    int b = i * 0x1c;
    int v;

    if (ST(c, b) != 0) {
        CTA(c, b)++;
        if (CTA(c, b) < 4)
            return;
        CTA(c, b) = 0;
        FB(c, b)++;
        if (FB(c, b) >= 3) {
            FB(c, b) = 0;
            OUTFLAG2(c, b) = 1;
            ST(c, b) = 0;
            return;
        }
        OUTFLAG(c, b) = data_ov006_0212eb10[FB(c, b)];
        return;
    }

    CTA(c, b)++;
    if (CTA(c, b) >= 4) {
        CTA(c, b) = 0;
        FB(c, b)++;
        if (FB(c, b) >= 6) {
            FB(c, b) = 0;
        }
        OUTFLAG(c, b) = data_ov006_0212eb1c[FB(c, b)];
    }

    ACC(c, b) += VEL(c, b);
    v = ACC(c, b) >> 0xc;
    if (v >= data_ov006_0212eb60[i + 2]) {
        if (VEL(c, b) >= 0x800)
            VEL(c, b) -= 0x80;
    } else {
        VEL(c, b) += 0x80;
    }
    if (v < data_ov006_0212eb60[i])
        return;

    ACC(c, b) = data_ov006_0212eb60[i] << 0xc;
    VEL(c, b) = -0x1000;
    ST(c, b)++;
    CTA(c, b) = 0;
    FB(c, b) = 0;
    OUTFLAG(c, b) = data_ov006_0212eb10[0];
}
