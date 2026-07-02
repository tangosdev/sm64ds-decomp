typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

void func_ov002_020c70ac(char* c) {
    if (*(u16*)(c+0x6a4) != 0) {
        *(int*)(c+0xa8) = 0;
        *(int*)(c+0x98) = 0;
        return;
    }
    *(u16*)(c+0x6a6) = 0;
    *(u8*)(c+0x6e5) = 0;
    *(int*)(c+0x9c) = -0x4000;
    switch (*(u8*)(c+0x6e3)) {
    case 12:
        *(s16*)(c+0x8e) += 0x8000;
    case 11:
    case 18:
        *(int*)(c+0x98) = 0x20000;
        *(u16*)(c+0x6a6) = 6;
        break;
    case 9:
        *(s16*)(c+0x8e) += 0x8000;
    case 8:
    case 16:
        *(int*)(c+0xa8) = 0x40000;
        *(int*)(c+0x98) = 0x18000;
        break;
    case 13:
        *(int*)(c+0xa8) = 0x36000;
        *(int*)(c+0x98) = 0xa000;
        *(u8*)(c+0x70c) = 0;
        break;
    case 17:
        *(int*)(c+0xa8) = 0x3c000;
        *(int*)(c+0x98) = 0xa000;
        *(u8*)(c+0x70c) = 0;
        break;
    }
}
