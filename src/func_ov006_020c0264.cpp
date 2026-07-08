//cpp
typedef int Fix12;
struct BCA_File;
struct BlendModelAnim {
    void SetAnim(BCA_File &f, int a, int b, Fix12 c, unsigned short d);
};
struct P2 { int w[2]; };

int ApproachLinear2(short &v, short t, short step);
extern "C" int RandomIntInternal(int *seed);
extern int RNG_STATE;
extern P2 data_ov006_0213ac88;

extern "C" void func_ov006_020c0264(char *c)
{
    if (ApproachLinear2(*(short *)(c + 0xf2), 0, 1) == 0)
        return;
    ((BlendModelAnim *)(c + 0x18))->SetAnim(*(BCA_File *)*(void **)(c + 0x14), 0, 0, 0x800, 0);
    int r = RandomIntInternal(&RNG_STATE);
    int idx = (int)((unsigned int)(r & 0x7fffffff) >> 0x13);
    *(short *)(c + 0xf2) = (short)(((idx * 0x258) >> 12) + 0x258);
    *(int *)(c + 0xf4) = 0;
    *(P2 *)(c + 0xb4) = data_ov006_0213ac88;
}
