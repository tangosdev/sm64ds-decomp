typedef unsigned int u32;
typedef unsigned char u8;

extern int RandomIntInternal(int* seed);
extern void SetBg2Offset(int a, int b);
extern void SetBg0Offset(int a, int b);

extern int data_0209d4b8;
extern int data_ov006_0212e534[];
extern int data_ov006_0212e554[];
extern u8 data_0209d45c;

void func_ov006_020e6528(char* c)
{
    int idx;

    if (*(int*)(c + 0x4fd0) == 0xff) {
        *(int*)(c + 0x4fd0) = 0;
    } else {
        idx = ((u32)RandomIntInternal(&data_0209d4b8) >> 16 & 0x7fff) * 4 >> 15;
        if (*(int*)(c + 0x4fd0) == idx) {
            int t = ((u32)RandomIntInternal(&data_0209d4b8) >> 16 & 0x7fff) * 3 >> 15;
            t += 1;
            idx += t;
            if (idx >= 4) idx -= 4;
        }
        *(int*)(c + 0x4fd0) = idx;
    }

    *(int*)(c + 0x4fc8) = data_ov006_0212e534[*(int*)(c + 0x4fd0) * 2];
    *(int*)(c + 0x4fcc) = data_ov006_0212e554[*(int*)(c + 0x4fd0) * 2];
    *(int*)(c + 0x4fd4) = data_ov006_0212e534[*(int*)(c + 0x4fd0) * 2 + 1];
    *(int*)(c + 0x4fd8) = data_ov006_0212e554[*(int*)(c + 0x4fd0) * 2 + 1];

    SetBg2Offset(0x80 - *(int*)(c + 0x4fc8), 0x80 - *(int*)(c + 0x4fcc));

    if (*(int*)(c + 0x4fd0) == 0) {
        data_0209d45c &= ~1;
        return;
    }
    SetBg0Offset(0x80 - *(int*)(c + 0x4fd4), 0x80 - *(int*)(c + 0x4fd8));
    data_0209d45c |= 1;
}
