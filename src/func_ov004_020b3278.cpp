//cpp
/* func_ov004_020b3278 at 0x020b3278 (ov004), size 0x360
 * Compiler mwccarm 1.2/sp2p3, flags:
 * -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 * default-arm guard spelled if(==0){...;break;} return; suppresses the pool-ldr
 * hoist over the strh */
extern "C" {
void DecompressLZ16(int src, int dst);
int func_ov004_020af5e0(int a, void *b, int c, int d);
}

struct Obj {
    virtual int m00(); virtual int m01(); virtual int m02(); virtual int m03();
    virtual int m04(); virtual int m05(); virtual int m06(); virtual int m07();
    virtual int m08(); virtual int m09(); virtual int m10(); virtual int m11();
    virtual int m12(); virtual int m13(); virtual int m14(); virtual int m15();
    virtual int m16(); virtual int m17(); virtual int m18(); virtual int m19();
    virtual int m20(); virtual int m21(); virtual int m22(); virtual int m23();
    virtual int m24(); virtual int m25(); virtual int m26();
};

struct Base { virtual void dummy(); };
typedef void (Base::*PMF)();

struct Pair { int w0; int w1; };
struct S3 { int v[3]; };

extern Obj *data_ov004_020beb68;
extern unsigned char data_ov004_020bf3e8[];
extern int data_ov004_020bf560[];
extern S3 data_ov004_020bc27c;
extern int data_ov004_020bf5d4[];
extern PMF data_ov004_020bf490[];
extern Pair data_ov004_020bf428[];
extern Pair data_ov004_020bf4f8[];

extern "C" void func_ov004_020b3278(char *self, int arg1, short arg2, short arg3, int arg4, int arg5, short arg6)
{
    int a, b;

    if (data_ov004_020beb68->m26() == 2) {
        a = 0x6400000;
        b = 0;
    } else {
        a = 0x6600000;
        b = 0x6400000;
    }

    switch (arg1) {
    case 3: case 4: case 5: case 6:
    case 8: case 9: case 10: case 11: case 12:
    case 14: case 16: case 17: case 18: case 19: case 20: case 21:
        if (data_ov004_020bf3e8[0] != 0)
            return;
        DecompressLZ16(data_ov004_020bf560[arg1], a + 0x7000);
        if (b != 0)
            DecompressLZ16(data_ov004_020bf560[arg1], b + 0x7000);
        *(short *)(self + 0x30) = 0;
        data_ov004_020bf3e8[0] = 1;
        break;
    default:
        if (data_ov004_020bf3e8[1] == 0) {
            DecompressLZ16(data_ov004_020bf560[arg1], a + 0x6000);
            if (b != 0)
                DecompressLZ16(data_ov004_020bf560[arg1], b + 0x6000);
            *(short *)(self + 0x30) = 1;
            data_ov004_020bf3e8[1] = 1;
            break;
        }
        if (data_ov004_020bf3e8[2] == 0) {
            DecompressLZ16(data_ov004_020bf560[arg1], a + 0x6800);
            if (b != 0)
                DecompressLZ16(data_ov004_020bf560[arg1], b + 0x6800);
            *(short *)(self + 0x30) = 2;
            data_ov004_020bf3e8[2] = 1;
            break;
        }
        return;
    }

    *(int *)(self + 0x20) = arg1;
    *(short *)(self + 0x10) = arg2;
    *(short *)(self + 0x12) = arg3;
    *(short *)(self + 0x14) = *(short *)(self + 0x10);
    *(short *)(self + 0x16) = *(short *)(self + 0x12);
    *(int *)(self + 0x1c) = arg4;
    *(int *)(self + 0x18) = arg5;
    *(short *)(self + 0x32) = 0;

    if (arg6 != 0xd) {
        *(short *)(self + 0x2e) = arg6;
    } else {
        switch (arg1) {
        case 11:
            *(short *)(self + 0x2e) = 7;
            break;
        case 3: case 4: case 5: case 6: case 20: case 21:
            *(short *)(self + 0x2e) = 8;
            break;
        case 8: case 14:
            *(short *)(self + 0x2e) = 9;
            break;
        case 0:
            *(short *)(self + 0x2e) = 3;
            break;
        case 1: case 2:
            *(short *)(self + 0x2e) = 4;
            break;
        case 13:
            *(short *)(self + 0x2e) = 0xc;
            break;
        default:
            *(short *)(self + 0x2e) = 0;
            break;
        }
    }

    {
        S3 tmp = data_ov004_020bc27c;
        *(short *)(self + 0x2c) = (short)func_ov004_020af5e0(
            data_ov004_020bf5d4[*(int *)(self + 0x20)],
            self + 0x34,
            tmp.v[*(short *)(self + 0x30)],
            *(int *)(self + 0x20));
    }

    {
        int st = *(short *)(self + 0x2e);
        if (data_ov004_020bf490[st])
            (((Base *)self)->*data_ov004_020bf490[st])();
    }

    {
        short st;
        Pair *e;
        st = *(short *)(self + 0x2e);
        e = &data_ov004_020bf428[st];
        a = data_ov004_020bf428[st].w0;
        b = e->w1;
        *(int *)(self + 0) = b ? a : a;
        *(int *)(self + 4) = b;
        st = *(short *)(self + 0x2e);
        e = &data_ov004_020bf4f8[st];
        a = data_ov004_020bf4f8[st].w0;
        b = e->w1;
        *(int *)(self + 8) = b ? a : a;
        *(int *)(self + 0xc) = b;
    }
}
