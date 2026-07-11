//cpp
extern "C" {
extern int _ZN6Player9GetHealthEv(void *c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void *c, void *state);
extern int func_ov002_020bf30c(void *c, int a);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *c, unsigned int a, int b, int f, unsigned int d);
extern void func_ov002_020d4748(void *c);
extern void func_ov002_020d4540(void *p);
extern void func_ov002_020caf68(void *self);
extern int func_ov002_020e3078(void *self, void *s);
extern int RandomIntInternal(int *seed);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, void *v);
extern void _ZN6Player17SetNoControlStateEhih(void *c, unsigned char a, int b, unsigned char d);

extern unsigned char data_020a0e40;
extern short data_0209f4a0;
extern unsigned char data_0209f2d8;
extern int data_0209caa0[3];
extern unsigned char data_0209f4ac;
extern int data_ov002_0211010c[];
extern int data_ov002_02110154[];
extern int data_ov002_0210e160[];
extern int data_ov002_020ff1b0[4];

int _ZN6Player12St_Walk_InitEv(char *c)
{
    if (_ZN6Player9GetHealthEv(c) == 0) {
        *(unsigned char *)(c + 0x6e3) = 1;
        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211010c);
        return 1;
    }

    if (*(int *)(c + 0x98) > func_ov002_020bf30c(c, 0x28000)) {
        *(int *)(c + 0x98) = func_ov002_020bf30c(c, 0x28000);
    }
    if (*(int *)(c + 0x98) < -0xe000) {
        *(int *)(c + 0x98) = -0xe000;
    }

    if (*(short *)((char *)&data_0209f4a0 + data_020a0e40 * 0x18) == 0) {
        int byte1 = data_0209f2d8;
        int cond = (byte1 == 1);
        if (cond) goto do_anim;
        if ((data_0209caa0[2] & 0x80) == 0) goto merge;
        cond = (byte1 == 2);
        if (cond) goto merge;
do_anim:
        if (*(unsigned char *)(c + 0x703) != 0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0xa0, 0, 0x1000, 0);
        } else {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x47, 0, 0x1000, 0);
        }
    } else {
        func_ov002_020d4748(c);
    }
merge:
    *(short *)(c + 0x6a4) = 0;
    if (*(unsigned char *)((char *)&data_0209f4ac + data_020a0e40 * 0x18) == 0) {
        if (*(int *)(c + 0x98) == 0) {
            *(short *)(c + 0x6a4) = 0x3c;
        }
    }

    *(int *)(c + 0x684) = *(int *)(c + 0x60);
    *(unsigned char *)(c + 0x6e5) = 0;
    *(unsigned char *)(c + 0x70c) = 0;
    if (*(int *)(c + 0x98) > func_ov002_020bf30c(c, 0x24000)) {
        *(unsigned char *)(c + 0x70c) = 1;
    }
    *(unsigned short *)(((long long)(int)(c + 0x6ce)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x100;
    *(unsigned char *)(c + 0x706) = 0;
    func_ov002_020d4540(c);

    func_ov002_020caf68(c);

    *(int *)(c + 0x354) = 0;
    *(int *)(c + 0x358) = 0;
    *(int *)(c + 0x35c) = 0;
    *(unsigned char *)(c + 0x6e3) = 0;
    *(int *)(c + 0xd0) = 0;
    if (func_ov002_020e3078(c, data_ov002_02110154) != 0) {
        *(short *)(c + 0x6b8) = 0x10;
        if (*(short *)((char *)&data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            *(short *)(c + 0x94) = *(short *)(c + 0x6d2);
            *(short *)(c + 0x8e) = *(short *)(c + 0x94);
        }
        if (*(unsigned char *)(c + 0x6ed) != 0) {
            unsigned int rnd = (unsigned int)RandomIntInternal(data_ov002_0210e160);
            int idx = (rnd >> 6) & 3;
            int voiceId = data_ov002_020ff1b0[idx];
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char *)(c + 0x6d9), voiceId, c + 0x74);
        }
    }

    if (*(int *)(c + 8) == 1) {
        if (*(unsigned char *)(c + 0x707) == 0) {
            *(unsigned char *)(c + 0x6ec) = 1;
        }
    }

    if (*(unsigned char *)(c + 0x703) != 0 && *(int *)(c + 0x80) != 0x3000) {
        _ZN6Player17SetNoControlStateEhih(c, 0xd, -1, 0);
    }
    return 1;
}
}
