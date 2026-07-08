typedef signed short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef long long s64;

extern s16 SINE_TABLE[];
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void* v);
extern void func_ov036_02111618(char* c);

int func_ov036_021116c0(char* c) {
    s16 a = *(u16*)(c + 0x116) << 10;
    if (*(u8*)(c + 0x118) == 0) {
        int idx = ((u16)a >> 4) * 2;
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x112) + (int)((((s64)SINE_TABLE[idx] << 13) + 0x800) >> 12);
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x110) + (int)((((s64)SINE_TABLE[idx + 1] << 11) + 0x800) >> 12);
    } else {
        int idx = ((u16)a >> 4) * 2;
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x112) - (int)((((s64)SINE_TABLE[idx] << 13) + 0x800) >> 12);
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x110) + (int)((((s64)SINE_TABLE[idx + 1] << 11) + 0x800) >> 12);
    }
    {
        u16 *p = (u16 *)(((int)c + 0x116) & 0xFFFFFFFFFFFFFFFF);
        *p = *p + 1;
    }
    if (*(u8*)(c + 0x119) != 0) {
        if (*(u16*)(c + 0x116) == 0x40) {
            _ZN5Sound9PlayBank3EjRK7Vector3(0x75, c + 0x74);
            *(u16 *)(c + 0x116) = 0;
        }
    }
    func_ov036_02111618(c);
    return 1;
}
