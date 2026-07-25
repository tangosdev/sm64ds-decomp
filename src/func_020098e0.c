typedef short s16;
typedef unsigned char u8;
typedef long long s64;

#define AT(p,off) ((void*)(int)(((long long)(int)((char*)(p)+(off)))&0xFFFFFFFFFFFFFFFFLL))

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
        int s0 = data_02086e9c[0];
        int s1 = data_02086e9c[1];
        int s2 = data_02086e9c[2];
        src[0] = s0;
        src[1] = s1;
        src[2] = s2;

        {
            char *p110 = *(char**)(self + 0x110);
            Vec3_RotateYAndTranslate(pos, (int*)(p110 + 0x5c), *(short*)(p110 + 0x8e), src);
        }

        result = func_020091f8(self, (int)pos, *(short*)(self + 0x186), 0);
        if (result != 0) {
            *(u8*)(self + 0x1a6) = 0;
        }
    } else {
        int idx = data_020a0e40;
        s16 *hp = (s16*)AT(self, 0x17c);
        int off = (int)(((long long)idx) & 0xFFFFFFFFFFFFFFFFLL) * 0x18;

        {
            s16 t1 = *(s16*)((char*)data_0209f4a2 + off);
            *hp = (s16)(*hp - (int)((((s64)t1 << 9) + 0x800) >> 12));
        }

        {
            s16 a186 = *(s16*)((char*)self + 0x186);
            int d = (s16)(*(s16*)((char*)self + 0x17c) - a186);
            s16 lim = 0x5000;
            if (d < -lim) d = -lim;
            else if (d > 0x5000) d = lim;
            *(s16*)((char*)self + 0x17c) = (s16)(a186 + d);
        }

        {
            s16 t2 = *(s16*)((char*)data_0209f4a4 + off);
            s16 *hp2 = (s16*)AT(self, 0x17e);
            *hp2 = (s16)(*hp2 + (int)((((s64)t2 << 9) + 0x800) >> 12));
        }

        {
            int v = *(s16*)((char*)self + 0x17e);
            s16 lim2 = 0x1c00;
            if (v < -lim2) v = -lim2;
            else if (v > 0x1c00) v = lim2;
            *(s16*)((char*)self + 0x17e) = (s16)v;
        }
    }

    {
        unsigned int r = func_020093f4(self, 0x48000);
        func_02009138((int*)self, (int)r);
    }
    func_020089d8(self);
    return 1;
}
