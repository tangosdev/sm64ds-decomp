//cpp
// NONMATCHING: regalloc/path-A schedule; idx colors to fp not r5 (div=77)
// NONMATCHING: regalloc/path-A schedule (div=77). Size matches; idx colors to fp not r5;
// path-A setup/y-load order diverges; struct-copy/epilogue shape close.
// NONMATCHING: regalloc/path-A schedule (div~77). Flag+copy+epilogue shape close;
// idx colors to fp not r5; path-A setup/y-load order diverges under default CSE.
extern "C" {
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
extern u8 data_0209f5bc[];
extern u8 data_020a0e40[];
extern u8 data_020a0de8[];
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
#pragma opt_common_subs off
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
        u8 idx = data_020a0e40[0];
        int sel = -1;
        int off = idx * 4;
        int has = 0;
        if (data_020a0de8[off]) {
            if (data_020a0de9[off]) has = 1;
        }
        if (has != 0) {
            if (data_0209b304[0] == 0) {
                int base = data_0208a170;
                int row = 0;
                int yoff = 0;
                int y_lo = 0x18;
                int y_hi = 0x60;
                u8* p = data_020a0de8;
                int ic = 0, ixlo = 0x40, ixhi = 0x70;
                do {
                    u8 x = p[idx * 4 + 2];
                    int col = ic, x_lo = ixlo, x_hi = ixhi, o = yoff;
                    do {
                        if (x >= x_lo && x < x_hi) {
                            u8 y = p[idx * 4 + 3];
                            if (y >= y_lo && y < y_hi) sel = base + o;
                        }
                        x_lo += 0x30; x_hi += 0x30; o += 4; col += 1;
                    } while (col < 3);
                    row += 1; y_lo += 0x48; y_hi += 0x48; yoff += 0xc;
                } while (row < 2);
            } else {
                u8* p = data_020a0de8 + idx * 4;
                u8 x = p[2];
                int base = data_0208a170;
                int i = 0, x_lo = 0x60, x_hi = 0x90, o = 0x18;
                u8 y = p[3];
                do {
                    if (x >= x_lo && x < x_hi) {
                        if ((u32)y >= 0x38u && (u32)y < 0x80u) sel = base + o;
                    }
                    i += 1; x_lo += 0x30; x_hi += 0x30; o += 4;
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
