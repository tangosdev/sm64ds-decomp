#pragma exceptions off

/* storage 0x023992D8 runtime 0x027CB250 size 0x11C main ARM.
 * Pools: =0x4808010, =0x380FFF4, =0x480819C, =0x4808290, =0x4808032, =0x3B6.
 * Three-slot entity scan; flag bit4 at +0x2FE tests as a bitfield extract. */

struct Sub600 {
    unsigned char pad00[0x1C];
    unsigned short f1C;
};

struct RxSub {
    unsigned char pad00[0xE4];
    unsigned short fE4;
};

struct Ent {
    unsigned char pad00[0x300];
    struct RxSub rx;
    unsigned char padE6[6];
    int f3EC;
};

struct Base {
    unsigned char pad00[0x2FE];
    unsigned short f2FElo:4, flag4:1, f2FEhi:11;
    unsigned char pad300[0x300];
    struct Sub600 s600;
};

int fn_023992D8(void) {
    int *v0;
    unsigned int result;
    unsigned int v2;
    unsigned short *v3;
    int v4;
    int v5;
    unsigned short *v7;
    unsigned short v8;

    *(unsigned short *)0x4808010 = 8;
    if (((struct Base *)*(int *)0x380FFF4)->flag4 && (*(unsigned short *)0x480819C & 1) == 0)
        *(unsigned short *)0x4808290 ^= 1;
    v0 = (int *)0x380FFF4;
    result = ((struct Base *)*v0)->s600.f1C & 8;
    if (result != 0) {
        v2 = 0;
        v3 = (unsigned short *)0x4808032;
        v4 = 0x3B6;
        do {
            v5 = *v0 + 20 * v2;
            result = ((struct Ent *)v5)->rx.fE4;
            if (((struct Ent *)v5)->rx.fE4 != 0) {
                result = ((struct Ent *)v5)->f3EC;
                if ((*(unsigned short *)(result + 0xC) & 0x4000) != 0) {
                    v8 = ((unsigned short *)result)[2] & 0xFF;
                    if (v8 != 0) {
                        v5 = result + 0xC + ((unsigned short *)result)[5];
                        v5 -= 7;
                        v5 &= ~1;
                        if (*(unsigned short *)v5 == 0 && *(unsigned short *)(v5 + 2) == 0) {
                            *(unsigned short *)(result + 4) = 0;
                            *v3 = 0;
                            *v3 = 0x8000;
                            v7 = (unsigned short *)(*v0 + v4);
                            result = (unsigned short)*v7 + 1;
                            *v7 = result;
                        }
                    }
                }
            }
            ++v2;
        } while (v2 < 3);
    }
    return result;
}
