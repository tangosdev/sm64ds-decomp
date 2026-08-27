//cpp
typedef short s16;
extern "C" int _Z15ApproachLinear2Rsss(s16 &dst, s16 a, s16 b);

extern "C" void func_ov006_021209ac(short *o)
{
    if (*(int *)((char *)o + 4) == 0) return;
    if (_Z15ApproachLinear2Rsss(o[1], 0, 1) != 0) {
        *(int *)((char *)o + 4) = 0;
    }
    if (((o[1] / 10) & 1) != 0) {
        *(int *)((char *)o + 8) = 0;
    } else {
        *(int *)((char *)o + 8) = 1;
    }
}
