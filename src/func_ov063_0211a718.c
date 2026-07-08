typedef signed short s16;
typedef unsigned short u16;

extern s16 SINE_TABLE[];

void func_ov063_0211a718(char* o) {
    int a;
    s16 v;
    s16* p;
    a = (u16)(s16)((*(u16*)(o + 0x100) - 0x1f) << 13) >> 4;
    p = (s16*)(((long long)(int)(o + 0x8e)) & 0xFFFFFFFFFFFFFFFFLL);
    v = *p;
    *p = v + (SINE_TABLE[a * 2 + 1] << 10) / 4096;
}
