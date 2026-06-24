//cpp
typedef int Fix12;
struct BCA_File;
struct BlendModelAnim {
    void SetAnim(BCA_File &f, int a, int b, Fix12 c, unsigned short d);
};
struct Sound { static void PlayBank2_2D(unsigned int id); };
struct P2 { int w[2]; };

extern P2 data_ov006_0213ac50;

extern "C" void func_ov006_020c1764(char *c)
{
    if (*(short *)(c + 0x1a) == 1) {
        ((BlendModelAnim *)(c + 0x1c))->SetAnim(*(BCA_File *)*(void **)(c + 0x234), 0, 0, 0x800, 0);
        if (*(int *)(c + 0x26c) == 0) {
            Sound::PlayBank2_2D(0x13a);
            *(int *)(c + 0x26c) = 1;
        }
    } else {
        ((BlendModelAnim *)(c + 0x1c))->SetAnim(*(BCA_File *)*(void **)(c + 0x1fc), 0, 0, 0x800, 0);
    }
    *(P2 *)c = data_ov006_0213ac50;
}
