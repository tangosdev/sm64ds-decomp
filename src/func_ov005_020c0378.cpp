//cpp
// NONMATCHING: size-exact regperm (div=44). has/sel/path-A schedule now matches;
// idx colors to r1 not r5; has to r3 not r2; path-B y load early.
extern "C" {
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
extern u8 data_0209f5bc[];
extern unsigned char data_020a0e40;
struct S { unsigned char a, b, c, d; };
extern struct S data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_0209b304[];
extern int data_0208a170;
extern int data_0208a174;
extern u8 data_0209b2fc[];
extern char data_0209f61c;
typedef struct Mid { int a[5]; } Mid;
typedef struct Entry {
    u16 id; int unk4, unk8, unkC, unk10; Mid mid; int unk28, unk2c, unk30;
} Entry;
extern Entry data_ov005_020c24d8[];
extern u16 data_0209b308;
extern Mid data_0209b31c;
extern int data_ov005_020c24dc[];
int func_ov005_020c00b4(void* self, int n);
void func_ov005_020c1688(void* self, int n);
void func_02012790(int n);
void _ZN5Scene9SetFadersEP15FaderBrightness(void* p);
void func_0202ec9c(void* p, int v);
int _ZN5Scene14StartSceneFadeEjjt(u32 a, u32 b, u16 c);
void _ZN5Sound22StopLoadedMusic_Layer1Ej(u32 n);
// 6e: preheader emission order
#pragma opt_loop_invariants off
void func_ov005_020c0378(char* c)
{
    int (**vt)(void*);
    void* obj = *(void**)data_0209f5bc;
    vt = *(int(***)(void*))obj;
    if (vt[5](obj) == 0) return;
    if (*(u8*)(c + 0xac) != 0) return;
    if (*(u8*)(c + 0x54) == 1) return;
    if (*(int*)(c + 0x90) > 0) return;
    if (*(int*)(c + 0x94) > 0) return;
    if (*(int*)(c + 0x98) > 0) return;

    {
    int row, yoff, y_lo, y_hi, base, ic, ixlo, ixhi;
    unsigned int idx;
    int sel;
    int ok;
    idx = data_020a0e40;
    sel = -1;
    ok = 0;
    if (data_020a0de8[idx].a != 0) {
        if (data_020a0de8[idx].b != 0) ok = 1;
    }
    if (ok) {
        if (data_0209b304[0] == 0) {
            row = 0;
            base = data_0208a170;
            yoff = row;
            y_lo = 0x18;
            y_hi = 0x60;
            ic = row;
            ixlo = 0x40;
            ixhi = 0x70;
            do {
                unsigned char x = ((unsigned char *)&data_020a0de8[idx])[2];
                int col = ic, x_lo = ixlo, x_hi = ixhi, o = yoff;
                do {
                    if (x >= x_lo && x < x_hi) {
                        unsigned char y = data_020a0de8[idx].d;
                        if (y >= y_lo && y < y_hi) sel = base + o;
                    }
                    x_lo += 0x30; x_hi += 0x30; o += 4; col += 1;
                } while (col < 3);
                row += 1; y_lo += 0x48; y_hi += 0x48; yoff += 0xc;
            } while (row < 2);
        } else {
            unsigned char *p = (unsigned char *)&data_020a0de8[idx];
            unsigned char x = p[2];
            base = data_0208a170;
            int i = 0, x_lo = 0x60, x_hi = 0x90;
            idx = 0x18;
            unsigned char y = p[3];
            do {
                if (x >= x_lo && x < x_hi) {
                    if ((u32)y >= 0x38u && (u32)y < 0x80u) sel = base + (int)idx;
                }
                i += 1; x_lo += 0x30; x_hi += 0x30; idx += 4;
            } while (i < 3);
        }
    }

    if (sel < 0) return;
    if (func_ov005_020c00b4(c, sel) == 0) return;

    {
        int mul = sel * 0x34;
        Entry* src = (Entry*)((char*)data_ov005_020c24d8 + mul);
        data_0208a174 = sel;
        data_0209b308 = src->id;
        *(int*)((char*)&data_0209b308 + 4) = src->unk4;
        *(int*)((char*)&data_0209b308 + 8) = src->unk8;
        *(int*)((char*)&data_0209b308 + 0xc) = src->unkC;
        *(int*)((char*)&data_0209b308 + 0x10) = src->unk10;
        data_0209b31c = src->mid;
        *(int*)((char*)&data_0209b308 + 0x28) = src->unk28;
        *(int*)((char*)&data_0209b308 + 0x2c) = src->unk2c;
        *(int*)((char*)&data_0209b308 + 0x30) = src->unk30;

        func_ov005_020c1688(c, sel);
        func_02012790(0x1e);
        _ZN5Scene9SetFadersEP15FaderBrightness(&data_0209f61c);
        func_0202ec9c(&data_0209f61c, 0);
        *(int*)((char*)&data_0209b308 + 0x30) = *(int*)(c + 0x5c);
        {
            int k = data_0208a174;
            int m = k * 0x34;
            _ZN5Scene14StartSceneFadeEjjt(
                *(u16*)((char*)data_ov005_020c24d8 + m),
                *(int*)((char*)data_ov005_020c24dc + m),
                0);
        }
        _ZN5Sound22StopLoadedMusic_Layer1Ej(0x1e);
        *(u8*)(c + 0xac) = 1;
        data_0209b2fc[0] = 1;
    }
    }
}
}
