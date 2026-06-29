typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

void func_02012718(void *a, int b);

void func_ov006_020d6b88(char *this, int idx)
{
    char *b = this + idx * 0x40;
    u8 flag = *(u8*)(b + 0x4696);
    int v3 = *(int*)(b + 0x4660) >> 0xc;
    int v0 = *(int*)(b + 0x4664) >> 0xc;
    if (flag != 0) {
        if (v3 <= 0xc0) return;
        if (v0 <= 0x40) return;
        if (v0 >= 0x80) return;
        *(u8*)(b + 0x4697) = 5;
        *(u8*)(b + 0x4694) = 0;
        *(u8*)(b + 0x4695) = 0;
        *(u16*)(b + 0x468e) = 0;
        *(int*)(b + 0x4670) = 0x999;
        func_02012718((void*)0x1dc, *(int*)(b + 0x4660));
    } else {
        if (v3 >= 0x40) return;
        if (v0 <= 0x40) return;
        if (v0 >= 0x80) return;
        *(u8*)(b + 0x4697) = 5;
        *(u8*)(b + 0x4694) = 0;
        *(u8*)(b + 0x4695) = 0;
        *(u16*)(b + 0x468e) = 0;
        *(int*)(b + 0x4670) = 0x999;
        func_02012718((void*)0x1dc, *(int*)(b + 0x4660));
    }
}
