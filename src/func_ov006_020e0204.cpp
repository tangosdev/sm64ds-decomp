//cpp
#pragma opt_strength_reduction off

class C { public: int dummy; };
typedef void (C::*PMF)();

extern "C" PMF data_ov006_02141870[];

typedef struct Frame {
    int a, b;
} Frame;

extern "C" Frame *data_ov006_0213c0d8[];

extern "C" void func_ov006_020dedfc(char *o, int a, int b, int i);
extern "C" void func_ov006_020debfc(char *p);
extern "C" void func_ov006_020c2b8c(char *p);

extern "C" int func_ov006_020e0204(char *o)
{
    int i;
    (((C *)o)->*data_ov006_02141870[*(int *)(o + 0x5418)])();
    for (i = 0; i < 3; i++) {
        Frame *f = &data_ov006_0213c0d8[*(int *)(((int)o + i * 4 + 0x5434) & 0xFFFFFFFFFFFFFFFF)][*(int *)(o + i * 4 + 0x5440)];
        int n = f->b;
        if (n != 0) {
            *(int *)(((int)o + i * 4 + 0x544c) & 0xFFFFFFFFFFFFFFFF) += 1;
            if (*(int *)(((int)o + i * 4 + 0x544c) & 0xFFFFFFFFFFFFFFFF) >= n) {
                *(int *)(((int)o + i * 4 + 0x544c) & 0xFFFFFFFFFFFFFFFF) = 0;
                *(int *)(((int)o + i * 4 + 0x5440) & 0xFFFFFFFFFFFFFFFF) += 1;
                func_ov006_020dedfc(o, *(int *)(((int)o + i * 4 + 0x5434) & 0xFFFFFFFFFFFFFFFF),
                                    *(int *)(((int)o + i * 4 + 0x5440) & 0xFFFFFFFFFFFFFFFF), i);
            }
        }
    }
    func_ov006_020debfc(o + 0x50e8);
    func_ov006_020c2b8c(o + 0x4f38);
    return 1;
}
