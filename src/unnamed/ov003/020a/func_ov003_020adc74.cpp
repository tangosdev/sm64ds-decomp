//cpp
namespace G2 { short* GetBG0ScrPtr(); }
extern "C" void func_ov003_020adc74(signed char* s, int r6, int r5) {
    short* p = G2::GetBG0ScrPtr() + (r5 + (r6 << 5));
    if (*s == 0) return;
    do {
        *p++ = (short)*s++;
    } while (*s != 0);
}
