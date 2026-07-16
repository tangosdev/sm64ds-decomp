typedef short s16;
typedef unsigned short u16;

extern int _ZN5Actor14GetSubtractionEss(void* actor, s16 a, s16 b);
extern int _Z14ApproachLinearRsss(s16* dst, s16 target, int step);
extern int func_ov060_02115744(void* c);
extern int func_ov060_02115718(void* c);
extern int func_ov060_021156ec(void* c);

void func_ov060_02114d08(char* self)
{
    int r4;
    int step;

    r4 = _ZN5Actor14GetSubtractionEss(self, *(s16*)(self + 0x8e), *(s16*)(self + 0x406));

    if (*(unsigned char*)(self + 0x414) == 1) {
        step = 0x400;
    } else {
        int t = *(signed char*)(self + 0x41e);
        if (t > 2) {
            step = 0x400;
        } else if (t == 2) {
            step = 0x300;
        } else {
            step = 0x200;
        }
    }
    _Z14ApproachLinearRsss((s16*)(self + 0x8e), *(s16*)(self + 0x406), step);

    if (*(unsigned char*)(self + 0x423) == 0) {
        *(short*)(self + 0x3fe) = 0;
        if (func_ov060_02115744(self) == 0) return;
        (*(unsigned char*)(((int)self + 0x423) & 0xFFFFFFFFFFFFFFFFLL))++;
        return;
    }
    if (*(unsigned char*)(self + 0x423) <= 2) {
        if (func_ov060_02115718(self) == 0) return;
        (*(u16*)(((int)self + 0x3fe) & 0xFFFFFFFFFFFFFFFFLL))++;
        if (*(int*)(self + 0x418) & 0x20000) {
            if (*(u16*)(self + 0x3fe) < 5) return;
            *(int*)(((int)self + 0x418) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x20000;
            return;
        }
        if (r4 >= 0x2000) return;
        *(int*)(self + 0x12c) = 0;
        (*(unsigned char*)(((int)self + 0x423) & 0xFFFFFFFFFFFFFFFFLL))++;
        *(short*)(self + 0x3fe) = 0;
        return;
    }
    if (func_ov060_021156ec(self) != 0) {
        *(int*)(self + 0x40c) = 0;
    }
}
