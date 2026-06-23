//cpp
typedef int Fix12;
struct BCA_File;
struct ModelAnim { int pad; void SetAnim(BCA_File *f, int a, Fix12 b, unsigned int c); };
struct Sound { static void PlayBank2_2D(unsigned int); };
extern "C" {
extern int data_ov006_02140428[];
extern int data_ov006_0214042c[];
extern double data_ov006_0213b030;
}

extern "C" void func_ov006_020c79a8(char *c)
{
    if (data_ov006_02140428[0] > 1)
        Sound::PlayBank2_2D(0x1ca);
    else
        Sound::PlayBank2_2D(0x1c9);
    ((ModelAnim *)(c + 0x4c))->SetAnim((BCA_File *)data_ov006_0214042c[0], 0, 0x800, 0);
    *(short *)(c + 0x32) = 0x28;
    *(double *)(c + 0x3c) = data_ov006_0213b030;
}
