//cpp
extern "C" {
struct BCA_File;
struct ModelAnim {
    void SetAnim(BCA_File*, int, int, unsigned int);
};
int _ZN8SaveData16HasPlayerLostCapEv(void);
extern char data_ov030_02115ce0[];

int func_ov030_02113a80(char* c)
{
    ((ModelAnim*)(c + 0xd4))->SetAnim(*(BCA_File**)(data_ov030_02115ce0 + 4), 0, 0x1000, 0);
    *(int*)(c + 0x130) = 0x1000;
    if (*(unsigned char*)(c + 0x3c8) == 0 && _ZN8SaveData16HasPlayerLostCapEv()) {
        *(unsigned char*)(c + 0x3c7) = 5;
    } else {
        char* p = *(char**)(c + 0x3a8);
        if (*(unsigned char*)(p + 0x6f9) != 0 ||
            *(unsigned char*)(p + 0x6fb) != 0 ||
            *(unsigned char*)(p + 0x6ff) != 0) {
            *(unsigned char*)(c + 0x3c7) = 4;
        } else {
            *(unsigned char*)(c + 0x3c7) = 0;
            unsigned char* f = (unsigned char*)(((unsigned long long)((int)(c) + 0x3c8)) & 0xFFFFFFFFFFFFFFFFULL);
            *f ^= 1;
        }
    }
    *(int*)(c + 0x3b4) = 3;
    return 1;
}
}
