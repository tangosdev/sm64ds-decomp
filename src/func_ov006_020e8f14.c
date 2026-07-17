typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef long long s64;

extern int RandomIntInternal(int *seed);
extern s16 data_02082214[];
extern int data_0209d4b8;
extern void func_ov006_020e7d7c(char *c);

void func_ov006_020e8f14(char *self, int i)
{
    int n;
    u16 timer;
    s16 tv1;
    s16 tv2;
    int speed2;
    int speed1;
    int angle1;
    char *angleBase;
    char *speedBase;

    n = i * 0x18;
    timer = *(u16 *) (self + 0x5214 + n);
    if (timer != 0)
    {
        *(u16 *) (self + 0x5214 + n) = timer - 1;
        if ((*(s16 *) (self + 0x5214 + n)) <= 0)
        {
            *(u16 *) (self + 0x5214 + n) = 0;
        }
        return;
    }

    speedBase = self + 0x5210;
    angleBase = self + 0x5216;
    speed1 = *(int *) (speedBase + n);
    angle1 = *(u16 *) (angleBase + n);
    angle1 >>= 4;
    tv1 = data_02082214[(angle1 << 1) + 1];
    *(int *) (self + 0x5208 + n) = (*(int *) (self + 0x5208 + n)) + ((int) ((((s64) tv1) * speed1 + 0x800) >> 0xc));

    tv2 = data_02082214[(*(u16 *) (angleBase + n) >> 4) << 1];
    speed2 = *(int *) (speedBase + n);
    *(int *) (self + 0x520c + n) = (*(int *) (self + 0x520c + n)) + ((int) ((((s64) tv2) * speed2 + 0x800) >> 0xc));
    *(int *) (speedBase + n) = (*(int *) (speedBase + n)) + 0x300;

    {
        int dx = 0x80 - ((*(int *) (self + 0x5208 + n)) >> 0xc);
        int dz = 0x30 - ((*(int *) (self + 0x520c + n)) >> 0xc);
        if (dx < -6)
            return;
        if (dx > 6)
            return;
        if (dz < -6)
            return;
        if (dz > 6)
            return;
    }

    *(int *) (self + 0x5208 + n) = 0x80000;
    *(int *) (self + 0x520c + n) = 0x30000;
    *(u8 *) (self + 0x5218 + n) = 0;

    if (*(u8 *) (self + 0x5552) == 0)
    {
        if ((*(u8 *) (self + 0x5551)) == (*(u8 *) (self + 0x521b + n)))
        {
            *(u8 *) (self + 0x5550) = 1;
        }
        else
        {
            int rnd = RandomIntInternal(&data_0209d4b8);
            u32 temp = ((u32) rnd >> 16) & 0x7fff;
            if (((temp << 1) >> 0xf) == 0)
            {
                *(u8 *) (self + 0x5550) = 0;
            }
            else
            {
                *(u8 *) (self + 0x5551) = *(u8 *) (self + 0x521b + n);
                *(u16 *) (self + 0x51f0) = *(u8 *) (self + 0x521b + n);
                *(u8 *) (self + 0x5550) = 1;
            }
        }
    }
    else
    {
        if ((*(u8 *) (self + 0x5551)) == (*(u8 *) (self + 0x521b + n)))
        {
            *(u8 *) (self + 0x5550) = 1;
        }
        else
        {
            *(u8 *) (self + 0x5550) = 0;
        }
    }

    *(int *) (self + 0x553c) = 2;
    *(int *) (self + 0x5540) = 0;
    *(u16 *) (self + 0x5548) = 0x90;
    if (*(u8 *) (self + 0x5550) == 0)
    {
        u16 *p548 = (u16 *) ((long long) (int) (self + 0x5548) & 0xFFFFFFFFFFFFFFFFLL);
        *p548 = *p548 + 0x40;
    }

    func_ov006_020e7d7c(self + 0x4fd8);
}
