struct Ddc {
    int unk0;
    int unk4;
    int unk8;
    int unkC;
    int unk10;
    int unk14;
    int unk18;
};

typedef struct {
    char pad[0x3C];
    int unk3C;
    int unk40;
} Obj40;

extern struct Ddc data_ov007_02102ddc;
extern int *data_ov007_02103478;
extern int data_ov007_020d7a5c;
extern unsigned short data_ov007_020d7a68[];
extern short data_ov007_020d7a84[];
extern int data_ov007_02102dd8;
extern int data_ov007_02103464;
extern int data_ov007_02103468;
extern Obj40 *data_ov007_0210346c;
extern Obj40 *data_ov007_02103460;

extern void MultiStore32Bytes(unsigned val, int *dst, int len);
extern int func_ov007_020b8ec0(int x);
extern int func_ov007_020c94a0(int a, unsigned int b);
extern void func_ov007_020b8b80(void *a, int b, int c);
extern void func_ov007_020bdeb0(int a);
extern void func_ov007_020c1d78(int i);
extern void func_ov007_020c1d8c(int i);
extern void func_ov007_020b7a44(void);
extern void func_ov007_020b883c(void);

void func_ov007_020b7a78(void)
{
    switch (data_ov007_02102ddc.unk4) {
    case 0:
        if (data_ov007_02102ddc.unk10 == 0) {
            if (data_ov007_02102ddc.unkC == 0) {
                volatile int sp0;
                sp0 = 0;
                MultiStore32Bytes(sp0, data_ov007_02103478, data_ov007_020d7a5c);
            } else if (data_ov007_02102ddc.unkC >= 0xAF0) {
                data_ov007_02102dd8 = 0xF;
                data_ov007_02103464 = 0x80;
                data_ov007_02103468 = data_ov007_02102ddc.unk18;
                func_ov007_020b8b80(0, func_ov007_020c94a0(0, func_ov007_020b8ec0(data_ov007_020d7a68[data_ov007_02102ddc.unk0])), 0);
                data_ov007_02102ddc.unk10 = 1;
                if (data_ov007_02102ddc.unk0 == 5) {
                    func_ov007_020bdeb0(0x10);
                } else if (data_ov007_02102ddc.unk0 == 0xB) {
                    func_ov007_020bdeb0(0x11);
                }
            }
        }
        data_ov007_02102ddc.unkC += 0x100;
        if (data_ov007_02102ddc.unkC > 0x1000) {
            data_ov007_02102ddc.unkC = 0x1000;
            if (data_ov007_02102ddc.unk0 == 1 || data_ov007_02102ddc.unk0 == 2 ||
                (unsigned int)(data_ov007_02102ddc.unk0 - 6) <= 2U) {
                data_ov007_02102ddc.unk4 = 1;
            } else {
                data_ov007_02102ddc.unk4 = 2;
            }
        }
        break;
    case 1:
        data_ov007_02102ddc.unk8 += 1;
        if (data_ov007_02102ddc.unk8 >= 0x3C) {
            data_ov007_02102ddc.unk8 = 0;
            data_ov007_02102ddc.unk4 = 2;
        }
        break;
    case 2:
        if (data_ov007_02102ddc.unk8 == 0 &&
            (data_ov007_02102ddc.unk0 == 9 || data_ov007_02102ddc.unk0 == 0xC)) {
            func_ov007_020c1d78(0);
        }
        {
            short v = data_ov007_020d7a84[data_ov007_02102ddc.unk0];
            if (v > 0) {
                data_ov007_02102ddc.unk8 += 1;
                if (data_ov007_02102ddc.unk8 >= v) {
                    func_ov007_020b7a44();
                }
            }
        }
        break;
    case 3:
        if (data_ov007_02102ddc.unk0 == 9 || data_ov007_02102ddc.unk0 == 0xC) {
            func_ov007_020c1d8c(0);
        }
        if (data_ov007_02102ddc.unk10 != 0 && data_ov007_02102ddc.unkC <= 0xAF0) {
            volatile int sp4;
            sp4 = 0;
            data_ov007_02102ddc.unk10 = 0;
            MultiStore32Bytes(sp4, data_ov007_02103478, data_ov007_020d7a5c);
        }
        data_ov007_02102ddc.unkC -= 0x100;
        if (data_ov007_02102ddc.unkC < 0) {
            data_ov007_02102ddc.unkC = 0;
            func_ov007_020b883c();
        }
        break;
    case 4:
    default:
        break;
    }

    if (data_ov007_02102ddc.unkC > 0) {
        int a = data_ov007_02102ddc.unkC;
        int result = (int)(((long long)a * (0x2000 - a)) >> 0xC);
        data_ov007_0210346c->unk40 = result;
        data_ov007_0210346c->unk3C = data_ov007_0210346c->unk40;
        data_ov007_02103460->unk40 = result;
        data_ov007_02103460->unk3C = data_ov007_02103460->unk40;
    } else {
        data_ov007_0210346c->unk40 = 0;
        data_ov007_0210346c->unk3C = data_ov007_0210346c->unk40;
        data_ov007_02103460->unk40 = 0;
        data_ov007_02103460->unk3C = data_ov007_02103460->unk40;
    }
}
