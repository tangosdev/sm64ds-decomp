//cpp
struct BCA_File;
typedef int Fix12;
typedef struct { int v[2]; } V2;
extern "C" void func_ov006_020c8658(void *c);
namespace Sound { void PlayBank2_2D(unsigned int); }
struct ModelAnim { void SetAnim(BCA_File*, int, Fix12, unsigned int); };

extern int data_ov006_0213b088[2];
extern void *data_ov006_0214042c;
extern int data_ov006_0213b090[2];

extern "C" void func_ov006_020c8084(char *c)
{
    int *p = (int*)(((long long)(int)(c + 0x3c)) & 0xFFFFFFFFFFFFFFFFLL);
    int *g = data_ov006_0213b088;
    if (p[0] == g[0] && (p[1] == g[1] || *(int*)(c + 0x3c) == 0)) {
        *(int*)(c + 0x24) = 0;
        func_ov006_020c8658(c);
    } else {
        *(int*)(c + 0x20) = 0;
        *(int*)(c + 0x24) = 0x2000;
        Sound::PlayBank2_2D(0x1c9);
        ((ModelAnim*)(c + 0x4c))->SetAnim((BCA_File*)data_ov006_0214042c, 0, 0x800, 0);
        *(int*)(c + 0xa4) = 0;
        *(V2*)(c + 0x3c) = *(V2*)data_ov006_0213b090;
    }
}
