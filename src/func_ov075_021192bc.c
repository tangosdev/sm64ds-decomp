extern int func_020124c4(int a, int b, int c, int d);
extern int func_0203d890(void);
extern void func_020308b4(int levelID);
extern int func_020308a8(void);
extern int func_0203d8fc(void);
extern void func_ov075_0211a100(char* c, void* r1);
extern void func_02012790(int a);

extern unsigned char NUM_PLAYERS_2;
extern unsigned char data_0209fc54;
extern unsigned char data_0208f274;
extern int data_ov075_0211d768[];

void func_ov075_021192bc(char* self)
{
    *(int*)(self + 0x278) = func_020124c4(*(int*)(self + 0x278), 2, 0x11d, 0);

    switch (*(unsigned char*)(self + 0x282)) {
    case 0:
        {
            unsigned char v = NUM_PLAYERS_2;
            int r = func_0203d890();
            if (v == r) {
                func_020308b4(r);
                (*(unsigned char*)(((int)self + 0x282) & 0xFFFFFFFFFFFFFFFFLL))++;
            } else {
                (*(int*)(((int)self + 0x268) & 0xFFFFFFFFFFFFFFFFLL))++;
                if (*(int*)(self + 0x268) >= 0x384) {
                    data_0209fc54 = 1;
                }
            }
        }
        break;
    case 1:
        if (func_020308a8() != 0) {
            (*(unsigned char*)(((int)self + 0x282) & 0xFFFFFFFFFFFFFFFFLL))++;
        }
        break;
    case 2:
        {
            int r = func_0203d8fc();
            if (r != 0) {
                func_020308b4(r);
                (*(unsigned char*)(((int)self + 0x282) & 0xFFFFFFFFFFFFFFFFLL))++;
            }
        }
        break;
    case 3:
        if (func_020308a8() != 0) {
            unsigned char v;
            int r;
            func_ov075_0211a100(self, data_ov075_0211d768);
            v = data_0208f274;
            r = func_0203d890();
            if (v == r) {
                func_02012790(0x11f);
                return;
            }
        }
        break;
    }

    if (*(unsigned char*)(self + 0x282) < 1) return;
    (*(int*)(((int)self + 0x274) & 0xFFFFFFFFFFFFFFFFLL))++;
    if (*(int*)(self + 0x274) >= 0xa8c) {
        data_0209fc54 = 1;
    }
}
