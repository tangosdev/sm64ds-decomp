typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

extern unsigned char data_ov006_0212eb0c[];
extern unsigned char data_ov006_0212eb14[];
extern int data_ov006_0212eb50[];

void func_ov006_020fc500(char* c, int i) {
    int b = i * 0x1c;
    int t;

    if (*(u8*)(c + 0x4eb4 + b) != 0) {
        (*(u16*)(c + 0x4eb0 + b))++;
        if (*(u16*)(c + 0x4eb0 + b) < 4) {
            return;
        }
        *(u16*)(c + 0x4eb0 + b) = 0;
        (*(u8*)(c + 0x4eb6 + b))++;
        if (*(u8*)(c + 0x4eb6 + b) >= 3) {
            *(u8*)(c + 0x4eb6 + b) = 0;
            *(u8*)(c + 0x4eb3 + b) = 2;
            *(u8*)(c + 0x4eb4 + b) = 0;
            return;
        }
        *(u8*)(c + 0x4eb5 + b) = data_ov006_0212eb0c[*(u8*)(c + 0x4eb6 + b)];
        return;
    }

    (*(u16*)(c + 0x4eb0 + b))++;
    if (*(u16*)(c + 0x4eb0 + b) >= 4) {
        *(u16*)(c + 0x4eb0 + b) = 0;
        (*(u8*)(c + 0x4eb6 + b))++;
        if (*(u8*)(c + 0x4eb6 + b) >= 6) {
            *(u8*)(c + 0x4eb6 + b) = 0;
        }
        *(u8*)(c + 0x4eb5 + b) = data_ov006_0212eb14[*(u8*)(c + 0x4eb6 + b)];
    }

    *(s32*)(c + 0x4ea0 + b) = *(s32*)(c + 0x4ea0 + b) + *(s32*)(c + 0x4ea8 + b);
    t = *(s32*)(c + 0x4ea0 + b) >> 0xc;
    if (t <= data_ov006_0212eb50[i + 2]) {
        if (*(s32*)(c + 0x4ea8 + b) <= -0x800) {
            *(s32*)(c + 0x4ea8 + b) = *(s32*)(c + 0x4ea8 + b) + 0x80;
        }
    } else {
        *(s32*)(c + 0x4ea8 + b) = *(s32*)(c + 0x4ea8 + b) - 0x80;
    }

    if (t > data_ov006_0212eb50[i]) {
        return;
    }
    *(s32*)(c + 0x4ea0 + b) = data_ov006_0212eb50[i] << 0xc;
    *(s32*)(c + 0x4ea8 + b) = 0x1000;
    (*(u8*)(c + 0x4eb4 + b))++;
    *(u16*)(c + 0x4eb0 + b) = 0;
    *(u8*)(c + 0x4eb6 + b) = 0;
    *(u8*)(c + 0x4eb5 + b) = data_ov006_0212eb0c[0];
}
