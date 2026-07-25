typedef short s16;
typedef unsigned char u8;
typedef long long s64;

#define LP(p) ((long long)(int)(p) & 0xFFFFFFFFFFFFFFFFLL)

extern void Vec3_RotateYAndTranslate(int *out, int *in, short angle, int *src);
extern int func_020091f8(void *a, int b, int c, int d);
extern unsigned int func_020093f4(void *p, int x);
extern int func_02009138(int *thiz, int arg);
extern void func_020089d8(void *p);

extern int data_02086e9c[3];
extern unsigned char data_020a0e40;
extern short data_0209f4a2[];
extern short data_0209f4a4[];

int func_020098e0(char *self)
{
    if (*(u8*)(self + 0x1a6) != 0) {
        int pos[3];
        int src[3];
        int result;
        src[0] = data_02086e9c[0];
        src[1] = data_02086e9c[1];
        src[2] = data_02086e9c[2];

        {
            char *p110 = *(char**)(self + 0x110);
            Vec3_RotateYAndTranslate(pos, (int*)(p110 + 0x5c), *(short*)(p110 + 0x8e), src);
        }

        result = func_020091f8(self, (int)pos, *(short*)(self + 0x186), 0);
        if (result != 0) {
            *(u8*)(self + 0x1a6) = 0;
        }
    } else {
        s16 *p17c = (s16*)(int)LP(self + 0x17c);
        int idx = data_020a0e40;
        int off = idx * 0x18;
        s16 t1 = *(short*)((char*)data_0209f4a2 + off);
        int scaled1 = (int)((((s64)t1 * 0x200) + 0x800) >> 12);

        *p17c = (s16)(*p17c - scaled1);

        {
            s16 a186 = *(short*)(self + 0x186);
            s16 diff = (s16)(*(short*)(self + 0x17c) - a186);
            if (diff < -0x5000) diff = -0x5000;
            else if (diff > 0x5000) diff = 0x5000;
            *(short*)(self + 0x100 + 0x7c) = (s16)(a186 + diff);
        }

        {
            s16 *p17e = (s16*)(int)LP(self + 0x17e);
            s16 t2 = *(short*)((char*)data_0209f4a4 + off);
            int scaled2 = (int)((((s64)t2 * 0x200) + 0x800) >> 12);
            *p17e = (s16)(*p17e + scaled2);
        }

        {
            s16 clamped = *(short*)(self + 0x100 + 0x7e);
            if (clamped < -0x1c00) clamped = -0x1c00;
            else if (clamped > 0x1c00) clamped = 0x1c00;
            *(short*)(self + 0x100 + 0x7e) = clamped;
        }
    }

    {
        unsigned int r = func_020093f4(self, 0x48000);
        func_02009138((int*)self, (int)r);
    }
    func_020089d8(self);
    return 1;
}
