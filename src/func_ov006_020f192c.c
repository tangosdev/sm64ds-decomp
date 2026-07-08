typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef long long s64;

extern int data_ov006_0212e868[];
extern s16 SINE_TABLE[];

void func_ov006_020f1dbc(char* c, int i);

void func_ov006_020f192c(char* c, int i) {
    u8* counter = (u8*)(c + 0x5275);
    int cnt;
    int j;
    if (counter[i] == 0) {
        counter[i]++;
        cnt = 0;
        j = i;
        if (i >= 8) {
            do {
                j -= 8;
                cnt++;
            } while (j >= 8);
        }
        if (*(u8*)(c + cnt + 0x51f5) != 0) {
            *(u16*)(c + i * 2 + 0x4f7c) = 0x8000;
        } else {
            *(u16*)(c + i * 2 + 0x4f7c) = 0;
        }
        return;
    }
    {
        u8* spd = (u8*)(c + 0x5365);
        int* xs = (int*)(c + 0x47f8);
        int* ys = (int*)(c + 0x49d8);
        int a = (int)*(u16*)(c + i * 2 + 0x4f7c) >> 4;
        xs[i] = xs[i] + (int)(((s64)SINE_TABLE[a * 2 + 1] * data_ov006_0212e868[spd[i]] + 0x800) >> 12);
        a = (int)*(u16*)(c + i * 2 + 0x4f7c) >> 4;
        ys[i] = ys[i] + (int)(((s64)SINE_TABLE[a * 2] * data_ov006_0212e868[spd[i]] + 0x800) >> 12);
    }
    func_ov006_020f1dbc(c, i);
}
