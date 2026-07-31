//cpp
extern "C" {
void func_ov002_020bf5e0(void*);
int _ZN6Player6IsAnimEj(void*, unsigned int);
void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned int, int);
void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int, int, int);
int Player_ScaleByCharFactor(void*, int);
int _ZNK9Animation12WillHitFrameEi(void*, int);
void func_ov002_020d4c30(void*);
}

extern unsigned char data_020a0e40;
extern unsigned char data_0209f4ac;
extern unsigned short data_0209f49c;
extern unsigned char data_0209f4ae;
extern short data_0209f4a0;
extern unsigned char data_0209f2d8;
extern int data_0209caa0[];

static inline int* GetAnim(char* c)
{
    return (int*)(*(char**)(c + _ZNK6Player14GetBodyModelIDEjb(c, (unsigned char)*(int*)(c + 8), 0) * 4 + 0xdc) + 0x50);
}

static inline unsigned int AnimSpeed(char* c)
{
    return (unsigned short)(GetAnim(c)[2] >> 12);
}

extern "C" void func_ov002_020d4748(char* c)
{
    int i = data_020a0e40 * 0x18;
    int flag = 0;

    if (*(unsigned char*)((char*)&data_0209f4ac + i) == 0) {
        if (!(*(unsigned short*)((char*)&data_0209f49c + i) & 0x800))
            flag = 1;
    } else {
        int k = (*(unsigned char*)((char*)&data_0209f4ae + i) != 2) ? 0xdc7 : 0xe00;
        if (*(short*)((char*)&data_0209f4a0 + i) < k + (*(unsigned char*)(c + 0x710) << 7))
            flag = 1;
    }
    *(unsigned char*)(c + 0x710) = flag;

    if (*(unsigned char*)(c + 0x703) != 0) {
        int i2;
        short v;
        func_ov002_020bf5e0(c);
        if (_ZN6Player6IsAnimEj(c, 0x9f) != 0)
            return;
        i2 = data_020a0e40 * 0x18;
        v = *(short*)((char*)&data_0209f4a0 + i2);
        if (v != 0) {
            if (flag != 0) {
                int spd;
                if (*(unsigned char*)((char*)&data_0209f4ac + i2) == 0) {
                    spd = (*(unsigned short*)(c + 0x6a4) != 0) ? 0x1000 : 0x2000;
                } else {
                    spd = v * 3;
                }
                _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x9e, 0, spd, 0);
                return;
            } else {
                _ZN6Player7SetAnimEji5Fix12IiEj(c, 0xa4, 0, 0x2000, 0);
                return;
            }
        } else {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x9d, 0x40000000, 0x1000, 0);
            return;
        }
    } else {
        int i3 = data_020a0e40 * 0x18;
        if (*(unsigned char*)((char*)&data_0209f4ac + i3) != 0) {
            unsigned char* p = (unsigned char*)(((long long)(int)(c + 0x6e3)) & 0xFFFFFFFFFFFFFFFFLL);
            unsigned char d;
            int b;
            int i4;
            int thr;
            *p = *p + 1;
            i4 = data_020a0e40 * 0x18;
            thr = (*(unsigned char*)((char*)&data_0209f4ae + i4) != 2) ? 0x955 : 0xaaa;
            if (*(short*)((char*)&data_0209f4a0 + i4) > thr)
                *(unsigned char*)(c + 0x6e3) = 0;
            d = data_0209f2d8;
            b = (d == 1);
            if (b == false) {
                if (data_0209caa0[2] & 0x80) {
                    int b2 = (d == 2);
                    if (b2 == false) goto after6e3;
                }
                *(unsigned char*)(c + 0x6e3) = 0;
            }
        after6e3:;
        } else {
            *(unsigned char*)(c + 0x6e3) = 0;
        }

        if (*(int*)(c + 0x68c) > 0x32000) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x7e, 0,
                *(short*)((char*)&data_0209f4a0 + data_020a0e40 * 0x18), AnimSpeed(c));
        } else if (*(unsigned char*)(c + 0x6e3) >= 0x17) {
            *(unsigned char*)(c + 0x6e3) = 0x17;
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x61, 0, 0x1000, AnimSpeed(c));
        } else if (*(int*)(c + 0x658) == 4 && *(short*)((char*)&data_0209f4a0 + data_020a0e40 * 0x18) == 0) {
            _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(*(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x47, 0, 0x1000, 0);
        } else if (*(unsigned char*)(c + 0x710) != 0) {
            unsigned int s5 = AnimSpeed(c);
            int spd = *(int*)(c + 0x98) >> 3;
            if (spd < 0x400)
                spd = 0x400;
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x48, 0, spd, s5);
        } else {
            int t = 0x1000 - *(int*)(c + 0x558);
            int f = (int)((((long long)t << 10) + 0x800) >> 12);
            int v;
            int diff = Player_ScaleByCharFactor(c, 0x1c000) - *(int*)(c + 0x98);
            v = (int)(((long long)diff * f + 0x800) >> 12);
            if (v < 0)
                v = 0;
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x3f, 0, v + 0x2800, AnimSpeed(c));
        }

        if (_ZNK9Animation12WillHitFrameEi(GetAnim(c), 4) != 0
            || _ZNK9Animation12WillHitFrameEi(GetAnim(c), 0x13) != 0) {
            func_ov002_020d4c30(c);
        }
    }
}
