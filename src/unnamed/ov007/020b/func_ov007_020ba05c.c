/* NONMATCHING -- compiles, but the result is 0x22c bytes against the ROM's 0x284:
 * 88 bytes SHORT, so this is an incomplete reconstruction rather than a near-miss.
 *
 * Three defects had to be fixed before it would compile at all, and each was a shadow
 * struct disagreeing with itself: `array24` and `array28` are members of StructObj0 but
 * were being read off StructObj20, and `f2C` was used on StructAInner without being
 * declared. The pad split below keeps every later offset where it was (pad1 spanned
 * 0x0c..0x30, so 0x20 of pad puts f2C at 0x2c and leaves f30 at 0x30).
 *
 * Those fixes are right, and they are what exposed the 88-byte gap the compile error
 * had been hiding.
 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

extern void* data_ov007_02104ba0;
extern void* data_ov007_0210342c;

extern int func_ov007_020c1da0(int);
extern void func_ov007_020c6724(void*);
extern int func_ov007_020c65bc(int);
extern void func_ov007_020bdbcc(s16, s16);
extern int func_ov007_020c6550(void*);
extern void func_ov007_020bb09c(void);

typedef struct StructA {
    void* p0;
    void* p4;
    void* p8;
    u8 pad0[0x18];
    void* p24;
    u8 pad1[0x8];
    u32 f30;
    u32 f34;
} StructA;

typedef struct StructB {
    u8 pad0[8];
    u16 f8;
    u16 fA;
    u16 fC;
    u8 pad1[6];
    u16 f14;
    u8 pad2[0xe];
    u32 f24;
    u32 f28;
    u32 f2C;
    u8 pad3[0xc];
    s16 f3C;
    s16 f3E;
} StructB;

typedef struct StructBHolder {
    u8 pad0[0x2c];
    u32 f2C;
    u8 pad1[0x20];
    StructB* b;
} StructBHolder;

typedef struct StructAHolder {
    StructA* a;
} StructAHolder;

typedef struct StructObj20 {
    u8 pad0[8];
    u16 f8;
} StructObj20;

typedef struct StructObj0 {
    u8 pad0[0x20];
    StructObj20* obj20;
    u32* array24;
    u32* array28;
} StructObj0;

typedef struct StructAInner {
    StructObj0* obj0;
    u8 pad0[4];
    s16* p8;
    u8 pad1[0x20];
    u32 f2C;
    u32 f30;
    u32 f34;
} StructAInner;

int func_ov007_020ba05c(void) {
    s32 res = 0;
    StructAHolder* aHolder = (StructAHolder*)data_ov007_02104ba0;
    StructBHolder* bHolder = (StructBHolder*)data_ov007_0210342c;
    StructAInner* a = (StructAInner*)aHolder->a;
    StructB* b = bHolder->b;

    if (a->f30 != 0 && b->fC != 0 && b->f14 == 0 && b->f24 >= 1) {
        if (func_ov007_020c1da0(0) == 0) {
            a->f30 = 0;
            func_ov007_020c6724(a->obj0);
            bHolder->b->f2C = 0; // or bHolder->f2C? wait
        }
    }

    if (a->obj0->obj20->f8 == 0) {
        a->f34 = 0;
    } else {
        a->obj0 = (StructObj0*)((char*)a->f34 + 0x1000);
    }

    if (func_ov007_020c1da0(0) != 0) {
        goto end;
    }

    s16 val8 = *a->p8;
    if (val8 == 6 || val8 == 7 || a->f30 != 0) {
        goto end;
    }

    if (b->fC != 0) {
        s32 ret5 = 0;
        StructObj0* obj0 = a->obj0;
        StructObj20* obj20 = obj0->obj20;
        s32 check = 0;
        if (obj20->f8 >= 2) {
            s32 sb = b->f3E;
            u32 sl1 = obj0->array28[obj20->f8 - 2];
            u32 lr = (u32)(sb * sb);
            s32 sb_val = (s32)(sl1 >> 12) - (s32)b->fA;
            u32 sl2 = obj0->array24[obj20->f8 - 2];
            s32 r7 = b->f3C;
            s32 sb_sq = sb_val * sb_val;
            s32 sb_acc = r7 * r7 + lr;
            s32 r7_val = (s32)(sl2 >> 12) - (s32)b->f8;
            s32 r7_acc = r7_val * r7_val + sb_sq;
            if (sb_acc >= 0x100 && r7_acc <= 4) {
                check = 1;
            }
        }

        if (check == 0) {
            ret5 = func_ov007_020c65bc((s32)a->f34 >> 12);
            func_ov007_020bdbcc(b->f3C, b->f3E);
        } else {
            bHolder->b->f2C |= 1;
        }

        if (ret5 == 2) {
            func_ov007_020c6550(a->obj0);
            a->f34 = 0;
            res = 1;
            goto end;
        } else if (ret5 == 1) {
            bHolder->b->f28 = 0;
            goto end;
        }
    } else {
        if (bHolder->b->f24 == 1) {
            s32 ret = func_ov007_020c6550(a->obj0);
            if (ret != 0) {
                if (ret == 2) {
                    res = 1;
                }
                a->f2C = *(u32*)((char*)a->obj0 + 8);
                a->f34 = 0;
                bHolder->b->f2C |= 1;
            }
        }
    }

end:
    func_ov007_020bb09c();
    return res;
}
