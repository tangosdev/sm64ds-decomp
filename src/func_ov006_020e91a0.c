typedef short s16;
typedef unsigned char u8;

extern u8 data_020a0e40;
extern u8 TOUCH_INPUT_ARR[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];

extern int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern void func_ov006_020e8d7c(char* c, int idx);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);

void func_ov006_020e91a0(char* self, int idx)
{
    unsigned int i = data_020a0e40;
    int ok = 0;
    int n;
    int v, w;

    if (TOUCH_INPUT_ARR[i * 4] != 0) {
        int t = data_020a0de9[i * 4] != 0;
        if (t) {
            ok = 1;
        }
    }
    if (ok == 0) return;

    n = idx * 0x18;
    v = data_020a0dea[data_020a0e40 * 4] - (*(int*)(self + 0x5208 + n) >> 12);
    w = data_020a0deb[data_020a0e40 * 4] - (*(int*)(self + 0x520c + n) >> 12);

    if (v < -0xa) return;
    if (v > 0xa) return;
    if (w < -0x18) return;
    if (w > 0x18) return;

    {
        u8* pc = (u8*)((long long)(int)(self + 0x554f) & 0xFFFFFFFFFFFFFFFFLL);
        *pc = *pc + 1;
    }
    *(s16*)(self + 0x5214 + n) = 0x20;
    *(int*)(self + 0x5210 + n) = 0;
    *(u8*)(self + 0x5219 + n) = 2;

    *(s16*)(self + 0x5216 + n) = _ZN4cstd5atan2E5Fix12IiES1_(
        0x30 - (*(int*)(self + 0x520c + n) >> 12),
        0x80 - (*(int*)(self + 0x5208 + n) >> 12));

    func_ov006_020e8d7c(self, idx);
    _ZN5Sound12PlayBank2_2DEj(0x190);
}
