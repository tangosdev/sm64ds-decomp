//cpp
typedef int Fix12;
struct BCA_File;
struct ModelAnim {
  void SetAnim(BCA_File *f, int a, Fix12 b, unsigned int c);
};
struct Sound {
  static void PlayBank2_2D(unsigned int);
};

extern "C" {
extern int data_ov006_0213b1a4[2];
extern int data_ov006_0213b1fc[2];
extern int data_ov006_02140540[];
extern int data_ov006_0213b20c[2];
}

extern "C" void func_ov006_020cb1a8(char *c)
{
    int *p;
    int *d;

    p = (int *)(((long long)(int)(c + 0x64)) & 0xFFFFFFFFFFFFFFFFLL);
    d = data_ov006_0213b1a4;
    if (p[0] == d[0]) {
        if (p[1] != d[1]) {
            if (*(int *)(c + 0x64) != 0)
                goto check2;
        }
        return;
    }
check2:
    p = (int *)(((long long)(unsigned)(c + 0x64)) & 0xFFFFFFFFFFFFFFFFLL);
    d = data_ov006_0213b1fc;
    if (p[0] == d[0]) {
        if (p[1] != d[1]) {
            if (*(int *)(c + 0x64) != 0)
                goto body;
        }
        return;
    }
body:
    *(int *)(c + 0x40) = 0x100;
    *(int *)(c + 0x34) = 0;
    *(int *)(c + 0x38) = 0x2000;
    Sound::PlayBank2_2D(0x1c9);
    ((ModelAnim *)(c + 0x6c))->SetAnim((BCA_File *)data_ov006_02140540[0], 0, 0x800, 0);
    *(int *)(c + 0xc4) = 0;
    {
        int a = data_ov006_0213b20c[0];
        int b = data_ov006_0213b20c[1];
        *(int *)(c + 0x64) = b ? a : a;
        *(int *)(c + 0x68) = b;
    }
}
