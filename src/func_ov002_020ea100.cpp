//cpp
extern "C" {

typedef unsigned char u8;
typedef unsigned short u16;

extern int func_ov002_020e73ac(char *arg);
extern int _ZN6Player12Unk_020c9e5cEh(void *thisPtr, int state);
extern void func_ov002_020e6fbc(char *c, int arg);
extern void func_ov002_020e9464(char *p);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thisPtr, int bcaFile, int fixA, int fixB, unsigned int arg4);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int a, int fixA);
extern int func_ov002_020ca0f4(void *player);
extern void GiveVsStars(int idx, int delta);
extern void func_ov002_020e9448(unsigned char *p);

extern char data_ov002_02110924[];
extern char data_ov002_02110944[];
extern unsigned char data_0209f2d8;
extern signed char data_0209f310[];

struct VObj {
    virtual void unk0();
};

#pragma opt_common_subs off
void func_ov002_020ea100(char *c)
{
    char *common;
    int state;

    common = *(char **)(c + 0x438);
    state = func_ov002_020e73ac(c);

    if (_ZN6Player12Unk_020c9e5cEh(common, state)) {
        if (state == 1 || state == 2) {
            func_ov002_020e6fbc(c, 0x14);
            *(u8 *)(c + 0x49c) = 1;
        } else if (state != 3) {
            func_ov002_020e6fbc(c, 0);
            *(u8 *)(c + 0x49c) = 2;
        }

        int *posY = (int *)(((long long)(int)(c + 0x60)) & 0xFFFFFFFFFFFFFFFFLL);

        *(int *)(c + 0x440) = 6;
        *(u16 *)(((long long)(int)(c + 0x4a2)) & 0xFFFFFFFFFFFFFFFFLL) &= ~4;
        *(u16 *)(((long long)(int)(c + 0x4a2)) & 0xFFFFFFFFFFFFFFFFLL) |= 2;
        *(u16 *)(c + 0x490) = 0;

        {
            int *src = (int *)(((long long)(int)(*(int *)(c + 0x438) + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
            int *cache = (int *)(((long long)(int)(c + 0x454)) & 0xFFFFFFFFFFFFFFFFLL);
            *(int *)(c + 0x454) = src[0];
            *(int *)(c + 0x458) = src[1];
            *(int *)(c + 0x45c) = src[2];
            *(int *)(c + 0x5c) = cache[0];
            *(int *)(c + 0x60) = cache[1];
            *(int *)(c + 0x64) = cache[2];
            *posY += 0x1e000;
        }

        *(short *)(c + 0x94) = *(short *)(*(int *)(c + 0x438) + 0x8e);
        *(u16 *)(c + 0x8c) = 0;

        if (*(u8 *)(common + 0x703) != 0) {
            *(int *)(c + 0x80) = 0x3000;
            *(int *)(c + 0x84) = 0x3000;
            *(int *)(c + 0x88) = 0x3000;
        } else {
            *(int *)(c + 0x80) = 0x1000;
            *(int *)(c + 0x84) = 0x1000;
            *(int *)(c + 0x88) = 0x1000;
        }

        func_ov002_020e9464(c);

        if (*(u8 *)(common + 0x706) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x30c, *(int *)(data_ov002_02110924 + 4), 0x40000000, 0x1000, 0);
        } else {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x30c, *(int *)(data_ov002_02110944 + 4), 0x40000000, 0x1000, 0);
        }

        {
            struct VObj *vobj = (struct VObj *)(c + 0x3d4);
            { u16 *_p = (u16 *)(((long long)(int)(c + 0x4a2)) & 0xFFFFFFFFFFFFFFFFLL); *_p = (*_p & ~1) | 1; }
            vobj->unk0();
        }

        if (*(int *)(c + 0x43c) == 9) {
            _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0, 0x7f000);
        }
    } else {
        if (func_ov002_020ca0f4(common) != 0) {
            int *posY2 = (int *)(((long long)(int)(c + 0x60)) & 0xFFFFFFFFFFFFFFFFLL);
            int *s = (int *)(((long long)(int)(*(int *)(c + 0x438) + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
            *(int *)(c + 0x5c) = s[0];
            *(int *)(c + 0x60) = s[1];
            *(int *)(c + 0x64) = s[2];
            *posY2 += 0xc8000;
        } else {
            int ok = (data_0209f2d8 == 1);
            if (ok) {
                unsigned char idx = *(u8 *)(common + 0x6d8);
                if (data_0209f310[idx] != 0) {
                    GiveVsStars(idx, -1);
                }
            }
            *(int *)(c + 0x440) = 8;
            *(int *)(c + 0xa8) = 0x20000;
            *(int *)(c + 0x98) = 0xc000;
            func_ov002_020e9448((unsigned char *)c);
            *(int *)(((long long)(int)(c + 0x128)) & 0xFFFFFFFFFFFFFFFFLL) &= ~1;
        }
    }

    *(int *)(c + 0x4b8) = 0;
    *(int *)(c + 0x4b4) = *(int *)(c + 0x4b8);
}

}
