typedef short s16;
typedef unsigned short u16;

#define M(p) ((long long)(int)(p))

extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int a, unsigned int b, int c, int d, int e, void* v, void* cb);
extern short data_02082214[];

#pragma opt_common_subs off

void func_ov024_02111350(char* c){
    s16* tbl = (s16*)(int)M(data_02082214);
    int a1 = ((int)((unsigned int)*(u16*)(c + 0x300 + 0xb2) << 0x1e)) >> 0x10;
    *(int*)(c + 0x5c) = tbl[((u16)a1 >> 4) * 2] * (s16)0x28 + *(int*)(c + 0x3a0);
    if ((u16)*(u16*)(c + 0x300 + 0xb2) < 0x3c) {
        int a2 = ((int)((unsigned int)*(u16*)(c + 0x300 + 0xb2) << 0x1d)) >> 0x10;
        int m = tbl[((u16)a2 >> 4) * 2] * (s16)0xa;
        if (m < 0) m = -m;
        *(int*)(c + 0x60) = *(int*)(c + 0x3a4) + m;
    } else {
        short* w = (short*)M(c + 0x3b0);
        int* q;
        *w = *w + 0x100;
        if (*(short*)(c + 0x300 + 0xb0) > 0x1800) {
            *(short*)(c + 0x300 + 0xb0) = 0x1800;
            *(int*)(c + 0xa8) = 0x5000;
        }
        *(short*)M(c + 0x8e) += *(short*)(c + 0x300 + 0xb0);
        q = (int*)M(c + 0x60);
        *q = *q + *(int*)(c + 0xa8);
    }
    *(int*)(c + 0x3ac) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(unsigned int*)(c + 0x3ac), 0x23, *(int*)(c + 0x5c), *(int*)(c + 0x60),
        *(int*)(c + 0x64), (void*)0, (void*)0);
    if (*(u16*)(c + 0x3b2) == 0x96) {
        *(unsigned char*)(c + 0x3b7) = 2;
    }
}
